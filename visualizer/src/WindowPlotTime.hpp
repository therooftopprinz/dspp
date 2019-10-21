#ifndef __WINDOWPLOTTIME_HPP__
#define __WINDOWPLOTTIME_HPP__

#include <string>
#include <thread>

#include <GLFW/glfw3.h>

#include <logless/Logger.hpp>

#include <PipeManager.hpp>

class WindowPlotTimeChannel
{
public:
    enum class Status {OK, PARTIAL_COVERAGE, NO_COVERAGE};
    WindowPlotTimeChannel(uint32_t pSampleRate, uint32_t pDownsamplingRatio, Pipe& pPipeIn)
        : mSampleRate(pSampleRate)
        , mDownsamplingRatio(pDownsamplingRatio)
        , mPipeIn(pPipeIn)
    {
        mPipeIn.registerOnSendCallBack(mPipeInCallbackId, Pipe::Callback{nullptr,
            [this](dsp::TimedRealSignal&& pSignal){return onReceive(std::move(pSignal));}});
    }

    ~WindowPlotTimeChannel()
    {
        // mPipeIn.deregisterOnSendCallBack(mPipeInCallbackId);
    }

    std::string execute(bfc::ArgsMap&&)
    {
        LoglessTrace trace("WindowPlotTimeChannel::execute");
        return "";
    }

    void registerOnSignalCallback(bfc::LightFunctionObject<void(const dsp::TimedRealSignal&)>&& pCallback)
    {
        mCallback = std::move(pCallback);
    }

    Status getCurrentWindowSamples(std::vector<typename Pipe::value_type::value_type>& pBuffer, uint64_t pCenterTime)
    {
        std::unique_lock<std::mutex> lg(mPlotBufferMutex);
        auto foundIt = std::lower_bound(mPlotBuffer.begin(), mPlotBuffer.end(), pCenterTime, dsp::TimedLessCmp<dsp::TimedRealSignal>());
        if (mPlotBuffer.end() == foundIt)
        {
            return Status::NO_COVERAGE;
        }

        std::size_t middleDstIndex = pBuffer.size()/2;
        ssize_t estimatedSrcIndex = (pCenterTime - foundIt->time())/mSampleRate;

        // forward copy
        auto scanIt = foundIt;
        int64_t scanItIndex = estimatedSrcIndex;
        auto forwardDstIndex = middleDstIndex;
        while (true)
        {
            if (forwardDstIndex>=pBuffer.size())
            {
                break;
            }
            if (scanItIndex>=ssize_t(scanIt->size()))
            {
                scanIt++;
                scanItIndex = 0;
                if (mPlotBuffer.end() == scanIt)
                {
                    break;
                }
            }
            auto srcIt = std::next(scanIt->begin(), scanItIndex);
            auto dstIt = std::next(pBuffer.begin(), forwardDstIndex);
            std::size_t dstLeft = pBuffer.size()-forwardDstIndex;
            std::size_t srcLeft = scanIt->size()-scanItIndex;
            std::size_t count = std::min(dstLeft, srcLeft);
            std::copy_n(srcIt, count, dstIt);
            scanItIndex += count;
            forwardDstIndex += count;
        }

        // backward copy
        scanIt = foundIt;
        scanItIndex = estimatedSrcIndex;
        // auto backwardDstIndex = middleDstIndex;


        return Status::OK;
    }

    void setSampleRate(uint32_t pSampleRate)
    {
        mSampleRate = pSampleRate;
    }

    void setDownsamplingRatio(uint32_t pDownsamplingRatio)
    {
        mDownsamplingRatio = pDownsamplingRatio;
    }

private:
    Pipe::Status onReceive(dsp::TimedRealSignal&& pSignal)
    {
        LoglessTrace trace("WindowPlotTimeChannel::onReceive");

        if (mCallback)
        {
            mCallback(pSignal);
        }

        Logless("receive t=_ size=_", pSignal.time(), pSignal.size());

        using SampleType = typename Pipe::value_type::value_type;

        SampleType* from;
        SampleType* to;
        std::size_t fromSize  = pSignal.size();

        from = pSignal.data();

        if (!mCurrentSignal)
        {
            Logless("taking pSignal to mCurrentSignal");
            mCurrentSignalIndex = 0;
            mCurrentSignal = std::move(pSignal);
            to = mCurrentSignal.data();
        }

        Logless("mCurrentSignalIndex=_ mCurrentSignalSize=_", mCurrentSignalIndex, mCurrentSignal.size());

        to = mCurrentSignal.data();

        uint32_t fromSignalIndex = 0;
        while (true)
        {
            if (mCurrentSignalIndex==mCurrentSignal.maxSize())
            {
                mCurrentSignal.resize(mCurrentSignalIndex);
                mCurrentSignalIndex = 0;
                {
                    Logless("taking mCurrentSignal to mPlotBuffer");
                    std::unique_lock<std::mutex> lg;
                    mPlotBuffer.emplace_back(std::move(mCurrentSignal));
                }

                if (fromSignalIndex<fromSize)
                {
                    Logless("taking pSignal to mCurrentSignal");
                    mCurrentSignal = std::move(pSignal);
                }
            }

            if (fromSignalIndex>=fromSize)
            {
                break;
            }

            to[mCurrentSignalIndex] = from[fromSignalIndex];

            fromSignalIndex += mDownsamplingRatio;
            mCurrentSignalIndex++;
        }
        return Pipe::Status::OK;
    }

