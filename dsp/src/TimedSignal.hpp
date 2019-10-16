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
    {
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

    double& time()
    {
        return mTime;
    }

    const double& time() const
    {
        return mTime;
    }

    size_t size() const
    {
        return mSize;
    }

    size_t& headRoomSize()
    {
        return mHeadRoomSize;
    }

    const size_t& headRoomSize() const
    {
        return mHeadRoomSize;
    }

    size_t maxSize() const
    {
        return ((mBuffer.data()+mBuffer.size())-(std::byte*)data())/sizeof(T);
    }

private:
    double mTime = 0;
    std::size_t mSize = 0;
    std::size_t mHeadRoomSize = 0;
    bfc::Buffer mBuffer = nullptr;
};

using TimedRealSignal = TimedSignal<double>;
using TimedComplexSignal = TimedSignal<std::complex<double>>;

} // namespace dsp

#endif // __TIMEDSIGNAL_HPP__