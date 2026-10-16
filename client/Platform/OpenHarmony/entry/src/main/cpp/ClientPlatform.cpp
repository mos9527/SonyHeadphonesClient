#include "ClientConnection.hpp"
#include "Platform/Platform.hpp"
#include <SDL3/SDL.h>
#include <napi/native_api.h>
#include <hilog/log.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstring>
#include <iterator>
#include <limits>
#include <mutex>
#include <string_view>

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <imstb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION
#undef STBTT_STATIC

namespace
{
    constexpr Uint64 kEventWaitRecheckNS = SDL_MS_TO_NS(50);
    struct EventWaitState
    {
        SDL_Semaphore* semaphore{};
        std::atomic<bool> waiting{};
    } gEventWait;

    bool SDLCALL WakeEventWait(void* userdata, SDL_Event*)
    {
        auto* state = static_cast<EventWaitState*>(userdata);
        if (state->waiting.load(std::memory_order_relaxed))
            SDL_SignalSemaphore(state->semaphore);
        return true;
    }

    ohos::ClientConnection gTransport;
    bool gConnectionInitialized{};
    std::atomic<float> gDisplayScale{1.0f};
    struct ShareRequest
    {
        uint32_t id{};
        bool taken{};
        mdr::String filename;
        mdr::String mimeType;
        mdr::Vector<unsigned char> data;
        ClientPlatformSaveBlobCallback callback{};
        void* userdata{};
    };
    std::mutex gShareMutex;
    ShareRequest gShare;
    uint32_t gNextShareId{};
    bool gShareAvailable{};

    bool TakeShare(uint32_t id, ShareRequest& request)
    {
        std::scoped_lock lock(gShareMutex);
        if (!gShare.id || (id && gShare.id != id))
            return false;
        request = std::move(gShare);
        gShare = {};
        return true;
    }

    void FinishShare(const ShareRequest& request, ClientPlatformSaveBlobResult result, const char* error = nullptr)
    {
        request.callback(request.userdata, result, request.filename.c_str(), error);
        SDL_Event wake{};
        wake.type = SDL_EVENT_USER;
        SDL_PushEvent(&wake);
    }

    struct FontMapping
    {
        void* data{};
        size_t size{};
        int face{};
        bool attempted{};
    };
    std::array<FontMapping, static_cast<size_t>(AppLocale::NUM_LOCALES)> gFonts;

    mdr::String NormalizeFontName(const mdr::String& name)
    {
        mdr::String result;
        for (unsigned char value : name)
            if (std::isalnum(value))
                result += static_cast<char>(std::tolower(value));
        return result;
    }

    int FontPreference(const mdr::String& name, AppLocale locale)
    {
        const mdr::String normalized = NormalizeFontName(name);
        int score = normalized.find("sans") != mdr::String::npos ? 10 : 0;
        if (normalized.find("cjk") != mdr::String::npos)
            score += 20;
        if (normalized.find("regular") != mdr::String::npos)
            score += 5;
        if (normalized.find("bold") != mdr::String::npos || normalized.find("italic") != mdr::String::npos)
            score -= 10;
        const char* suffix = "sc";
        switch (locale)
        {
        case AppLocale::TRADITIONAL_CHINESE: suffix = "tc"; break;
        case AppLocale::JAPANESE: suffix = "jp"; break;
        case AppLocale::KOREAN: suffix = "kr"; break;
        default: break;
        }
        if (normalized.find(mdr::String("sans") + suffix) != mdr::String::npos ||
            normalized.find(mdr::String("cjk") + suffix) != mdr::String::npos)
            score += 50;
        return score;
    }

    bool FontSupportsLocale(const stbtt_fontinfo& font, AppLocale locale)
    {
        if (!stbtt_FindGlyphIndex(&font, 'A') || !stbtt_FindGlyphIndex(&font, '0'))
            return false;
        std::u32string_view sample = U"中文电连接控制";
        switch (locale)
        {
        case AppLocale::TRADITIONAL_CHINESE: sample = U"中文電連接聲音"; break;
        case AppLocale::JAPANESE: sample = U"日本語あいうアイウ"; break;
        case AppLocale::KOREAN: sample = U"한국어연결음량"; break;
        default: break;
        }
        return std::all_of(sample.begin(), sample.end(), [&font](char32_t codepoint) {
            return stbtt_FindGlyphIndex(&font, static_cast<int>(codepoint)) != 0;
        });
    }

