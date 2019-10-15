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
    
    TimedSignal(void* pBuffer, size_t pAllocationSize)
        : mHeader(new (pBuffer) Header{0, 0, (pAllocationSize-sizeof(mHeader))/sizeof(T)})
        , mData((T*)((uint8_t*)pBuffer+sizeof(Header)))
    {
    }

    static size_t allocationSize(size_t pN)
    {
        return sizeof(Header) + sizeof(T)*pN;
    }

    T* data()
    {
        return mData;
    }

    const T* data() const
    {
        return mData;
    }

    template <typename U>
    T* emplace_back(U&& pE)
    {
        return new (mData+mHeader->size++) T(std::forward<U>(pE));
    }

    double& time()
    {
        return mHeader->time;
    }

    const double& time() const
    {
        return mHeader->time;
    }

    size_t size() const
    {
        return mHeader->size;
    }

    size_t maxSize() const
    {
        return mHeader->maxSize;
    }

private:
    struct Header
    {
        double time=0;
        size_t size=0;
        size_t maxSize=0;
    };

    Header* mHeader;
    T* mData;
};

using TimedRealSignal = TimedSignal<double>;
using TimedComplexSignal = TimedSignal<std::complex<double>>;

} // namespace dsp


#endif // __TIMEDSIGNAL_HPP__