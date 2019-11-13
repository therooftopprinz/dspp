#ifndef __CMDTESTSIGNAL_HPP__
#define __CMDTESTSIGNAL_HPP__

#include <thread>
#include <chrono>
#include <memory>
#include <cmath>

#include <bfc/CommandManager.hpp>
#include <logless/Logger.hpp>

#include <dsp/TimedSignal.hpp>
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
        , mGenThreadRunning(false)
        , mGenThread([this](){run();})
    {
    }

    TestSignal(const TestSignal&) = delete;
    TestSignal(TestSignal&&) = default;

    std::string execute(bfc::ArgsMap&& pArgs)
    {
        LoglessTrace trace("TestSignal::execute");
        auto frequency = pArgs.argAs<int>("frequency");
        if (frequency)
        {
            mFrequency = *frequency;
        }
        return "";
    }

    ~TestSignal()
    {
        mGenThreadRunning = false;
        mGenThread.join();
    }

private:
    uint64_t time()
    {
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    }

    void run()
    {
        mGenThreadRunning = true;
        while (mGenThreadRunning)
        {
            dsp::TimedRealSignal signal(0, mPipe.allocate(mBlockSize*2), mBlockSize);
            signal.time() = time();
            const auto baseTime = signal.time();

            for (size_t i=0; i<mBlockSize; i++)
            {
                double t = double(baseTime)/(1000*1000*1000) + double(i)/mSampleRate;
                double sample = std::sin(2*pi*mFrequency*t+mPhase);
                // TODO: FIX
                // signal.emplace_back(sample);
                *(signal.data()+i) = sample;
                // Logless("sample[_] = sin(2*pi*_*_+_) = _", i, mFrequency, t, mPhase, *(signal.data()+i));
            }
            signal.resize(mBlockSize);
            mPipe.send(std::move(signal));
            std::this_thread::sleep_for(std::chrono::microseconds((1000*1000*mBlockSize)/mSampleRate));
        }
    }

    Pipe& mPipe;
    uint32_t mBlockSize;
    uint32_t mSampleRate;
    double mFrequency;
    double mPhase;
    constexpr static double pi =  3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170;
    std::atomic_bool mGenThreadRunning;
    std::thread mGenThread;
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
        
        LoglessTrace trace{"CmdTestSignal::execute"};

        auto id         = pArgs.argAs<int>("id");
        auto blockSize  = pArgs.argAs<uint32_t>("block_size");
        auto sampleRate = pArgs.argAs<uint32_t>("sample_rate");
        auto frequency  = pArgs.argAs<double>("frequency");
        auto phase      = pArgs.argAs<double>("phase");

        TestSignal* ts = nullptr;

        if (id)
        {
            Logless("id=", *id);
            auto foundIt = mSignals.find(*id);
            if (mSignals.end()!=foundIt)
            {
                ts = foundIt->second.get();
            }
        }
        else
        {
            return "id not specified!";
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

        Logless("blockSize=_ sampleRate=_ frequency=_ phase=_", *id, *sampleRate, *frequency, *phase);

        if (ts)
        {
            return ts->execute(std::move(pArgs));
        }
        else
        {
            uint32_t pipeId = mPipeMan.createPipe();
            Pipe& pipe = *mPipeMan.getPipe(pipeId);
            mSignals.emplace(*id, std::make_unique<TestSignal>(pipe, *blockSize, *sampleRate, *frequency, *phase));
            return "Test signal created pipe_out="s+std::to_string(pipeId);
        }
    }

private:
    PipeManager& mPipeMan;
    std::map<uint32_t, std::unique_ptr<TestSignal>> mSignals;
};
#endif // __CMDTESTSIGNAL_HPP__