    mdr::String FontFaceName(const stbtt_fontinfo& font)
    {
        int length{};
        const char* name = stbtt_GetFontNameString(&font, &length, 3, 1, 0x0409, 6);
        mdr::String result;
        if (name)
            for (int index = 0; index + 1 < length; index += 2)
                if (name[index] == 0 && static_cast<unsigned char>(name[index + 1]) < 128)
                    result += name[index + 1];
        return result;
    }

    napi_value Throw(napi_env env, const char* message)
    {
        napi_throw_error(env, nullptr, message);
        return nullptr;
    }

    napi_value Undefined(napi_env env)
    {
        napi_value value{};
        napi_get_undefined(env, &value);
        return value;
    }

    bool Arguments(napi_env env, napi_callback_info info, size_t count, napi_value* values)
    {
        size_t actual = count;
        return napi_get_cb_info(env, info, &actual, values, nullptr, nullptr) == napi_ok && actual == count;
    }

    bool String(napi_env env, napi_value value, mdr::String& output)
    {
        size_t length{};
        if (napi_get_value_string_utf8(env, value, nullptr, 0, &length) != napi_ok || length > 4096)
            return false;
        mdr::Vector<char> buffer(length + 1);
        if (napi_get_value_string_utf8(env, value, buffer.data(), buffer.size(), &length) != napi_ok)
            return false;
        output.assign(buffer.data(), length);
        return output.find('\0') == mdr::String::npos;
    }

    bool Generation(napi_env env, napi_value value, uint32_t& output)
    {
        double number{};
        if (napi_get_value_double(env, value, &number) != napi_ok || !std::isfinite(number) || number < 0 ||
            number > std::numeric_limits<uint32_t>::max())
            return false;
        output = static_cast<uint32_t>(number);
        return static_cast<double>(output) == number;
    }

    bool Property(napi_env env, napi_value object, const char* name, napi_value value)
    {
        return napi_set_named_property(env, object, name, value) == napi_ok;
    }

    bool Number(napi_env env, napi_value object, const char* name, uint32_t value)
    {
        napi_value number{};
        return napi_create_uint32(env, value, &number) == napi_ok && Property(env, object, name, number);
    }

    bool Text(napi_env env, napi_value object, const char* name, const mdr::String& value)
    {
        napi_value text{};
        return napi_create_string_utf8(env, value.c_str(), value.size(), &text) == napi_ok &&
            Property(env, object, name, text);
    }

