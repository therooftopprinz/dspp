#ifndef __TIMEDSIGNAL_HPP__
#define __TIMEDSIGNAL_HPP__

#include <cstddef>
#include <cmath>
#include <complex>

namespace dsp
{

template <typename T>
struct TimedSignal
{
public:

    using value_type = T;

    TimedSignal() = default;

    TimedSignal(double pTime, bfc::Buffer&& pBuffer, std::size_t pHeadRoomSize=0)
        : mTime(pTime)
        , mHeadRoomSize(pHeadRoomSize)
        , mBuffer(std::move(pBuffer))
    {
        if (mBuffer.size()%sizeof(T))
        {
            throw std::runtime_error("Buffer size not aligned!");
        }
    }

    TimedSignal(const TimedSignal&) = delete;

    TimedSignal(TimedSignal&& pOther)
        : mTime(pOther.mTime)
        , mSize(pOther.mSize)
        , mHeadRoomSize(pOther.mHeadRoomSize)
        , mBuffer(std::move(pOther.mBuffer))
    {
    }

    TimedSignal& operator=(const TimedSignal&) = delete;
    TimedSignal& operator=(TimedSignal&& pOther)
    {
        mTime = pOther.mTime;
        mSize = pOther.mSize;
        mHeadRoomSize = pOther.mHeadRoomSize;
        mBuffer = std::move(pOther.mBuffer);
        return *this;
    }

    T* data()
    {
        return (T*)mBuffer.data()+mHeadRoomSize;
    }

    const T* data() const
    {
        return (T*)mBuffer.data()+mHeadRoomSize;
    }

    T* begin()
    {
        return data();
    }

    T* end()
    {
        return data()+size();
    }

    const T* begin() const
    {
        return data();
    }

    const T* end() const
    {
        return data()+size();
    }

    template <typename U>
    T* emplace_back(U&& pE)
    {
        return new (mBuffer.data()+sizeof(T)*mSize++) T(std::forward<U>(pE));
    }

    uint64_t& time()
    {
        return mTime;
    }

    const uint64_t& time() const
    {
        return mTime;
    }

    std::size_t size() const
    {
        return mSize;
    }

    void resize(std::size_t pSize)
    {
        mSize = pSize;
    }

    std::size_t& headRoomSize()
    {
        return mHeadRoomSize;
    }

    const std::size_t& headRoomSize() const
    {
        return mHeadRoomSize;
    }

    std::size_t maxSize() const
    {
        return ((mBuffer.data()+mBuffer.size())-(std::byte*)data())/sizeof(T);
    }

    operator bool()
    {
        return nullptr != mBuffer.data();
    }

private:
    uint64_t mTime = 0;
    std::size_t mSize = 0;
    std::size_t mHeadRoomSize = 0;
    bfc::Buffer mBuffer;
};

using TimedRealSignal = TimedSignal<double>;
using TimedComplexSignal = TimedSignal<std::complex<double>>;

template <typename T>
struct TimedLessCmp
{
    bool operator()(const T& a, uint64_t b)
    {
        return a.time() < b;
    }
    bool operator()(uint64_t a, const T& b)
    {
        return a < b.time();
    }
};

} // namespace dsp

#endif // __TIMEDSIGNAL_HPP__