#include "ClientConnection.hpp"
#include "Platform/Platform.hpp"
#include <SDL3/SDL.h>
#include <mdr/Protocol.hpp>
#include <jni.h>
#include <android/log.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <mutex>
#include <string_view>

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#include <imstb_truetype.h>
#pragma clang diagnostic pop
#undef STB_TRUETYPE_IMPLEMENTATION
#undef STBTT_STATIC

namespace
{
    constexpr char kStderrLogTag[] = "MDR";

    int InitStderrLog()
    {
        static bool started{};
        if (started)
            return 0;
        int descriptors[2];
        if (pipe2(descriptors, O_CLOEXEC) != 0)
            return errno;
        FILE* input = fdopen(descriptors[0], "r");
        if (!input)
        {
            const int error = errno;
            close(descriptors[0]);
            close(descriptors[1]);
            return error;
        }
        pthread_t thread;
        const int error = pthread_create(&thread, nullptr, [](void* userdata) -> void* {
            FILE* input = static_cast<FILE*>(userdata);
            char message[3073];
            for (;;)
            {
                if (!std::fgets(message, sizeof(message), input))
                {
                    if (std::ferror(input) && errno == EINTR)
                    {
                        std::clearerr(input);
                        continue;
                    }
                    break;
                }
                size_t size = std::strlen(message);
                while (size && (message[size - 1] == '\n' || message[size - 1] == '\r'))
                    message[--size] = '\0';
                if (size)
                    __android_log_write(ANDROID_LOG_INFO, kStderrLogTag, message);
            }
            std::fclose(input);
            return nullptr;
        }, input);
        if (error != 0)
        {
            std::fclose(input);
            close(descriptors[1]);
            return error;
        }
        std::fflush(stderr);
        int result;
        do
            result = dup2(descriptors[1], STDERR_FILENO);
        while (result < 0 && errno == EINTR);
        const int redirectError = result < 0 ? errno : 0;
        close(descriptors[1]);
        if (redirectError)
        {
            pthread_join(thread, nullptr);
            return redirectError;
        }
        pthread_detach(thread);
        started = true;
        return 0;
    }

    constexpr char kClientClass[] = "com/mos9527/SonyHeadphonesClient/NativeClient";
    android::ClientConnection gTransport;
    bool gConnectionInitialized{};
    std::atomic<float> gDisplayScale{1.0f};
    jclass gSnapshotClass{};
    jmethodID gSnapshotConstructor{};
    struct ShareRequest
    {
        jlong id{};
        mdr::String filename;
        ClientPlatformSaveBlobCallback callback{};
        void* userdata{};
    };
    std::mutex gShareMutex;
    ShareRequest gShare;
    jlong gNextShareId{};

    bool TakeShare(jlong id, ShareRequest& request)
    {
        std::scoped_lock lock(gShareMutex);
        if (!gShare.id || (id && gShare.id != id))
            return false;
        request = std::move(gShare);
        gShare = {};
        return true;
    }

    jstring JavaString(JNIEnv* env, const char* text)
    {
        char* utf16 = SDL_iconv_string("UTF-16LE", "UTF-8", text, std::strlen(text) + 1);
        if (!utf16)
            return nullptr;
        const auto* characters = reinterpret_cast<const jchar*>(utf16);
        jsize count{};
        while (characters[count])
            ++count;
        jstring result = env->NewString(characters, count);
        SDL_free(utf16);
        return result;
    }

    struct FontMapping
    {
        void* data{};
        size_t size{};
        int face{};
        bool attempted{};
    };
    std::array<FontMapping, static_cast<size_t>(AppLocale::NUM_LOCALES)> gFonts;

    void Throw(JNIEnv* env, const char* message)
    {
        if (env->ExceptionCheck())
            return;
        jclass type = env->FindClass("java/lang/IllegalArgumentException");
        if (type)
            env->ThrowNew(type, message);
    }

    bool Generation(JNIEnv* env, jlong value, uint32_t& result)
    {
        if (value < 0 || value > std::numeric_limits<uint32_t>::max())
        {
            Throw(env, "Invalid transport generation");
            return false;
        }
        result = static_cast<uint32_t>(value);
        return true;
    }