    napi_value SetShareAvailable(napi_env env, napi_callback_info info)
    {
        napi_value argument{};
        bool available{};
        if (!Arguments(env, info, 1, &argument) || napi_get_value_bool(env, argument, &available) != napi_ok)
            return Throw(env, "setShareAvailable expects a boolean");
        ShareRequest request;
        {
            std::scoped_lock lock(gShareMutex);
            gShareAvailable = available;
            if (!available && gShare.id)
            {
                request = std::move(gShare);
                gShare = {};
            }
        }
        if (request.id)
            FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_CANCELLED);
        return Undefined(env);
    }

    napi_value PollShare(napi_env env, napi_callback_info)
    {
        std::unique_lock lock(gShareMutex);
        if (!gShareAvailable || !gShare.id || gShare.taken)
            return Undefined(env);
        napi_value result{};
        napi_value data{};
        void* bytes{};
        if (napi_create_object(env, &result) != napi_ok || !Number(env, result, "id", gShare.id) ||
            !Text(env, result, "filename", gShare.filename) || !Text(env, result, "mimeType", gShare.mimeType) ||
            napi_create_arraybuffer(env, gShare.data.size(), &bytes, &data) != napi_ok ||
            !Property(env, result, "data", data))
        {
            ShareRequest request = std::move(gShare);
            gShare = {};
            lock.unlock();
            FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, "Unable to prepare OHOS file sharing");
            return Throw(env, "Unable to create share request");
        }
        std::memcpy(bytes, gShare.data.data(), gShare.data.size());
        gShare.data = mdr::Vector<unsigned char>{};
        gShare.taken = true;
        return result;
    }

    napi_value CompleteShare(napi_env env, napi_callback_info info)
    {
        napi_value arguments[3]{};
        uint32_t id{};
        uint32_t result{};
        mdr::String error;
        if (!Arguments(env, info, 3, arguments) || !Generation(env, arguments[0], id) || !id ||
            !Generation(env, arguments[1], result) ||
            (result != CLIENT_PLATFORM_SAVE_BLOB_DISPATCHED && result != CLIENT_PLATFORM_SAVE_BLOB_CANCELLED &&
             result != CLIENT_PLATFORM_SAVE_BLOB_FAILED) || !String(env, arguments[2], error))
            return Throw(env, "completeShare expects a request ID, share result and error string");
        ShareRequest request;
        if (TakeShare(id, request))
        {
            if (result == CLIENT_PLATFORM_SAVE_BLOB_FAILED && error.empty())
                error = "File sharing failed";
            FinishShare(request, static_cast<ClientPlatformSaveBlobResult>(result), error.empty() ? nullptr : error.c_str());
        }
        return Undefined(env);
    }

    napi_value PollTransport(napi_env env, napi_callback_info info)
    {
        napi_value argument{};
        bool takeTx{};
        if (!Arguments(env, info, 1, &argument) || napi_get_value_bool(env, argument, &takeTx) != napi_ok)
            return Throw(env, "pollTransport expects a boolean");
        const auto snapshot = gTransport.Take(takeTx);
        napi_value result{};
        napi_value refresh{};
        napi_value data{};
        void* bytes{};
        if (napi_create_object(env, &result) != napi_ok ||
            !Number(env, result, "generation", snapshot.generation) ||
            !Number(env, result, "action", snapshot.action) ||
            !Text(env, result, "address", snapshot.address) || !Text(env, result, "uuid", snapshot.uuid) ||
            napi_get_boolean(env, snapshot.refreshDevices, &refresh) != napi_ok ||
            !Property(env, result, "refreshDevices", refresh) ||
            napi_create_arraybuffer(env, snapshot.data.size(), &bytes, &data) != napi_ok ||
            !Property(env, result, "data", data))
            return Throw(env, "Unable to create transport snapshot");
        if (!snapshot.data.empty())
            std::memcpy(bytes, snapshot.data.data(), snapshot.data.size());
        return result;
    }

    napi_value TransportConnected(napi_env env, napi_callback_info info)
    {
        napi_value argument{};
        uint32_t generation{};
        if (!Arguments(env, info, 1, &argument) || !Generation(env, argument, generation))
            return Throw(env, "transportConnected expects a generation");
        gTransport.Connected(generation);
        return Undefined(env);
    }

    napi_value TransportFailed(napi_env env, napi_callback_info info)
    {
        napi_value arguments[2]{};
        uint32_t generation{};
        mdr::String error;
        if (!Arguments(env, info, 2, arguments) || !Generation(env, arguments[0], generation) ||
            !String(env, arguments[1], error))
            return Throw(env, "transportFailed expects a generation and an error string");
        gTransport.Failed(generation, error);
        return Undefined(env);
    }

    napi_value FeedRx(napi_env env, napi_callback_info info)
    {
        napi_value arguments[2]{};
        uint32_t generation{};
        void* bytes{};
        size_t length{};
        if (!Arguments(env, info, 2, arguments) || !Generation(env, arguments[0], generation) ||
            napi_get_arraybuffer_info(env, arguments[1], &bytes, &length) != napi_ok)
            return Throw(env, "feedRx expects a generation and an ArrayBuffer");
        if (!gTransport.Feed(generation, static_cast<const unsigned char*>(bytes), length))
            return Throw(env, "OHOS receive queue overflow");
        return Undefined(env);
    }

    void CopyText(char* destination, size_t capacity, const mdr::String& value)
    {
        size_t count = std::min(value.size(), capacity - 1);
        if (count < value.size())
            while (count && (static_cast<unsigned char>(value[count]) & 0xc0) == 0x80)
                --count;
        std::memcpy(destination, value.data(), count);
        destination[count] = '\0';
    }

    napi_value SetDevices(napi_env env, napi_callback_info info)
    {
        napi_value argument{};
        bool isArray{};
        uint32_t count{};
        if (!Arguments(env, info, 1, &argument) || napi_is_array(env, argument, &isArray) != napi_ok ||
            !isArray || napi_get_array_length(env, argument, &count) != napi_ok || count > 512)
            return Throw(env, "setDevices expects at most 512 paired devices");
        mdr::Vector<MDRDeviceInfo> devices;
        devices.reserve(count);
        for (uint32_t index = 0; index < count; ++index)
        {
            napi_value device{};
            napi_value nameValue{};
            napi_value addressValue{};
            mdr::String name;
            mdr::String address;
            if (napi_get_element(env, argument, index, &device) != napi_ok ||
                napi_get_named_property(env, device, "name", &nameValue) != napi_ok ||
                napi_get_named_property(env, device, "address", &addressValue) != napi_ok ||
                !String(env, nameValue, name) || !String(env, addressValue, address) || address.size() != 17)
                return Throw(env, "Invalid paired device name or Bluetooth address");
            MDRDeviceInfo value{};
            CopyText(value.szDeviceName, sizeof(value.szDeviceName), name.empty() ? address : name);
            CopyText(value.szDeviceMacAddress, sizeof(value.szDeviceMacAddress), address);
            devices.push_back(value);
        }
        gTransport.UpdateDevices(std::move(devices));
        return Undefined(env);
    }

    napi_value StopTransport(napi_env env, napi_callback_info)
    {
        gTransport.Abort();
        return Undefined(env);
    }

    napi_value SetScale(napi_env env, napi_callback_info info)
    {
        napi_value argument{};
        double scale{};
        if (!Arguments(env, info, 1, &argument) ||
            napi_get_value_double(env, argument, &scale) != napi_ok || !std::isfinite(scale) || scale < 0.5 || scale > 8.0)
            return Throw(env, "setScale expects a finite scale between 0.5 and 8");
        gDisplayScale.store(static_cast<float>(scale), std::memory_order_relaxed);
        return Undefined(env);
    }

    napi_value Init(napi_env env, napi_value exports)
    {
        napi_property_descriptor methods[]{
            {"setScale", nullptr, SetScale, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"setShareAvailable", nullptr, SetShareAvailable, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"pollShare", nullptr, PollShare, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"completeShare", nullptr, CompleteShare, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"pollTransport", nullptr, PollTransport, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"transportConnected", nullptr, TransportConnected, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"transportFailed", nullptr, TransportFailed, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"feedRx", nullptr, FeedRx, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"setDevices", nullptr, SetDevices, nullptr, nullptr, nullptr, napi_default, nullptr},
            {"stopTransport", nullptr, StopTransport, nullptr, nullptr, nullptr, napi_default, nullptr}
        };
        if (napi_define_properties(env, exports, std::size(methods), methods) != napi_ok)
            return Throw(env, "Unable to register OHOS client transport");
        return exports;
    }

    napi_module gModule{1, 0, nullptr, Init, "main", nullptr, {0}};
}

