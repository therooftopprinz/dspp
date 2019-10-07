#ifndef __CMDTESTSIGNAL_HPP__
#define __CMDTESTSIGNAL_HPP__

#include <thread>
#include <chrono>
#include <memory>

#include <BFC/CommandManager.hpp>
#include <PipeManager.hpp>
#include <TaskManager.hpp>
#include <WindowPlotTime.hpp>

class TestSignal
{
public:
    TestSignal(Pipe& pPipe, uint32_t pBlockSize, uint32_t pSampleRate, double pFrequency, double pPhase)
        : mPipe(pPipe)
        , mBlockSize(pBlockSize)
        , mSampleRate(pSampleRate)
        , mFrequency(pFrequency)
        , mPhase(pPhase)
        , mGenThread([this](){run();})
    {
        mPipe.setBufferMax(4);
    }

    TestSignal(const TestSignal&) = delete;
    TestSignal(TestSignal&&) = default;

    ~TestSignal()
    {
        mGenThreadRunning = false;
        mGenThread.join();
    }

private:
    uint64_t time()
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now-mTimeBase).count();
    }
    void run()
    {
        mGenThreadRunning = true;
        while (mGenThreadRunning)
        {
            
        }
    }

    Pipe& mPipe;
    uint32_t mBlockSize;
    uint32_t mSampleRate;
    double mFrequency;
    double mPhase;
    double mTime;
    std::atomic_bool mGenThreadRunning;
    std::thread mGenThread;
    decltype(std::chrono::high_resolution_clock::now()) mTimeBase = std::chrono::high_resolution_clock::now();
};

class CmdTestSignal
{
public:
    CmdTestSignal (PipeManager& pPipeMan)
        : mPipeMan(pPipeMan)
    {}

    std::string execute(bfc::ArgsMap&& pArgs)
    {
        using namespace std::string_literals;

        auto id = pArgs.argAs<int>("id");
        auto blockSize = pArgs.argAs<uint32_t>("block_size");
        auto sampleRate = pArgs.argAs<uint32_t>("sample_rate");
        auto frequency = pArgs.argAs<double>("frequency");
        auto phase = pArgs.argAs<double>("phase");

        if (id) // update
        {
            auto foundIt = mSignals.find(*id);
            if (mSignals.end()==foundIt)
            {
                return "Test signal with id="s + std::to_string(*id) + "is not found!";
            }
            return "Test signal updated!";
        }
        
        if (!blockSize)
        {
            blockSize = 20;
        }
    
        if (!sampleRate)
        {
            sampleRate = 44100;
        }

        if (!frequency)
        {
            frequency = 1000;
        }

        if (!phase)
        {
            phase = 0;
        }

        uint32_t pipeId = mPipeMan.createPipe();
        Pipe& pipe = *mPipeMan.getPipe(pipeId);

        uint32_t tsigId = mIdGen++;
        mSignals.emplace(tsigId, std::make_unique<TestSignal>(pipe, *blockSize, *sampleRate, *frequency, *phase));
        return "Test signal created id=" + std::to_string(tsigId) +" pipe_out="s+std::to_string(pipeId);
    }

private:
    PipeManager& mPipeMan;
    std::map<uint32_t, std::unique_ptr<TestSignal>> mSignals;
    uint32_t mIdGen = 0;
};
#endif // __CMDTESTSIGNAL_HPP__