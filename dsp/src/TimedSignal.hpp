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

    TimedSignal(double pTime, bfc::Buffer&& pBuffer)
        : mTime(pTime)
        , mBuffer(std::move(pBuffer))
    {
    }

    TimedSignal(const TimedSignal&) = delete;
    TimedSignal(TimedSignal&& pOther)
        : mTime(pOther.mTime)
        , mSize(pOther.mSize)
    {
    }

    T* data()
    {
        return (T*)mBuffer.data();
    }

    const T* data() const
    {
        return (T*)mBuffer.data();
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

    size_t maxSize() const
    {
        return mBuffer.size()/sizeof(T);
    }

private:
    double mTime = 0;
    std::size_t mSize = 0;
    bfc::Buffer mBuffer = nullptr;
};

using TimedRealSignal = TimedSignal<double>;
using TimedComplexSignal = TimedSignal<std::complex<double>>;

} // namespace dsp

#endif // __TIMEDSIGNAL_HPP__