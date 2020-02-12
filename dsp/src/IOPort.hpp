#ifndef __IOPORT_HPP__
#define __IOPORT_HPP__

#include <bfc/FixedFunctionObject.hpp>
#include <bfc/MemoryPool.hpp>

namespace dsp
{

/**
 * @brief Provides communication point between processing blocks
 * @tparam T Fundametal type for sending and receiving.
 */
template <typename T>
class IOPort
{
public:
    using value_type = T;

    using Copier = bfc::LightFunctionObject<void(const T&)>;
    using Receiver = bfc::LightFunctionObject<void(T&)>;
    using Mover = bfc::LightFunctionObject<void(T&&)>;
    struct SendCallback
    {
        Mover onCopy;
        Mover onMove;
    };
    /**
     * Send data via move to the first send listener of the port.
     * The producer block invokes this function when operating in producer driven mode.
     * The callback can be blocking or non blocking.
     */
    void send(T&& pData) 
    {
        if (mOnSendCallbacks.size())
        {
            mOnSendCallbacks[0].second.onMove(std::move(pData));
        }
    }

    /**
     * Send data via copy to the all the send listener of the port.
     * The producer block invokes this function.
     * The callback can be blocking or non blocking.
     */
    void sendToAll(T&& pData)
    {
        for (auto& i : mOnSendCallbacks)
        {
            i.second.onCopy(pData);
        }
    }

    /**
     * Receive data from tge receive listener of the port.
     * The consumer block invokes this function.
     * The callback can be blocking or non blocking
     */
    void recv(T& pData)
    {
        mOnRecvCallback(pData);
    }

    /**
     * Register send callback for the port.
     * Can register multiple send callbacks for multi consumer mode.
     * This must be invoked before or after send and sendToAll, concurrency should be handled by the block code.
     */
    void registerOnSendCallBack(std::uint32_t& pId, SendCallback pCallback)
    {
        mOnSendCallbacks.emplace_back(mIdGen, std::move(pCallback));
        pId = mIdGen++;
    }

    /**
     * Register receive callback for the port.
     */
    void registerOnReceiveCallBack(Receiver pCallback)
    {
        mOnRecvCallback = std::move(pCallback);
    }

    /**
     * Allocate a Buffer from pool, the pool is managed by this port.
     * The Buffer is automatically returned to the pool when destroyed.
     */
    bfc::Buffer allocate(std::size_t pSize)
    {
        return mMemoryPool.allocate(pSize*sizeof(T));
    }

private:
    std::mutex mOnSendCallbacksMutex;
    uint32_t mIdGen = 0;
    std::vector<std::pair<std::uint32_t, SendCallback>> mOnSendCallbacks;
    Receiver mOnRecvCallback;
    bfc::Log2MemoryPool<alignof(T)> mMemoryPool;
};

} // dsp

#endif // __IOPORT_HPP__
