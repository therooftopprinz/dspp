#ifndef __IOPORT_HPP__
#define __IOPORT_HPP__

#include <bfc/FixedFunctionObject.hpp>
#include <bfc/MemoryPool.hpp>

namespace dsp
{

template <typename T>
class IOPort
{
public:
    enum class Status {OK, NOK, SOME_OK};
    struct Callback
    {
        bfc::LightFunctionObject<Status(const T&)> onCopy;
        bfc::LightFunctionObject<Status(T&&)> onMove;
    };

    Status send(T&& pData)
    {
        std::unique_lock<std::mutex> lg(mOnSendCallbacksMutex);
        if (1==mOnSendCallbacks.size())
        {
            return mOnSendCallbacks[0].second->onMove(std::move(pData));
        }
        std::uint32_t okCount = 0;
        for (auto& i : mOnSendCallbacks)
        {
            if (Status::OK == i.second->onCopy(pData))
            {
                okCount++;
            }
        }
        if (okCount == mOnSendCallbacks.size())
        {
            return Status::OK;
        }
        if (okCount)
        {
            return Status::SOME_OK;
        }
        return Status::NOK;
    }

    Status recv(T&& pData)
    {
        std::unique_lock<std::mutex> lg(mOnSendCallbacksMutex);
        return std::move(pData);
    }

    Status registerOnSendCallBack(std::uint32_t& pId, Callback* pCallback)
    {
        std::unique_lock<std::mutex> lg(mOnSendCallbacksMutex);
        mOnSendCallbacks.emplace_back(mIdGen, pCallback);
        pId = mIdGen++;
        return Status::OK;
    }

    Status registerOnReceiveCallBack(std::uint32_t& pId, Callback* pCallback)
    {
        std::unique_lock<std::mutex> lg(mOnRecvCallbacksMutex);
        mOnRecvCallbacks.emplace_back(mIdGen, pCallback);
        pId = mIdGen++;
        return Status::OK;
    }

    bfc::Buffer allocate(std::size_t pSize)
    {
        return mMemoryPool.allocate(pSize*sizeof(T));
    }

private:
    std::mutex mOnSendCallbacksMutex;
    std::mutex mOnRecvCallbacksMutex;
    uint32_t mIdGen = 0;
    std::vector<std::pair<std::uint32_t, Callback*>> mOnSendCallbacks;
    std::vector<std::pair<std::uint32_t, Callback*>> mOnRecvCallbacks;
    bfc::Log2MemoryPool<alignof(T)> mMemoryPool;
};

} // dsp

#endif // __IOPORT_HPP__