extern "C" __attribute__((constructor)) void RegisterSHCInterfaces()
{
    napi_module_register(&gModule);
}

bool clientPlatformInit()
{
    if (gEventWait.semaphore)
        return true;
    if (!SDL_Init(SDL_INIT_EVENTS))
        return false;
    gEventWait.semaphore = SDL_CreateSemaphore(0);
    if (!gEventWait.semaphore)
        return false;
    if (!SDL_AddEventWatch(WakeEventWait, &gEventWait))
    {
        SDL_DestroySemaphore(gEventWait.semaphore);
        gEventWait.semaphore = nullptr;
        return false;
    }
    return true;
}

bool clientPlatformWaitEventTimeout(int timeoutMS)
{
    const Uint64 deadline = timeoutMS > 0 ? SDL_GetTicksNS() + SDL_MS_TO_NS(timeoutMS) : 0;
    bool hasEvent = false;
    gEventWait.waiting.store(true, std::memory_order_relaxed);
    while (true)
    {
        SDL_PumpEvents();
        const int queued = SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST);
        if (queued != 0)
        {
            hasEvent = queued > 0;
            break;
        }
        const Uint64 now = SDL_GetTicksNS();
        if (timeoutMS == 0 || (timeoutMS > 0 && now >= deadline))
            break;
        const Uint64 waitNS = timeoutMS < 0 ? kEventWaitRecheckNS : std::min(kEventWaitRecheckNS, deadline - now);
        const Sint32 waitMS = static_cast<Sint32>(SDL_NS_TO_MS(waitNS + SDL_NS_PER_MS - 1));
        SDL_WaitSemaphoreTimeout(gEventWait.semaphore, waitMS);
    }
    gEventWait.waiting.store(false, std::memory_order_relaxed);
    return hasEvent;
}

