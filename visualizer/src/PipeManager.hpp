#ifndef __PIPEMANAGER_HPP__
#define __PIPEMANAGER_HPP__

#include <map>
#include <deque>
#include <mutex>

#include <BFC/MemoryPool.hpp>

class Pipe
{
public:
    void write(bfc::Buffer pData)
    {
        std::unique_lock<std::mutex> lg(mDatasLock);
        while (mBufferMax < mDatas.size())
        {
            mDatas.pop_front();
        }
        mDatas.emplace_back(std::move(pData));
    }

    bfc::Buffer read()
    {
        std::unique_lock<std::mutex> lg(mDatasLock);
        bfc::Buffer rv;
        if (mDatas.size())
        {
            rv = std::move(mDatas.front());
            mDatas.pop_front();
            return rv;
        }
        return rv;
    }

    void setBufferMax(uint32_t pBufferMax)
    {
        std::unique_lock<std::mutex> lg(mDatasLock);
        mBufferMax = pBufferMax;
    }

    bfc::Buffer allocate(std::size_t pSize)
    {
        return mMemPool.allocate(pSize);
    }

private:
    uint32_t mBufferMax=0;
    std::mutex mDatasLock;
    bfc::Log2MemoryPool mMemPool;
    std::deque<bfc::Buffer> mDatas;
};

class PipeManager
{
public:
    uint32_t createPipe()
    {
        std::unique_lock<std::mutex> lg(mPipesLock);
        mPipes.emplace(mPipesIdGen, std::make_unique<Pipe>());
        return mPipesIdGen++;
    }

    Pipe* getPipe(uint32_t pId)
    {
        auto foundIt = mPipes.find(pId);
        if (mPipes.end() != foundIt)
        {
            return foundIt->second.get();;
        }
        return nullptr;
    }

private:
    std::map<uint32_t, std::unique_ptr<Pipe>> mPipes;
    std::mutex mPipesLock;
    uint32_t mPipesIdGen = 0;
};

#endif // __PIPEMANAGER_HPP__
