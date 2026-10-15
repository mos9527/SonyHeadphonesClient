#import <Foundation/Foundation.h>
#import <IOBluetooth/IOBluetooth.h>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#import <dispatch/dispatch.h>

#include "../Utils.hpp"
#include <mdr-bt/ConnectionMacOS.h>

struct MDRMacState
{
    std::mutex mutex;
    std::condition_variable condition;
    uint64_t generation{};
    MDRResult result{MDR_RESULT_ERROR_NO_CONNECTION};
    mdr::String error;
    mdr::Vector<uint8_t> buffer;
    size_t mtu{1};
    bool writePending{};
    bool enumPending{};
    bool enumReady{};
    MDRDeviceInfo* devices{};
    int deviceCount{};

    ~MDRMacState() { mdr::MDRAllocator<MDRDeviceInfo>().deallocate(devices); }
};

@interface MDRBluetoothDelegate : NSObject <IOBluetoothRFCOMMChannelDelegate>
@property (nonatomic, strong) IOBluetoothRFCOMMChannel *rfcommChannel;
- (instancetype)initWithState:(std::shared_ptr<MDRMacState>)state generation:(uint64_t)generation;
- (void)connectToAddress:(NSString *)address serviceUUID:(NSString *)uuid;
- (void)disconnect;
- (void)sendData:(NSData *)data;
@end

@interface MDRBluetoothOwner : NSObject
@property (nonatomic, strong) MDRBluetoothDelegate *delegate;
@end

@implementation MDRBluetoothOwner
@end

@implementation MDRBluetoothDelegate
{
    std::shared_ptr<MDRMacState> _state;
    uint64_t _generation;
    NSData *_writeData;
}

- (instancetype)initWithState:(std::shared_ptr<MDRMacState>)state generation:(uint64_t)generation {
    self = [super init];
    if (self) {
        _state = std::move(state);
        _generation = generation;
    }
    return self;
}

- (void)fail:(NSString *)message {
    std::scoped_lock lock(_state->mutex);
    if (_state->generation != _generation)
        return;
    _state->result = MDR_RESULT_ERROR_NET;
    _state->error = [message UTF8String];
    _state->condition.notify_all();
}

- (void)connectToAddress:(NSString *)address serviceUUID:(NSString *)uuidString {
    IOBluetoothDevice *device = [IOBluetoothDevice deviceWithAddressString:address];
    if (!device) {
        [self fail:@"Device not found (not paired?)"];
        return;
    }
    uint8_t uuidBytes[16];
    if (serviceUUIDtoBytes([uuidString UTF8String], uuidBytes) != 0) {
        [self fail:@"Invalid UUID string"];
        return;
    }
    IOBluetoothSDPUUID *uuid = [IOBluetoothSDPUUID uuidWithBytes:uuidBytes length:16];
    IOBluetoothSDPServiceRecord *serviceRecord = [device getServiceRecordForUUID:uuid];
    if (!serviceRecord) {
        [self fail:@"Service not found on device (try checking Bluetooth settings/ensure paired)"];
        return;
    }
    BluetoothRFCOMMChannelID channelID;
    if ([serviceRecord getRFCOMMChannelID:&channelID] != kIOReturnSuccess) {
        [self fail:@"Could not get RFCOMM Channel ID from service record"];
        return;
    }
    IOBluetoothRFCOMMChannel *channel = nil;
    const IOReturn result = [device openRFCOMMChannelAsync:&channel withChannelID:channelID delegate:self];
    if (result != kIOReturnSuccess) {
        [self fail:[NSString stringWithFormat:@"Failed to open RFCOMM channel: 0x%x", result]];
        return;
    }
    self.rfcommChannel = channel;
}

- (void)disconnect {
    IOBluetoothRFCOMMChannel *channel = self.rfcommChannel;
    self.rfcommChannel = nil;
    [channel setDelegate:nil];
    [channel closeChannel];
    _writeData = nil;
}

- (void)sendData:(NSData *)data {
    _writeData = data;
    const IOReturn result = [self.rfcommChannel writeAsync:(void *)data.bytes
                                                   length:(UInt16)data.length refcon:nullptr];
    if (result != kIOReturnSuccess) {
        _writeData = nil;
        [self fail:[NSString stringWithFormat:@"RFCOMM write failed: 0x%x", result]];
    }
}

- (void)rfcommChannelWriteComplete:(IOBluetoothRFCOMMChannel *)channel refcon:(void *)refcon status:(IOReturn)status {
    if (channel != self.rfcommChannel)
        return;
    _writeData = nil;
    if (status != kIOReturnSuccess) {
        [self fail:[NSString stringWithFormat:@"RFCOMM write failed: 0x%x", status]];
        return;
    }
    std::scoped_lock lock(_state->mutex);
    if (_state->generation == _generation) {
        _state->writePending = false;
        _state->condition.notify_all();
    }
}

- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel *)channel status:(IOReturn)error {
    bool stale;
    {
        std::scoped_lock lock(_state->mutex);
        stale = _state->generation != _generation;
    }
    if (stale) {
        [channel setDelegate:nil];
        [channel closeChannel];
        return;
    }
    if (error != kIOReturnSuccess) {
        [self fail:[NSString stringWithFormat:@"Connection handshake failed: 0x%x", error]];
        return;
    }
    self.rfcommChannel = channel;
    const size_t mtu = [channel getMTU];
    std::scoped_lock lock(_state->mutex);
    if (_state->generation != _generation)
        return;
    _state->mtu = std::max(size_t{1}, mtu);
    _state->result = MDR_RESULT_OK;
    _state->condition.notify_all();
}

- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel *)channel data:(void *)dataPointer length:(size_t)dataLength {
    const auto* bytes = static_cast<const uint8_t*>(dataPointer);
    std::scoped_lock lock(_state->mutex);
    if (_state->generation != _generation || _state->result != MDR_RESULT_OK)
        return;
    _state->buffer.insert(_state->buffer.end(), bytes, bytes + dataLength);
    _state->condition.notify_all();
}

- (void)rfcommChannelClosed:(IOBluetoothRFCOMMChannel *)channel {
    if (channel != self.rfcommChannel)
        return;
    [self disconnect];
    [self fail:@"Connection closed by remote"];
}

@end

struct MDRConnectionMacOS
{
    MDRConnection mdrConn;
    MDRBluetoothOwner *owner;
    std::shared_ptr<MDRMacState> state{std::make_shared<MDRMacState>()};
    mdr::String errorSnapshot;

    MDRConnectionMacOS() {
        owner = [[MDRBluetoothOwner alloc] init];
        mdrConn = {
            .user = this,
            .connect = Connect,
            .disconnect = Disconnect,
            .recv = Recv,
            .send = Send,
            .poll = Poll,
            .getDevicesList = GetDevicesList,
            .freeDevicesList = FreeDevicesList,
            .getLastError = GetLastError
        };
    }
    
    ~MDRConnectionMacOS() {
        Disconnect(this);
        owner = nil;
    }

    static MDRResult Connect(void* user, const char* macAddress, const char* serviceUUID) {
        auto* self = static_cast<MDRConnectionMacOS*>(user);
        @autoreleasepool {
            NSString *address = [NSString stringWithUTF8String:macAddress];
            NSString *uuid = [NSString stringWithUTF8String:serviceUUID];
            const auto state = self->state;
            MDRBluetoothOwner *owner = self->owner;
            std::scoped_lock lock(state->mutex);
            const uint64_t generation = ++state->generation;
            state->result = MDR_RESULT_INPROGRESS;
            state->error.clear();
            state->buffer.clear();
            state->writePending = false;
            dispatch_async(dispatch_get_main_queue(), ^{
                {
                    std::scoped_lock stateLock(state->mutex);
                    if (state->generation != generation)
                        return;
                }
                [owner.delegate disconnect];
                owner.delegate = [[MDRBluetoothDelegate alloc] initWithState:state generation:generation];
                [owner.delegate connectToAddress:address serviceUUID:uuid];
            });
            state->condition.notify_all();
            return MDR_RESULT_INPROGRESS;
        }
    }

    static void Disconnect(void* user) {
        auto* self = static_cast<MDRConnectionMacOS*>(user);
        const auto state = self->state;
        MDRBluetoothOwner *owner = self->owner;
        std::scoped_lock lock(state->mutex);
        const uint64_t generation = ++state->generation;
        state->result = MDR_RESULT_ERROR_NO_CONNECTION;
        state->buffer.clear();
        state->writePending = false;
        state->condition.notify_all();
        dispatch_async(dispatch_get_main_queue(), ^{
            {
                std::scoped_lock stateLock(state->mutex);
                if (state->generation != generation)
                    return;
            }
            [owner.delegate disconnect];
            owner.delegate = nil;
        });
    }

    static MDRResult Recv(void* user, char* dst, int size, int* pReceived) {
        auto* self = static_cast<MDRConnectionMacOS*>(user);
        *pReceived = 0;
        std::unique_lock lock(self->state->mutex, std::try_to_lock);
        if (!lock.owns_lock())
            return MDR_RESULT_INPROGRESS;
        if (self->state->result != MDR_RESULT_OK)
            return self->state->result;
        auto& buffer = self->state->buffer;
        if (buffer.empty())
            return MDR_RESULT_INPROGRESS;
        const size_t count = std::min(static_cast<size_t>(size), buffer.size());
        std::memcpy(dst, buffer.data(), count);
        buffer.erase(buffer.begin(), buffer.begin() + count);
        *pReceived = static_cast<int>(count);
        return MDR_RESULT_OK;
    }

