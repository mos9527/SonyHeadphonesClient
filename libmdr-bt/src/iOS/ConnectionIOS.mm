#import <ExternalAccessory/ExternalAccessory.h>
#import <Foundation/Foundation.h>
#import <dispatch/dispatch.h>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <strings.h>

#include "../Utils.hpp"
#include <mdr-bt/ConnectionIOS.h>

static NSString* const kProtocolV1 = @"jp.co.sony.songpal.mdr.link";
static NSString* const kProtocolV2 = @"jp.co.sony.songpal.mdr.link2";
// Matches Sound Connect's IapSession
constexpr size_t kMaxWriteSize = 2048;
constexpr size_t kReadChunkSize = 1024;

static NSString* ProtocolForServiceUUID(const char* uuid)
{
    if (strcasecmp(uuid, MDR_SERVICE_UUID_V2) == 0)
        return kProtocolV2;
    if (strcasecmp(uuid, MDR_SERVICE_UUID_V1) == 0)
        return kProtocolV1;
    return nil;
}

static bool SupportsMdr(EAAccessory* accessory)
{
    NSArray<NSString*>* protocols = accessory.protocolStrings;
    return [protocols containsObject:kProtocolV1] || [protocols containsObject:kProtocolV2];
}

static void FormatAccessoryAddress(EAAccessory* accessory, char (&out)[18])
{
    uint64_t hash = 0xcbf29ce484222325ull; // FNV-1a
    const auto mix = [&hash](NSString* value)
    {
        for (const char* p = value.UTF8String; p && *p; ++p)
            hash = (hash ^ static_cast<uint8_t>(*p)) * 0x100000001b3ull;
        hash *= 0x100000001b3ull;
    };
    mix(accessory.modelNumber);
    if (accessory.serialNumber.length)
        mix(accessory.serialNumber);
    else
        mix([NSString stringWithFormat:@"%lu", static_cast<unsigned long>(accessory.connectionID)]);
    uint8_t b[6];
    for (int i = 0; i < 6; ++i)
        b[i] = static_cast<uint8_t>(hash >> (8 * (5 - i)));
    b[0] = (b[0] & 0xfc) | 0x02; // Locally administered, unicast
    snprintf(out, sizeof(out), "%02X:%02X:%02X:%02X:%02X:%02X", b[0], b[1], b[2], b[3], b[4], b[5]);
}

struct MDRIOSState
{
    std::mutex mutex;
    std::condition_variable condition;
    uint64_t generation{};
    MDRResult result{MDR_RESULT_ERROR_NO_CONNECTION};
    mdr::String error;
    mdr::Vector<uint8_t> rx;
    mdr::Vector<uint8_t> tx;
    bool enumPending{};
    bool enumReady{};
    MDRDeviceInfo* devices{};
    int deviceCount{};

    ~MDRIOSState() { mdr::MDRAllocator<MDRDeviceInfo>().deallocate(devices); }
};

@interface MDRAccessorySession : NSObject <NSStreamDelegate, EAAccessoryDelegate>
- (instancetype)initWithState:(std::shared_ptr<MDRIOSState>)state generation:(uint64_t)generation;
- (void)openAddress:(NSString*)address protocol:(NSString*)protocol;
- (void)close;
- (void)flush;
@end

@interface MDRAccessoryOwner : NSObject
@property (nonatomic, strong) MDRAccessorySession* session;
@end

@implementation MDRAccessoryOwner
@end

@implementation MDRAccessorySession
{
    std::shared_ptr<MDRIOSState> _state;
    uint64_t _generation;
    EASession* _session;
    int _openStreams;
}

- (instancetype)initWithState:(std::shared_ptr<MDRIOSState>)state generation:(uint64_t)generation
{
    self = [super init];
    if (self)
    {
        _state = std::move(state);
        _generation = generation;
    }
    return self;
}

- (void)fail:(NSString*)message
{
    [self close];
    std::scoped_lock lock(_state->mutex);
    if (_state->generation != _generation)
        return;
    _state->result = MDR_RESULT_ERROR_NET;
    _state->error = message.UTF8String ?: "Unknown error";
    _state->condition.notify_all();
}

