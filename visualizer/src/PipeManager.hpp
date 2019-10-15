#ifndef __PIPEMANAGER_HPP__
#define __PIPEMANAGER_HPP__

#include <map>
#include <deque>
#include <mutex>

#include <dsp/IOPort.hpp>
#include <dsp/TimedSignal.hpp>

using Pipe = dsp::IOPort<dsp::TimedRealSignal>;
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
