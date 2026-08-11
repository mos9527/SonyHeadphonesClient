#pragma once

#include <mdr-c/Headphones.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace mdrtest
{
    // In-memory MDRConnection: replays a canned RX stream and records everything the
    // library writes back, so tests exercise the real receive path without a device.
    struct MockTransport
    {
        std::vector<uint8_t> rx;
        std::vector<uint8_t> tx;
        size_t offset{};
        // Bytes handed out per Receive call, mimicking a fragmenting transport. 0 means
        // "as much as the caller asked for".
        size_t chunk{};
        MDRConnection connection{
            this, Connect, Disconnect, Receive, Send, Poll, GetDevices, FreeDevices, GetLastError,
        };

        void Load(std::span<const uint8_t> bytes)
        {
            rx.assign(bytes.begin(), bytes.end());
            offset = 0;
        }

        static MDRResult Connect(void*, const char*, const char*) { return MDR_RESULT_OK; }

        static void Disconnect(void*) {}

        static MDRResult Receive(void* user, char* destination, int size, int* received)
        {
            auto& self = *static_cast<MockTransport*>(user);
            *received = 0;
            if (self.offset == self.rx.size())
                return MDR_RESULT_INPROGRESS;

            size_t count = std::min(static_cast<size_t>(size), self.rx.size() - self.offset);
            if (self.chunk != 0)
                count = std::min(count, self.chunk);
            std::copy_n(self.rx.begin() + static_cast<ptrdiff_t>(self.offset), count, destination);
            self.offset += count;
            *received = static_cast<int>(count);
            return MDR_RESULT_OK;
        }

        static MDRResult Send(void* user, const char* source, int size, int* sent)
        {
            auto& self = *static_cast<MockTransport*>(user);
            self.tx.insert(self.tx.end(), source, source + size);
            *sent = size;
            return MDR_RESULT_OK;
        }

        static MDRResult Poll(void*, int) { return MDR_RESULT_OK; }

        static MDRResult GetDevices(void*, MDRDeviceInfo** devices, int* count)
        {
            *devices = nullptr;
            *count = 0;
            return MDR_RESULT_OK;
        }

        static MDRResult FreeDevices(void*, MDRDeviceInfo** devices)
        {
            *devices = nullptr;
            return MDR_RESULT_OK;
        }

        static const char* GetLastError(void*) { return "mock transport"; }
    };

    inline std::string GetLastError(MDRHeadphones* headphones)
    {
        uint32_t size = 0;
        if (mdrHeadphonesGetText(headphones, MDR_TEXT_LAST_ERROR, 0, nullptr, &size) != MDR_RESULT_OK)
        {
            return {};
        }
        std::vector<char> buffer(size);
        if (mdrHeadphonesGetText(headphones, MDR_TEXT_LAST_ERROR, 0, buffer.data(), &size) != MDR_RESULT_OK)
        {
            return {};
        }
        return buffer.data();
    }
} // namespace mdrtest