- (void)openAddress:(NSString*)address protocol:(NSString*)protocol
{
    EAAccessory* target = nil;
    for (EAAccessory* accessory in [EAAccessoryManager sharedAccessoryManager].connectedAccessories)
    {
        char candidate[18];
        FormatAccessoryAddress(accessory, candidate);
        if (strcasecmp(candidate, address.UTF8String) == 0)
        {
            target = accessory;
            break;
        }
    }
    if (!target)
    {
        [self fail:@"Device not found (not connected?)"];
        return;
    }
    if (![target.protocolStrings containsObject:protocol])
    {
        [self fail:[NSString stringWithFormat:@"Device does not support %@", protocol]];
        return;
    }
    _session = [[EASession alloc] initWithAccessory:target forProtocol:protocol];
    if (!_session)
    {
        [self fail:@"Unable to open session (in use by another app?)"];
        return;
    }
    target.delegate = self;
    for (NSStream* stream in @[_session.inputStream, _session.outputStream])
    {
        stream.delegate = self;
        [stream scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
        [stream open];
    }
}

- (void)close
{
    if (!_session)
        return;
    if (_session.accessory.delegate == self)
        _session.accessory.delegate = nil;
    for (NSStream* stream in @[_session.inputStream, _session.outputStream])
    {
        stream.delegate = nil;
        [stream close];
        [stream removeFromRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    }
    _session = nil;
    _openStreams = 0;
}

- (void)readAvailable
{
    NSInputStream* input = _session.inputStream;
    uint8_t buffer[kReadChunkSize];
    while (input.hasBytesAvailable)
    {
        const NSInteger count = [input read:buffer maxLength:sizeof(buffer)];
        if (count < 0)
        {
            [self fail:input.streamError.localizedDescription ?: @"Read failed"];
            return;
        }
        if (count == 0)
            return;
        std::scoped_lock lock(_state->mutex);
        if (_state->generation != _generation)
            return;
        _state->rx.insert(_state->rx.end(), buffer, buffer + count);
        _state->condition.notify_all();
    }
}

- (void)flush
{
    NSOutputStream* output = _session.outputStream;
    if (!output)
        return;
    NSString* error = nil;
    {
        std::scoped_lock lock(_state->mutex);
        if (_state->generation != _generation)
            return;
        auto& tx = _state->tx;
        while (!tx.empty() && output.hasSpaceAvailable)
        {
            const NSInteger count = [output write:tx.data() maxLength:tx.size()];
            if (count < 0)
            {
                error = output.streamError.localizedDescription ?: @"Write failed";
                break;
            }
            if (count == 0)
                break;
            tx.erase(tx.begin(), tx.begin() + count);
        }
        if (tx.empty())
            _state->condition.notify_all();
    }
    if (error)
        [self fail:error];
}

- (void)stream:(NSStream*)stream handleEvent:(NSStreamEvent)event
{
    if (!_session)
        return;
    switch (event)
    {
    case NSStreamEventOpenCompleted:
    {
        _openStreams |= stream == _session.inputStream ? 1 : 2;
        if (_openStreams != 3)
            break;
        std::scoped_lock lock(_state->mutex);
        if (_state->generation == _generation && _state->result == MDR_RESULT_INPROGRESS)
        {
            _state->result = MDR_RESULT_OK;
            _state->condition.notify_all();
        }
        break;
    }
    case NSStreamEventHasBytesAvailable:
        [self readAvailable];
        break;
    case NSStreamEventHasSpaceAvailable:
        [self flush];
        break;
    case NSStreamEventErrorOccurred:
        [self fail:stream.streamError.localizedDescription ?: @"Stream error"];
        break;
    case NSStreamEventEndEncountered:
        [self fail:@"Connection closed by remote"];
        break;
    default:
        break;
    }
}

- (void)accessoryDidDisconnect:(EAAccessory*)accessory
{
    [self fail:@"Device disconnected"];
}

@end

struct MDRConnectionIOS
{
    MDRConnection mdrConn;
    MDRAccessoryOwner* owner;
    std::shared_ptr<MDRIOSState> state{std::make_shared<MDRIOSState>()};
    mdr::String errorSnapshot;

    MDRConnectionIOS()
    {
        owner = [[MDRAccessoryOwner alloc] init];
        mdrConn = {
            .user = this,
            .connect = Connect,
            .disconnect = Disconnect,
            .recv = Recv,
            .send = Send,
            .poll = Poll,
            .getDevicesList = GetDevicesList,
            .freeDevicesList = FreeDevicesList,
            .getLastError = GetLastError,
        };
        dispatch_async(dispatch_get_main_queue(), ^{
            [[EAAccessoryManager sharedAccessoryManager] registerForLocalNotifications];
        });
    }

    ~MDRConnectionIOS()
    {
        Disconnect(this);
        owner = nil;
    }

    static MDRResult Connect(void* user, const char* address, const char* serviceUUID)
    {
        auto* self = static_cast<MDRConnectionIOS*>(user);
        @autoreleasepool
        {
            const auto state = self->state;
            std::scoped_lock lock(state->mutex);
            if (!address || !serviceUUID)
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            NSString* protocol = ProtocolForServiceUUID(serviceUUID);
            if (!protocol)
            {
                state->error = mdr::Format("No External Accessory protocol for service {}", serviceUUID);
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            }
            NSString* addressString = [NSString stringWithUTF8String:address];
            MDRAccessoryOwner* owner = self->owner;
            const uint64_t generation = ++state->generation;
            state->result = MDR_RESULT_INPROGRESS;
            state->error.clear();
            state->rx.clear();
            state->tx.clear();
            dispatch_async(dispatch_get_main_queue(), ^{
                {
                    std::scoped_lock stateLock(state->mutex);
                    if (state->generation != generation)
                        return;
                }
                [owner.session close];
                owner.session = [[MDRAccessorySession alloc] initWithState:state generation:generation];
                [owner.session openAddress:addressString protocol:protocol];
            });
            state->condition.notify_all();
            return MDR_RESULT_INPROGRESS;
        }
    }

    static void Disconnect(void* user)
    {
        auto* self = static_cast<MDRConnectionIOS*>(user);
        const auto state = self->state;
        MDRAccessoryOwner* owner = self->owner;
        std::scoped_lock lock(state->mutex);
        const uint64_t generation = ++state->generation;
        state->result = MDR_RESULT_ERROR_NO_CONNECTION;
        state->rx.clear();
        state->tx.clear();
        state->condition.notify_all();
        dispatch_async(dispatch_get_main_queue(), ^{
            {
                std::scoped_lock stateLock(state->mutex);
                if (state->generation != generation)
                    return;
            }
            [owner.session close];
            owner.session = nil;
        });
    }

    static MDRResult Recv(void* user, char* dst, int size, int* pReceived)
    {
        auto* self = static_cast<MDRConnectionIOS*>(user);
        *pReceived = 0;
        std::unique_lock lock(self->state->mutex, std::try_to_lock);
        if (!lock.owns_lock())
            return MDR_RESULT_INPROGRESS;
        if (self->state->result != MDR_RESULT_OK)
            return self->state->result;
        auto& rx = self->state->rx;
        if (rx.empty())
            return MDR_RESULT_INPROGRESS;
        const size_t count = std::min(static_cast<size_t>(size), rx.size());
        std::memcpy(dst, rx.data(), count);
        rx.erase(rx.begin(), rx.begin() + count);
        *pReceived = static_cast<int>(count);
        return MDR_RESULT_OK;
    }

    static MDRResult Send(void* user, const char* src, int size, int* pSent)
    {
        auto* self = static_cast<MDRConnectionIOS*>(user);
        *pSent = 0;
        const auto state = self->state;
        MDRAccessoryOwner* owner = self->owner;
        std::scoped_lock lock(state->mutex);
        if (state->result != MDR_RESULT_OK)
            return state->result;
        if (!state->tx.empty())
            return MDR_RESULT_INPROGRESS;
        const size_t count = std::min(static_cast<size_t>(size), kMaxWriteSize);
        const auto* bytes = reinterpret_cast<const uint8_t*>(src);
        state->tx.assign(bytes, bytes + count);
        const uint64_t generation = state->generation;
        dispatch_async(dispatch_get_main_queue(), ^{
            {
                std::scoped_lock stateLock(state->mutex);
                if (state->generation != generation)
                    return;
            }
            [owner.session flush];
        });
        *pSent = static_cast<int>(count);
        return MDR_RESULT_OK;
    }

    static MDRResult Poll(void* user, int timeout)
    {
        auto* self = static_cast<MDRConnectionIOS*>(user);
        const auto state = self->state;
        std::unique_lock lock(state->mutex);
        const auto ready = [&]
        {
            return state->result != MDR_RESULT_INPROGRESS &&
                (state->result != MDR_RESULT_OK || state->tx.empty() || !state->rx.empty());
        };
        if (ready())
            return state->result;
        if (timeout == 0)
            return state->result == MDR_RESULT_OK ? MDR_RESULT_ERROR_TIMEOUT : state->result;
        if ([NSThread isMainThread])
        {
            @autoreleasepool
            {
                const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(std::max(timeout, 0));
                while (!ready())
                {
                    const auto now = std::chrono::steady_clock::now();
                    if (timeout > 0 && now >= deadline)
                        return MDR_RESULT_ERROR_TIMEOUT;
                    const auto interval = timeout < 0
                        ? std::chrono::steady_clock::duration(std::chrono::milliseconds(10))
                        : std::min(deadline - now, std::chrono::steady_clock::duration(std::chrono::milliseconds(10)));
                    NSDate* limit = [NSDate dateWithTimeIntervalSinceNow:std::chrono::duration<double>(interval).count()];
                    lock.unlock();
                    [[NSRunLoop mainRunLoop] runMode:NSDefaultRunLoopMode beforeDate:limit];
                    lock.lock();
                }
                return state->result;
            }
        }
        if (timeout < 0)
            state->condition.wait(lock, ready);
        else if (!state->condition.wait_for(lock, std::chrono::milliseconds(timeout), ready))
            return MDR_RESULT_ERROR_TIMEOUT;
        return state->result;
    }

    static MDRResult GetDevicesList(void* user, MDRDeviceInfo** ppList, int* pCount)
    {
        auto* self = static_cast<MDRConnectionIOS*>(user);
        const auto state = self->state;
        std::scoped_lock lock(state->mutex);
        if (state->enumReady)
        {
            *ppList = state->devices;
            *pCount = state->deviceCount;
            state->devices = nullptr;
            state->deviceCount = 0;
            state->enumReady = false;
            return MDR_RESULT_OK;
        }
        if (!state->enumPending)
        {
            state->enumPending = true;
            dispatch_async(dispatch_get_main_queue(), ^{
                MDRDeviceInfo* devices{};
                int count{};
                EnumerateDevices(&devices, &count);
                std::scoped_lock stateLock(state->mutex);
                mdr::MDRAllocator<MDRDeviceInfo>().deallocate(state->devices);
                state->devices = devices;
                state->deviceCount = count;
                state->enumPending = false;
                state->enumReady = true;
            });
        }
        return MDR_RESULT_INPROGRESS;
    }

    static void EnumerateDevices(MDRDeviceInfo** ppList, int* pCount)
    {
        @autoreleasepool
        {
            *ppList = nullptr;
            *pCount = 0;
            NSMutableArray<EAAccessory*>* accessories = [NSMutableArray array];
            for (EAAccessory* accessory in [EAAccessoryManager sharedAccessoryManager].connectedAccessories)
                if (SupportsMdr(accessory))
                    [accessories addObject:accessory];
            if (accessories.count == 0)
                return;
            *ppList = mdr::MDRAllocator<MDRDeviceInfo>().allocate(accessories.count);
            *pCount = static_cast<int>(accessories.count);
            for (NSUInteger i = 0; i < accessories.count; ++i)
            {
                MDRDeviceInfo* info = &(*ppList)[i];
                std::memset(info, 0, sizeof(MDRDeviceInfo));
                CopyUtf8(info->szDeviceName, accessories[i].name.UTF8String);
                FormatAccessoryAddress(accessories[i], info->szDeviceMacAddress);
            }
        }
    }

    static MDRResult FreeDevicesList(void* user, MDRDeviceInfo** ppList)
    {
        if (*ppList)
        {
            mdr::MDRAllocator<MDRDeviceInfo>().deallocate(*ppList);
            *ppList = nullptr;
        }
        return MDR_RESULT_OK;
    }

    static const char* GetLastError(void* user)
    {
        auto* self = static_cast<MDRConnectionIOS*>(user);
        std::scoped_lock lock(self->state->mutex);
        self->errorSnapshot = self->state->error;
        return self->errorSnapshot.c_str();
    }
};

MDR_API MDRConnectionIOS* mdrConnectionIOSCreate() { return mdr::Construct<MDRConnectionIOS>(); }
MDR_API MDRConnection* mdrConnectionIOSGet(MDRConnectionIOS* self) { return &self->mdrConn; }
MDR_API void mdrConnectionIOSDestroy(MDRConnectionIOS* self) { mdr::Destruct(self); }
