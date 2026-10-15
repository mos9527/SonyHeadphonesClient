#pragma once

#include <mdr-c/Connection.h>
#include <mdr/Protocol.hpp>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <utility>

namespace ohos
{
    class ClientConnection
    {
    public:
        static constexpr size_t kCapacity = 65536;
        enum Action { NONE, CONNECT, DISCONNECT };
        struct Snapshot
        {
            uint32_t generation{};
            Action action{};
            bool refreshDevices{};
            mdr::String address;
            mdr::String uuid;
            mdr::Vector<unsigned char> data;
        };

        MDRConnection connection{this, Connect, Disconnect, Receive, Send, Poll, Devices, FreeDevices, LastError};

        Snapshot Take(bool takeTx)
        {
            std::scoped_lock lock(mMutex);
            Snapshot result{mGeneration, mAction, mRefreshRequested, mAddress, mUuid, {}};
            mAction = NONE;
            mRefreshRequested = false;
            if (takeTx && mResult == MDR_RESULT_OK)
            {
                const size_t count = std::min(mTx.size(), size_t{4096});
                result.data.assign(mTx.begin(), mTx.begin() + count);
                mTx.erase(mTx.begin(), mTx.begin() + count);
            }
            return result;
        }

        void Connected(uint32_t generation)
        {
            std::scoped_lock lock(mMutex);
            if (generation == mGeneration && mResult == MDR_RESULT_INPROGRESS)
                mResult = MDR_RESULT_OK;
        }

        void Failed(uint32_t generation, const mdr::String& error)
        {
            std::scoped_lock lock(mMutex);
            if (generation != mGeneration || (mResult != MDR_RESULT_OK && mResult != MDR_RESULT_INPROGRESS))
                return;
            Fail(error);
        }

        bool Feed(uint32_t generation, const unsigned char* data, size_t size)
        {
            std::scoped_lock lock(mMutex);
            if (generation != mGeneration || mResult != MDR_RESULT_OK)
                return true;
            if (size > kCapacity - mRx.size())
            {
                Fail("OHOS receive queue overflow");
                return false;
            }
            if (size)
                mRx.insert(mRx.end(), data, data + size);
            return true;
        }

        void UpdateDevices(mdr::Vector<MDRDeviceInfo> devices)
        {
            std::scoped_lock lock(mMutex);
            mDevices = std::move(devices);
            mDevicesReady = true;
            mRefreshPending = false;
            mRefreshRequested = false;
        }

        void Abort()
        {
            std::scoped_lock lock(mMutex);
            NextGeneration();
            mResult = MDR_RESULT_ERROR_NO_CONNECTION;
            mError = "OHOS page hidden or transport stopped";
            mAction = DISCONNECT;
            mRx.clear();
            mTx.clear();
        }

    private:
        std::mutex mMutex;
        uint32_t mGeneration{};
        Action mAction{NONE};
        MDRResult mResult{MDR_RESULT_ERROR_NO_CONNECTION};
        mdr::String mAddress;
        mdr::String mUuid;
        mdr::String mError;
        mdr::Deque<unsigned char> mRx;
        mdr::Deque<unsigned char> mTx;
        mdr::Vector<MDRDeviceInfo> mDevices;
        bool mDevicesReady{};
        bool mRefreshPending{};
        bool mRefreshRequested{};

        void NextGeneration()
        {
            if (++mGeneration == 0)
                ++mGeneration;
        }

        void Fail(const mdr::String& error)
        {
            mResult = MDR_RESULT_ERROR_NET;
            mError = error;
            mAction = DISCONNECT;
            mRx.clear();
            mTx.clear();
        }

        static MDRResult Connect(void* user, const char* address, const char* uuid)
        {
            if (!address || !uuid || !*address || !*uuid)
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            auto& self = *static_cast<ClientConnection*>(user);
            std::scoped_lock lock(self.mMutex);
            self.NextGeneration();
            self.mAddress = address;
            self.mUuid = uuid;
            self.mError.clear();
            self.mRx.clear();
            self.mTx.clear();
            self.mResult = MDR_RESULT_INPROGRESS;
            self.mAction = CONNECT;
            return MDR_RESULT_INPROGRESS;
        }

        static void Disconnect(void* user)
        {
            static_cast<ClientConnection*>(user)->Abort();
        }

        static MDRResult Receive(void* user, char* destination, int size, int* received)
        {
            if (received) *received = 0;
            if (!destination || size < 0 || !received)
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            auto& self = *static_cast<ClientConnection*>(user);
            std::scoped_lock lock(self.mMutex);
            if (self.mResult != MDR_RESULT_OK)
                return self.mResult;
            const size_t count = std::min(static_cast<size_t>(size), self.mRx.size());
            std::copy_n(self.mRx.begin(), count, destination);
            self.mRx.erase(self.mRx.begin(), self.mRx.begin() + count);
            *received = static_cast<int>(count);
            return count || size == 0 ? MDR_RESULT_OK : MDR_RESULT_INPROGRESS;
        }

        static MDRResult Send(void* user, const char* source, int size, int* sent)
        {
            if (sent) *sent = 0;
            if (!source || size < 0 || !sent)
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            auto& self = *static_cast<ClientConnection*>(user);
            std::scoped_lock lock(self.mMutex);
            if (self.mResult != MDR_RESULT_OK)
                return self.mResult;
            const size_t count = std::min(static_cast<size_t>(size), kCapacity - self.mTx.size());
            self.mTx.insert(self.mTx.end(), source, source + count);
            *sent = static_cast<int>(count);
            return count || size == 0 ? MDR_RESULT_OK : MDR_RESULT_INPROGRESS;
        }

        static MDRResult Poll(void* user, int)
        {
            auto& self = *static_cast<ClientConnection*>(user);
            std::scoped_lock lock(self.mMutex);
            if (self.mResult != MDR_RESULT_OK)
                return self.mResult;
            return !self.mRx.empty() || self.mTx.size() < kCapacity ? MDR_RESULT_OK : MDR_RESULT_ERROR_TIMEOUT;
        }

        static MDRResult Devices(void* user, MDRDeviceInfo** list, int* count)
        {
            if (!list || !count)
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            *list = nullptr;
            *count = 0;
            auto& self = *static_cast<ClientConnection*>(user);
            std::scoped_lock lock(self.mMutex);
            if (!self.mDevicesReady)
            {
                if (!self.mRefreshPending)
                {
                    self.mRefreshPending = true;
                    self.mRefreshRequested = true;
                }
                return MDR_RESULT_INPROGRESS;
            }
            if (!self.mDevices.empty())
            {
                *list = mdr::MDRAllocator<MDRDeviceInfo>{}.allocate(self.mDevices.size());
                std::uninitialized_copy(self.mDevices.begin(), self.mDevices.end(), *list);
            }
            *count = static_cast<int>(self.mDevices.size());
            self.mDevicesReady = false;
            return MDR_RESULT_OK;
        }

        static MDRResult FreeDevices(void*, MDRDeviceInfo** list)
        {
            if (!list)
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            mdr::MDRAllocator<MDRDeviceInfo>{}.deallocate(*list);
            *list = nullptr;
            return MDR_RESULT_OK;
        }

        static const char* LastError(void* user)
        {
            auto& self = *static_cast<ClientConnection*>(user);
            std::scoped_lock lock(self.mMutex);
            thread_local mdr::String error;
            error = self.mError;
            return error.c_str();
        }
    };
}