float clientPlatformGetDisplayScale()
{
    return gDisplayScale.load(std::memory_order_relaxed);
}

bool clientPlatformInitGraphics(SDL_Renderer* renderer)
{
    if (renderer)
        return true;

    return SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0) && SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
}

int clientPlatformConnectionInit(int flags)
{
    if (flags & MDR_INIT_BT_BLE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    gConnectionInitialized = true;
    return MDR_RESULT_OK;
}

MDRConnection* clientPlatformConnectionGet()
{
    return gConnectionInitialized ? &gTransport.connection : nullptr;
}

void clientPlatformConnectionDestroy()
{
    gTransport.Abort();
    gConnectionInitialized = false;
}

int clientPlatformMemoryMapFile(const char* path, void** outAddr, size_t* outSize)
{
    if (outAddr) *outAddr = nullptr;
    if (outSize) *outSize = 0;
    if (!path || !outAddr || !outSize)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const int file = open(path, O_RDONLY | O_CLOEXEC);
    if (file < 0)
        return MDR_RESULT_ERROR_NOT_FOUND;
    struct stat info{};
    if (fstat(file, &info) != 0 || !S_ISREG(info.st_mode) || info.st_size <= 0 ||
        static_cast<uint64_t>(info.st_size) > std::numeric_limits<size_t>::max())
    {
        close(file);
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    const size_t size = static_cast<size_t>(info.st_size);
    void* data = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, file, 0);
    close(file);
    if (data == MAP_FAILED)
        return MDR_RESULT_ERROR_GENERAL;
    *outAddr = data;
    *outSize = size;
    return MDR_RESULT_OK;
}

void clientPlatformMemoryUnmapFile(void* addr, size_t size)
{
    if (addr && size)
        munmap(addr, size);
}

int clientPlatformLocateFontBinary(AppLocale locale, const char** outData, int* outFaceIndex)
{
    if (!outData || !outFaceIndex)
        return 0;
    *outData = nullptr;
    *outFaceIndex = 0;
    const size_t index = static_cast<size_t>(locale);
    if (index >= gFonts.size())
        return 0;
    FontMapping& mapping = gFonts[index];
    if (!mapping.attempted)
    {
        mapping.attempted = true;
        DIR* directory = opendir("/system/fonts");
        if (!directory)
        {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0, "SHCFont", "Unable to open /system/fonts");
            return 0;
        }
        mdr::Vector<mdr::String> candidates;
        while (const auto* entry = readdir(directory))
        {
            mdr::String name = entry->d_name;
            mdr::String lower = name;
            std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char value) { return std::tolower(value); });
            if (lower.ends_with(".ttf") || lower.ends_with(".otf") || lower.ends_with(".ttc"))
                candidates.push_back("/system/fonts/" + name);
        }
        closedir(directory);
        std::sort(candidates.begin(), candidates.end(), [locale](const mdr::String& left, const mdr::String& right) {
            const int leftScore = FontPreference(left, locale);
            const int rightScore = FontPreference(right, locale);
            return leftScore != rightScore ? leftScore > rightScore : left < right;
        });
        for (const auto& path : candidates)
        {
            void* data{};
            size_t size{};
            if (clientPlatformMemoryMapFile(path.c_str(), &data, &size) != MDR_RESULT_OK)
                continue;
            int bestFace = -1;
            int bestScore = INT_MIN;
            mdr::String bestName;
            if (size >= 12 && size <= static_cast<size_t>(INT_MAX))
            {
                const auto* bytes = static_cast<const unsigned char*>(data);
                const int faces = stbtt_GetNumberOfFonts(bytes);
                if (faces > 0 && faces <= 64 &&
                    (std::memcmp(bytes, "ttcf", 4) != 0 || size >= 12 + static_cast<size_t>(faces) * 4))
                {
                    for (int face = 0; face < faces; ++face)
                    {
                        const int offset = stbtt_GetFontOffsetForIndex(bytes, face);
                        stbtt_fontinfo font{};
                        if (offset < 0 || static_cast<size_t>(offset) > size - 12 ||
                            !stbtt_InitFont(&font, bytes, offset) || !FontSupportsLocale(font, locale))
                            continue;
                        const mdr::String name = FontFaceName(font);
                        const int score = FontPreference(name, locale);
                        if (score > bestScore)
                        {
                            bestFace = face;
                            bestScore = score;
                            bestName = name;
                        }
                    }
                }
            }
            if (bestFace >= 0)
            {
                mapping.data = data;
                mapping.size = size;
                mapping.face = bestFace;
                OH_LOG_Print(LOG_APP, LOG_INFO, 0, "SHCFont",
                    "Selected %{public}s, face %{public}d (%{public}s), locale %{public}d",
                    path.c_str(), bestFace, bestName.c_str(), static_cast<int>(locale));
                break;
            }
            clientPlatformMemoryUnmapFile(data, size);
        }
        if (!mapping.data)
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0, "SHCFont", "No system font covers locale %{public}d", static_cast<int>(locale));
    }
    *outData = static_cast<const char*>(mapping.data);
    *outFaceIndex = mapping.face;
    return static_cast<int>(mapping.size);
}