    uint32_t mSampleRate;
    uint32_t mDownsamplingRatio;
    Pipe& mPipeIn;
    uint32_t mPipeInCallbackId;
    std::mutex mPlotBufferMutex;
    std::deque<dsp::TimedRealSignal> mPlotBuffer;
    dsp::TimedRealSignal mCurrentSignal;
    uint32_t mCurrentSignalIndex;
    bfc::LightFunctionObject<void(const dsp::TimedRealSignal&)> mCallback;
};

class WindowPlotTime
{
public:
    WindowPlotTime(PipeManager& pPipeManager)
        : mPipeManager(pPipeManager)
    {
        LoglessTrace trace("WindowPlotTime::WindowPlotTime");

        mWindow = glfwCreateWindow(640, 480, "Time Plot", NULL, NULL);
        Logless("glfwCreateWindow = _", mWindow);
        std::cout << "WindowPlotTime: window=" << mWindow << "\n";
        if (!mWindow)
        {
            Logless("unable to create window");
            throw std::runtime_error("unable to create window");
        }
    }

    WindowPlotTime(const WindowPlotTime&) = delete;

    WindowPlotTime(WindowPlotTime&& pOther) = delete;

    ~WindowPlotTime()
    {
        reset();
    }

    void schedule()
    {
        if (!mWindow)
        {
            return;
        }

        glfwMakeContextCurrent(mWindow);
        glfwPollEvents();

        glViewport( 0, 0, 640, 480 );
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPointSize(10);
        glLineWidth(1.5); 
        glBegin(GL_LINES);
        {
            glColor3f(1.0, 0.0, 0.0);
            glVertex2f(-1,-1);
            glVertex2f(1,1);

            glColor3f(0.0, 1.0, 0.0);
            glVertex2f(1,1);
            glVertex2f(-1,1);

            glColor3f(0.0, 0.0, 1.0);
            glVertex2f(-1,1);
            glVertex2f(1,-1);
        }
        glEnd();

        glfwSwapBuffers(mWindow);
    }

    std::string execute(bfc::ArgsMap&& pArgs)
    {
        LoglessTrace trace("WindowPlotTime::execute");
        using namespace std::string_literals;
        auto channel = pArgs.argAs<uint32_t>("channel");
        auto sampleRate = pArgs.argAs<int>("sample_rate");
        auto pipeIn = pArgs.argAs<int>("pipe_in");
        if (!channel)
        {
            channel = 0;
            Logless("channel = _ assumed", *channel);
        }
        else
        {
            Logless("channel = _", *channel);
        }

        auto foundit = std::find_if(mPlotChannel.begin(), mPlotChannel.end(), [&channel](const auto& i){
            return *channel==i.first;});

        if (mPlotChannel.end() != foundit)
        {
            return foundit->second.get()->execute(std::move(pArgs));
        }

        if (!sampleRate)
        {
            Logless("no sample\\_rate!");
            return "no sample_rate!";
        }

        Pipe* pipe;

        if (!pipeIn)
        {
            Logless("no pipe\\_in!");
            return "no pipe_in!";
        }
        else
        {
            pipe = mPipeManager.getPipe(*pipeIn);
            if (!pipe)
            {
                Logless("pipe not found!");
                return "pipe not found!";
            }
        }

        Logless("channel = _ not found creating!", *channel);
        auto plotchannel = std::make_unique<WindowPlotTimeChannel>(*sampleRate, 1, *pipe);
        WindowPlotTimeChannel* plotchannelptr = plotchannel.get();
        mPlotChannel.emplace_back(*channel, std::move(plotchannel));
        return plotchannelptr->execute(std::move(pArgs));
    }

private:
    void reset()
    {
        if (mWindow)
        {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }
    }

    PipeManager& mPipeManager;
    GLFWwindow* mWindow = nullptr;
    std::optional<uint32_t> mTriggeringChannelId;
    std::mutex mPlotChannelMutex;
    std::vector<std::pair<uint32_t, std::unique_ptr<WindowPlotTimeChannel>>> mPlotChannel;
};

#endif // __WINDOWPLOTTIME_HPP__