    bool String(JNIEnv* env, jstring value, mdr::String& result)
    {
        if (!value || env->GetStringLength(value) > 4096)
        {
            Throw(env, "Invalid Bluetooth string");
            return false;
        }
        const jsize size = env->GetStringLength(value);
        const jchar* characters = env->GetStringChars(value, nullptr);
        if (!characters)
            return false;
        result.clear();
        for (jsize index = 0; index < size; ++index)
        {
            uint32_t codepoint = characters[index];
            if (codepoint >= 0xd800 && codepoint <= 0xdbff && index + 1 < size &&
                characters[index + 1] >= 0xdc00 && characters[index + 1] <= 0xdfff)
                codepoint = 0x10000 + ((codepoint - 0xd800) << 10) + (characters[++index] - 0xdc00);
            else if (codepoint >= 0xd800 && codepoint <= 0xdfff)
                codepoint = 0xfffd;
            if (codepoint <= 0x7f)
                result += static_cast<char>(codepoint);
            else if (codepoint <= 0x7ff)
            {
                result += static_cast<char>(0xc0 | (codepoint >> 6));
                result += static_cast<char>(0x80 | (codepoint & 0x3f));
            }
            else if (codepoint <= 0xffff)
            {
                result += static_cast<char>(0xe0 | (codepoint >> 12));
                result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f));
                result += static_cast<char>(0x80 | (codepoint & 0x3f));
            }
            else
            {
                result += static_cast<char>(0xf0 | (codepoint >> 18));
                result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f));
                result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f));
                result += static_cast<char>(0x80 | (codepoint & 0x3f));
            }
        }
        env->ReleaseStringChars(value, characters);
        if (result.find('\0') != mdr::String::npos)
        {
            Throw(env, "Bluetooth strings cannot contain null characters");
            return false;
        }
        return true;
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

    void SetScale(JNIEnv* env, jclass, jfloat scale)
    {
        if (!std::isfinite(scale) || scale < 0.5f || scale > 8.0f)
        {
            Throw(env, "Display scale must be between 0.5 and 8");
            return;
        }
        gDisplayScale.store(scale, std::memory_order_relaxed);
    }

    jobject PollTransport(JNIEnv* env, jclass, jboolean takeTx)
    {
        const auto snapshot = gTransport.Take(takeTx == JNI_TRUE);
        jstring address = env->NewStringUTF(snapshot.address.c_str());
        if (!address)
            return nullptr;
        jstring uuid = env->NewStringUTF(snapshot.uuid.c_str());
        if (!uuid)
            return nullptr;
        jbyteArray data = env->NewByteArray(static_cast<jsize>(snapshot.data.size()));
        if (!data)
            return nullptr;
        if (!snapshot.data.empty())
            env->SetByteArrayRegion(data, 0, static_cast<jsize>(snapshot.data.size()),
                                   reinterpret_cast<const jbyte*>(snapshot.data.data()));
        if (env->ExceptionCheck())
            return nullptr;
        return env->NewObject(gSnapshotClass, gSnapshotConstructor, static_cast<jlong>(snapshot.generation),
                              static_cast<jint>(snapshot.action), static_cast<jboolean>(snapshot.refreshDevices),
                              address, uuid, data);
    }

    void TransportConnected(JNIEnv* env, jclass, jlong value)
    {
        uint32_t generation{};
        if (Generation(env, value, generation))
            gTransport.Connected(generation);
    }

    void TransportFailed(JNIEnv* env, jclass, jlong value, jstring message)
    {
        uint32_t generation{};
        mdr::String error;
        if (Generation(env, value, generation) && String(env, message, error))
            gTransport.Failed(generation, error);
    }

    jboolean FeedRx(JNIEnv* env, jclass, jlong value, jbyteArray data, jint size)
    {
        uint32_t generation{};
        if (!Generation(env, value, generation))
            return JNI_FALSE;
        if (!data || size < 0 || size > 4096 || size > env->GetArrayLength(data))
        {
            Throw(env, "Invalid RFCOMM receive buffer");
            return JNI_FALSE;
        }
        std::array<unsigned char, 4096> buffer{};
        if (size)
            env->GetByteArrayRegion(data, 0, size, reinterpret_cast<jbyte*>(buffer.data()));
        if (env->ExceptionCheck())
            return JNI_FALSE;
        return gTransport.Feed(generation, buffer.data(), static_cast<size_t>(size)) ? JNI_TRUE : JNI_FALSE;
    }

    void SetDevices(JNIEnv* env, jclass, jobjectArray names, jobjectArray addresses)
    {
        if (!names || !addresses)
        {
            Throw(env, "Paired device arrays are required");
            return;
        }
        const jsize count = env->GetArrayLength(names);
        if (count > 512 || count != env->GetArrayLength(addresses))
        {
            Throw(env, "Invalid paired device array lengths");
            return;
        }
        mdr::Vector<MDRDeviceInfo> devices;
        devices.reserve(static_cast<size_t>(count));
        for (jsize index = 0; index < count; ++index)
        {
            jstring nameValue = static_cast<jstring>(env->GetObjectArrayElement(names, index));
            if (env->ExceptionCheck())
                return;
            jstring addressValue = static_cast<jstring>(env->GetObjectArrayElement(addresses, index));
            if (env->ExceptionCheck())
                return;
            mdr::String name;
            mdr::String address;
            const bool valid = String(env, nameValue, name) && String(env, addressValue, address);
            env->DeleteLocalRef(nameValue);
            env->DeleteLocalRef(addressValue);
            if (!valid)
                return;
            if (address.size() != 17)
            {
                Throw(env, "Invalid Bluetooth address");
                return;
            }
            MDRDeviceInfo device{};
            CopyText(device.szDeviceName, sizeof(device.szDeviceName), name.empty() ? address : name);
            CopyText(device.szDeviceMacAddress, sizeof(device.szDeviceMacAddress), address);
            devices.push_back(device);
        }
        gTransport.UpdateDevices(std::move(devices));
    }

    void StopTransport(JNIEnv*, jclass)
    {
        gTransport.Stop();
    }

    void ShareCompleted(JNIEnv* env, jclass, jlong id, jboolean dispatched, jstring errorValue)
    {
        if (id <= 0)
            return;
        ShareRequest request;
        if (!TakeShare(id, request))
            return;
        mdr::String error;
        if (errorValue && !String(env, errorValue, error))
        {
            env->ExceptionClear();
            error = "Unable to read file sharing error";
        }
        if (dispatched != JNI_TRUE && error.empty())
            error = "File sharing failed";
        request.callback(request.userdata,
                         dispatched == JNI_TRUE ? CLIENT_PLATFORM_SAVE_BLOB_DISPATCHED : CLIENT_PLATFORM_SAVE_BLOB_FAILED,
                         request.filename.c_str(), error.empty() ? nullptr : error.c_str());
    }

    bool FontSupportsLocale(const stbtt_fontinfo& font, AppLocale locale)
    {
        std::u32string_view sample = U"A0中文电连接控制";
        switch (locale)
        {
        case AppLocale::TRADITIONAL_CHINESE: sample = U"A0中文電連接聲音"; break;
        case AppLocale::JAPANESE: sample = U"A0日本語あいうアイウ"; break;
        case AppLocale::KOREAN: sample = U"A0한국어연결음량"; break;
        default: break;
        }
        return std::all_of(sample.begin(), sample.end(), [&font](char32_t codepoint) {
            return stbtt_FindGlyphIndex(&font, static_cast<int>(codepoint)) != 0;
        });
    }
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
    if (const int error = InitStderrLog(); error != 0)
        __android_log_print(ANDROID_LOG_ERROR, kStderrLogTag,
                            "Could not redirect stderr to Logcat: %s", std::strerror(error));
    JNIEnv* env{};
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;
    jclass client = env->FindClass(kClientClass);
    if (!client)
        return JNI_ERR;
    const mdr::String snapshotName = mdr::String(kClientClass) + "$TransportSnapshot";
    jclass snapshot = env->FindClass(snapshotName.c_str());
    if (!snapshot)
        return JNI_ERR;
    gSnapshotConstructor = env->GetMethodID(snapshot, "<init>", "(JIZLjava/lang/String;Ljava/lang/String;[B)V");
    if (!gSnapshotConstructor)
        return JNI_ERR;
    gSnapshotClass = static_cast<jclass>(env->NewGlobalRef(snapshot));
    if (!gSnapshotClass)
        return JNI_ERR;
    const mdr::String snapshotSignature = "(Z)L" + snapshotName + ";";
    JNINativeMethod methods[]{
        {const_cast<char*>("setScale"), const_cast<char*>("(F)V"), reinterpret_cast<void*>(SetScale)},
        {const_cast<char*>("pollTransport"), const_cast<char*>(snapshotSignature.c_str()), reinterpret_cast<void*>(PollTransport)},
        {const_cast<char*>("transportConnected"), const_cast<char*>("(J)V"), reinterpret_cast<void*>(TransportConnected)},
        {const_cast<char*>("transportFailed"), const_cast<char*>("(JLjava/lang/String;)V"), reinterpret_cast<void*>(TransportFailed)},
        {const_cast<char*>("feedRx"), const_cast<char*>("(J[BI)Z"), reinterpret_cast<void*>(FeedRx)},
        {const_cast<char*>("setDevices"), const_cast<char*>("([Ljava/lang/String;[Ljava/lang/String;)V"), reinterpret_cast<void*>(SetDevices)},
        {const_cast<char*>("stopTransport"), const_cast<char*>("()V"), reinterpret_cast<void*>(StopTransport)},
        {const_cast<char*>("shareCompleted"), const_cast<char*>("(JZLjava/lang/String;)V"), reinterpret_cast<void*>(ShareCompleted)}
    };
    const jint result = env->RegisterNatives(client, methods, static_cast<jint>(std::size(methods)));
    env->DeleteLocalRef(client);
    env->DeleteLocalRef(snapshot);
    if (result != JNI_OK)
    {
        env->DeleteGlobalRef(gSnapshotClass);
        gSnapshotClass = nullptr;
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void*)
{
    JNIEnv* env{};
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK && gSnapshotClass)
        env->DeleteGlobalRef(gSnapshotClass);
    gSnapshotClass = nullptr;
    gSnapshotConstructor = nullptr;
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