int clientPlatformIsLocalBluetoothAddress(const char*, int* outIsLocal)
{
    if (outIsLocal) *outIsLocal = 0;
    return MDR_RESULT_ERROR_NOT_SUPPORTED;
}

ClientMediaPause* clientPlatformMediaPause() { return nullptr; }
void clientPlatformMediaResume(ClientMediaPause*) {}

int clientPlatformSaveBlob(
    const char* filename, const unsigned char* data, size_t dataSize, const char* mimeType, SDL_Window* window,
    ClientPlatformSaveBlobCallback callback, void* userdata)
{
    if (!filename || !*filename || std::strlen(filename) > 255 || std::strpbrk(filename, "/\\") ||
        std::strcmp(filename, ".") == 0 || std::strcmp(filename, "..") == 0 || !data || !dataSize ||
        dataSize > static_cast<size_t>(INT_MAX) || !mimeType || !*mimeType || std::strlen(mimeType) > 255 || !window || !callback)
    {
        SDL_SetError("Invalid file sharing arguments");
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    std::scoped_lock lock(gShareMutex);
    if (!gShareAvailable)
    {
        SDL_SetError("OHOS system sharing is unavailable");
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    }
    if (gShare.id)
    {
        SDL_SetError("A file sharing request is already active");
        return MDR_RESULT_ERROR_GENERAL;
    }
    MDR_CHECK(gNextShareId < std::numeric_limits<uint32_t>::max());
    gShare.id = ++gNextShareId;
    gShare.filename = filename;
    gShare.mimeType = mimeType;
    gShare.data.assign(data, data + dataSize);
    gShare.callback = callback;
    gShare.userdata = userdata;
    return MDR_RESULT_OK;
}

void clientPlatformDestroy()
{
    if (gEventWait.semaphore)
    {
        SDL_RemoveEventWatch(WakeEventWait, &gEventWait);
        SDL_DestroySemaphore(gEventWait.semaphore);
        gEventWait.semaphore = nullptr;
    }
    ShareRequest request;
    {
        std::scoped_lock lock(gShareMutex);
        gShareAvailable = false;
        request = std::move(gShare);
        gShare = {};
    }
    if (request.id)
        FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_CANCELLED);
    clientPlatformConnectionDestroy();
    for (auto& font : gFonts)
    {
        clientPlatformMemoryUnmapFile(font.data, font.size);
        font = {};
    }
}