    static MDRResult Send(void* user, const char* src, int size, int* pSent) {
        auto* self = static_cast<MDRConnectionMacOS*>(user);
        *pSent = 0;
        @autoreleasepool {
            const auto state = self->state;
            MDRBluetoothOwner *owner = self->owner;
            std::scoped_lock lock(state->mutex);
            if (state->result != MDR_RESULT_OK)
                return state->result;
            if (state->writePending)
                return MDR_RESULT_INPROGRESS;
            const uint64_t generation = state->generation;
            const size_t count = std::min({static_cast<size_t>(size), state->mtu, size_t{UINT16_MAX}});
            NSData *data = [NSData dataWithBytes:src length:count];
            state->writePending = true;
            dispatch_async(dispatch_get_main_queue(), ^{
                {
                    std::scoped_lock stateLock(state->mutex);
                    if (state->generation != generation || state->result != MDR_RESULT_OK)
                        return;
                }
                [owner.delegate sendData:data];
            });
            *pSent = static_cast<int>(count);
            return MDR_RESULT_OK;
        }
    }

    static MDRResult Poll(void* user, int timeout) {
        auto* self = static_cast<MDRConnectionMacOS*>(user);
        const auto state = self->state;
        std::unique_lock lock(state->mutex);
        const auto ready = [&] {
            return state->result != MDR_RESULT_INPROGRESS &&
                (state->result != MDR_RESULT_OK || !state->writePending || !state->buffer.empty());
        };
        if (ready())
            return state->result;
        if (timeout == 0)
            return state->result == MDR_RESULT_OK ? MDR_RESULT_ERROR_TIMEOUT : state->result;
        if ([NSThread isMainThread]) {
            @autoreleasepool {
                const auto deadline = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(timeout > 0 ? timeout : 0);
                while (!ready()) {
                    const auto now = std::chrono::steady_clock::now();
                    if (timeout > 0 && now >= deadline)
                        return MDR_RESULT_ERROR_TIMEOUT;
                    const auto interval = timeout < 0 ? std::chrono::steady_clock::duration(std::chrono::milliseconds(10))
                        : std::min(deadline - now, std::chrono::steady_clock::duration(std::chrono::milliseconds(10)));
                    NSDate *limit = [NSDate dateWithTimeIntervalSinceNow:std::chrono::duration<double>(interval).count()];
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

    static MDRResult GetDevicesList(void* user, MDRDeviceInfo** ppList, int* pCount) {
        auto* self = static_cast<MDRConnectionMacOS*>(user);
        const auto state = self->state;
        std::scoped_lock lock(state->mutex);
        if (state->enumReady) {
            *ppList = state->devices;
            *pCount = state->deviceCount;
            state->devices = nullptr;
            state->deviceCount = 0;
            state->enumReady = false;
            return MDR_RESULT_OK;
        }
        if (!state->enumPending) {
            state->enumPending = true;
            dispatch_async(dispatch_get_main_queue(), ^{
                MDRDeviceInfo* devices{};
                int count{};
                GetDevicesListImpl(nullptr, &devices, &count);
                std::scoped_lock stateLock(state->mutex);
                state->devices = devices;
                state->deviceCount = count;
                state->enumPending = false;
                state->enumReady = true;
            });
        }
        return MDR_RESULT_INPROGRESS;
    }

    static MDRResult GetDevicesListImpl(void* user, MDRDeviceInfo** ppList, int* pCount) {
        @autoreleasepool {
            NSArray *devices = [IOBluetoothDevice pairedDevices];
            if (!devices || devices.count == 0) {
                *pCount = 0;
                *ppList = nullptr;
                return MDR_RESULT_OK;
            }
            
            *ppList = mdr::MDRAllocator<MDRDeviceInfo>().allocate(devices.count);
            *pCount = (int)devices.count;
            
            for (NSUInteger i = 0; i < devices.count; i++) {
                IOBluetoothDevice *dev = devices[i];
                NSString *name = dev.name;
                NSString *addr = [dev.addressString stringByReplacingOccurrencesOfString:@"-" withString:@":"];
                
                MDRDeviceInfo *info = &(*ppList)[i];
                memset(info, 0, sizeof(MDRDeviceInfo));
                
                if (name) {
                    CopyUtf8(info->szDeviceName, [name UTF8String]);
                }
                if (addr) {
                    CopyUtf8(info->szDeviceMacAddress, [addr UTF8String]);
                }
            }
            return MDR_RESULT_OK;
        }
    }
    
    static MDRResult FreeDevicesList(void* user, MDRDeviceInfo** ppList) {
        if (*ppList)
        {
            mdr::MDRAllocator<MDRDeviceInfo>().deallocate(*ppList);
            *ppList = nullptr;
        }
        return MDR_RESULT_OK;
    }
    
    static const char* GetLastError(void* user) {
        auto* self = static_cast<MDRConnectionMacOS*>(user);
        std::scoped_lock lock(self->state->mutex);
        self->errorSnapshot = self->state->error;
        return self->errorSnapshot.c_str();
    }
};

MDR_API MDRConnectionMacOS* mdrConnectionMacOSCreate() { return mdr::Construct<MDRConnectionMacOS>(); }
MDR_API MDRConnection* mdrConnectionMacOSGet(MDRConnectionMacOS* self) { return &self->mdrConn; }
MDR_API void mdrConnectionMacOSDestroy(MDRConnectionMacOS* self) { mdr::Destruct(self); }