bool clientPlatformInit()
{
    if (const int error = InitStderrLog(); error != 0)
        return SDL_SetError("Could not redirect stderr to Logcat: %s", std::strerror(error));
    return true;
}

bool clientPlatformWaitEventTimeout(int timeoutMS)
{
    return SDL_WaitEventTimeout(nullptr, timeoutMS);
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
            return 0;
        mdr::Vector<mdr::String> candidates;
        while (const auto* entry = readdir(directory))
        {
            const mdr::String name = entry->d_name;
            if (name.ends_with(".ttf") || name.ends_with(".otf") || name.ends_with(".ttc"))
                candidates.push_back("/system/fonts/" + name);
        }
        closedir(directory);
        std::sort(candidates.begin(), candidates.end(), [](const auto& left, const auto& right) {
            const bool leftCJK = left.find("NotoSansCJK") != mdr::String::npos;
            const bool rightCJK = right.find("NotoSansCJK") != mdr::String::npos;
            return leftCJK != rightCJK ? leftCJK : left < right;
        });
        int preferredFace = 2;
        if (locale == AppLocale::TRADITIONAL_CHINESE) preferredFace = 3;
        if (locale == AppLocale::JAPANESE) preferredFace = 0;
        if (locale == AppLocale::KOREAN) preferredFace = 1;
        for (const auto& path : candidates)
        {
            void* data{};
            size_t size{};
            if (clientPlatformMemoryMapFile(path.c_str(), &data, &size) != MDR_RESULT_OK)
                continue;
            if (size >= 12 && size <= static_cast<size_t>(INT_MAX))
            {
                const auto* bytes = static_cast<const unsigned char*>(data);
                const int faces = stbtt_GetNumberOfFonts(bytes);
                if (faces > 0 && faces <= 64 &&
                    (std::memcmp(bytes, "ttcf", 4) != 0 || size >= 12 + static_cast<size_t>(faces) * 4))
                {
                    for (int attempt = 0; attempt < faces; ++attempt)
                    {
                        const int face = (preferredFace + attempt) % faces;
                        const int offset = stbtt_GetFontOffsetForIndex(bytes, face);
                        stbtt_fontinfo font{};
                        if (offset < 0 || static_cast<size_t>(offset) > size - 12 ||
                            !stbtt_InitFont(&font, bytes, offset) || !FontSupportsLocale(font, locale))
                            continue;
                        mapping = {data, size, face, true};
                        MDR_LOG("Selected Android font {}, face {}", path.c_str(), face);
                        break;
                    }
                }
            }
            if (mapping.data)
                break;
            clientPlatformMemoryUnmapFile(data, size);
        }
        if (!mapping.data)
            MDR_LOG("No Android system font covers locale {}", static_cast<int>(locale));
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
        dataSize > static_cast<size_t>(INT_MAX) || !mimeType || !*mimeType || !window || !callback)
    {
        SDL_SetError("Invalid file sharing arguments");
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    auto* env = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    if (!env)
    {
        SDL_SetError("Android JNI is unavailable");
        return MDR_RESULT_ERROR_GENERAL;
    }
    jobject activity = static_cast<jobject>(SDL_GetAndroidActivity());
    jclass type = activity ? env->GetObjectClass(activity) : nullptr;
    jmethodID method = type ? env->GetMethodID(type, "shareBlob", "(JLjava/lang/String;[BLjava/lang/String;)V") : nullptr;
    jstring name = method ? JavaString(env, filename) : nullptr;
    jstring mime = name ? JavaString(env, mimeType) : nullptr;
    jbyteArray bytes = mime ? env->NewByteArray(static_cast<jsize>(dataSize)) : nullptr;
    const auto release = [&]
    {
        if (bytes) env->DeleteLocalRef(bytes);
        if (mime) env->DeleteLocalRef(mime);
        if (name) env->DeleteLocalRef(name);
        if (type) env->DeleteLocalRef(type);
        if (activity) env->DeleteLocalRef(activity);
    };
    if (bytes)
        env->SetByteArrayRegion(bytes, 0, static_cast<jsize>(dataSize), reinterpret_cast<const jbyte*>(data));
    if (!bytes || env->ExceptionCheck())
    {
        env->ExceptionClear();
        release();
        SDL_SetError("Unable to prepare Android file sharing");
        return MDR_RESULT_ERROR_GENERAL;
    }
    jlong id{};
    {
        std::scoped_lock lock(gShareMutex);
        if (gShare.id)
        {
            release();
            SDL_SetError("A file sharing request is already active");
            return MDR_RESULT_ERROR_GENERAL;
        }
        MDR_CHECK(gNextShareId < std::numeric_limits<jlong>::max());
        id = ++gNextShareId;
        gShare = {id, filename, callback, userdata};
    }
    env->CallVoidMethod(activity, method, id, name, bytes, mime);
    const bool failed = env->ExceptionCheck();
    if (failed)
        env->ExceptionClear();
    release();
    ShareRequest rejected;
    if (failed && TakeShare(id, rejected))
    {
        SDL_SetError("Unable to schedule Android file sharing");
        return MDR_RESULT_ERROR_GENERAL;
    }
    return MDR_RESULT_OK;
}

void clientPlatformDestroy()
{
    ShareRequest request;
    if (TakeShare(0, request))
        request.callback(request.userdata, CLIENT_PLATFORM_SAVE_BLOB_CANCELLED, request.filename.c_str(), nullptr);
    clientPlatformConnectionDestroy();
    for (auto& font : gFonts)
    {
        clientPlatformMemoryUnmapFile(font.data, font.size);
        font = {};
    }
}
