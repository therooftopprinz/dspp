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
        mPipeIn.registerOnSendCallBack(mPipeInCallbackId, Pipe::SendCallback{nullptr,
            [this](dsp::TimedRealSignal&& pSignal) {
                onReceive(std::move(pSignal));
            }});
    }

    ~WindowPlotTimeChannel()
    {
        // mPipeIn.deregisterOnSendCallBack(mPipeInCallbackId);
    }

    std::string execute(bfc::ArgsMap&& pArgs)
    {
        LoglessTrace trace("WindowPlotTimeChannel::execute");
        auto sampleRatio = pArgs.argAs<int>("sample_ratio");
        if (sampleRatio)
        {
            mDownsamplingRatio = *sampleRatio;
        }
        return "";
    }

    void registerOnSignalCallback(bfc::LightFunctionObject<void(const dsp::TimedRealSignal&)>&& pCallback)
    {
        mCallback = std::move(pCallback);
    }

    Status getCurrentWindowSamples(std::vector<typename Pipe::value_type::value_type>& pDstBuffer, uint64_t pCenterTime)
    {
        LoglessTrace trace("WindowPlotTimeChannel::getCurrentWindowSamples");
        std::unique_lock<std::mutex> lg(mPlotBufferMutex);
        Logless("getCurrentWindowSamples size=_", mPlotBuffer.size());
        if (mPlotBuffer.begin()==mPlotBuffer.end())
        {
            return Status::NO_COVERAGE;
        }
        Logless("getCurrentWindowSamples time earliest=_", mPlotBuffer.begin()->time());
        Logless("getCurrentWindowSamples time latest=_", (mPlotBuffer.end()-1)->time());
        Logless("getCurrentWindowSamples time find=_", pCenterTime);
        auto foundIt = std::lower_bound(mPlotBuffer.begin(), mPlotBuffer.end(), pCenterTime, dsp::TimedLessCmp<dsp::TimedRealSignal>());
        if (mPlotBuffer.end() == foundIt || mPlotBuffer.begin()==foundIt)
        {
            Logless("No coverage!");
            return Status::NO_COVERAGE;
        }

        foundIt--;
        Logless("getCurrentWindowSamples time found=_", foundIt->time());

        std::size_t middleDstIndex = pDstBuffer.size()/2;
        ssize_t estimatedSrcIndex = mSampleRate*double(pCenterTime - foundIt->time())/(1000*1000*1000);

        // forward copy
        auto scanIt = foundIt;
        auto forwardSrcIndex = estimatedSrcIndex;
        auto forwardDstIndex = middleDstIndex;
        Logless("getCurrentWindowSamples forward forwardSrcIndex=_", forwardSrcIndex);
        Logless("getCurrentWindowSamples forward forwardDstIndex=_", forwardDstIndex);
        while (true)
        {
            if (forwardDstIndex>=pDstBuffer.size())
            {
                break;
            }
            if (forwardSrcIndex>=ssize_t(scanIt->size()))
            {
                Logless("fetching next, forwardDstIndex=_", forwardDstIndex);
                scanIt++;
                forwardSrcIndex = 0;
                if (mPlotBuffer.end() == scanIt)
                {
                    break;
                }
                Logless("next time=_", scanIt->time());
            }
            auto srcIt = std::next(scanIt->begin(), forwardSrcIndex);
            auto dstIt = std::next(pDstBuffer.begin(), forwardDstIndex);
            std::size_t dstLeft = pDstBuffer.size()-forwardDstIndex;
            std::size_t srcLeft = scanIt->size()-forwardSrcIndex;
            std::size_t count = std::min(dstLeft, srcLeft);
            Logless("getCurrentWindowSamples forward srcIndex=_@_ srcLeft=_ srcSize=_ srcMaxSize=_", forwardSrcIndex, scanIt->data()+forwardSrcIndex, srcLeft, scanIt->size(), scanIt->maxSize());
            Logless("getCurrentWindowSamples forward dstIndex=_@_ dstLeft=_ dstSize=_", forwardDstIndex, pDstBuffer.data()+forwardDstIndex, dstLeft, pDstBuffer.size());
            Logless("getCurrentWindowSamples forward count=_", count);
            std::copy_n(srcIt, count, dstIt);
            for (std::size_t i = 0; i<count; i++)
            {
                Logless("forward copy signal[_]=_", i, dstIt[i]);
            }
            forwardSrcIndex += count;
            forwardDstIndex += count;
        }

        // backward copy
        scanIt = foundIt;
        ssize_t backwardSrcIndex = estimatedSrcIndex;
        ssize_t backwardDstIndex = middleDstIndex;

        if (backwardSrcIndex>=ssize_t(scanIt->size()))
        {
            backwardSrcIndex = scanIt->size();
        }
        Logless("getCurrentWindowSamples backward backwardSrcIndex=_", backwardSrcIndex);
        Logless("getCurrentWindowSamples backward backwardDstIndex=_", backwardDstIndex);

        while (true)
        {
            if (backwardDstIndex<=0)
            {
                break;
            }
            if (backwardSrcIndex<=0)
            {
                if (mPlotBuffer.begin() == scanIt)
                {
                    break;
                }
                scanIt--;
                backwardSrcIndex = scanIt->size();
                Logless("fetching previous, backwardDstIndex=", backwardSrcIndex);
                Logless("previous time=_", scanIt->time());
            }
            auto count = std::min(backwardSrcIndex,    backwardDstIndex);
            auto srcIt = std::next(scanIt->begin(),    backwardSrcIndex-count);
            auto dstIt = std::next(pDstBuffer.begin(), backwardDstIndex-count);
            Logless("getCurrentWindowSamples backward count=_", count);
            Logless("getCurrentWindowSamples backward srcIndex=_@_ srcLeftIndex=_ srcSize=_", backwardSrcIndex-count, scanIt->data()+   (backwardSrcIndex-count), backwardSrcIndex, scanIt->size());
            Logless("getCurrentWindowSamples backward dstIndex=_@_ dstLeftIndex=_ dstSize=_", backwardDstIndex-count, pDstBuffer.data()+(backwardDstIndex-count), backwardDstIndex, pDstBuffer.size());
            std::copy_n(srcIt, count, dstIt);
            for (int i = 0; i<count; i++)
            {
                Logless("backward copy signal[_]=_", i, dstIt[i]);
            }
            backwardSrcIndex -= count;
            backwardDstIndex -= count;
        }

        for (std::size_t i = 0; i<pDstBuffer.size(); i++)
        {
            Logless("display signal[_]=_", i, pDstBuffer[i]);
        }

        for (std::size_t i=0; i<(pDstBuffer.size()-1); i++)
        {
            if (std::fabs(pDstBuffer[i]-pDstBuffer[i+1])>0.1)
            {
                Logless("getCurrentWindowSamples glitch at [_]=_ and [_]=_", i, pDstBuffer[i], i+1, pDstBuffer[i+1]);
            }
        }

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
    void onReceive(dsp::TimedRealSignal&& pSignal)
    {
        LoglessTrace trace("WindowPlotTimeChannel::onReceive");

        Logless("receive t=_ size=_", pSignal.time(), pSignal.size());
        for (size_t i = 0; i<pSignal.size(); i++)
        {
            Logless("signal[_]=_", i, *(pSignal.data()+i));
        }

        using SampleType = typename Pipe::value_type::value_type;

        SampleType* from;
        SampleType* to;
        std::size_t fromSize  = pSignal.size();
        uint32_t fromSignalIndex = 0;

        if (mDownsamplingSampleSkip)
        {
            if (mDownsamplingSampleSkip >= pSignal.size())
            {
                mDownsamplingSampleSkip -= pSignal.size();
                return Pipe::Status::OK;
            }

            fromSignalIndex = mDownsamplingRatio;
            mDownsamplingSampleSkip = mDownsamplingRatio;
        }
        else
        {
            mDownsamplingSampleSkip = mDownsamplingRatio;
        }

        from = pSignal.data();

        if (!mCurrentSignal)
        {
            Logless("taking pSignal to mCurrentSignal");
            mCurrentSignalIndex = 0;
            mCurrentSignal = std::move(pSignal);
            mCurrentSignal.resize(0);
        }

        Logless("mCurrentSignalIndex=_ mCurrentSignalSize=_", mCurrentSignalIndex, mCurrentSignal.size());

        to = mCurrentSignal.data();

        while (true)
        {
            if (mCurrentSignalIndex==mCurrentSignal.maxSize())
            {
                mCurrentSignal.resize(mCurrentSignalIndex);
                mCurrentSignalIndex = 0;
                {
                    Logless("taking mCurrentSignal to mPlotBuffer");
                    std::unique_lock<std::mutex> lg;

                    Logless("emplace receive t=_ size=_", mCurrentSignal.time(), mCurrentSignal.size());
                    for (size_t i = 0; i<mCurrentSignal.size(); i++)
                    {
                        Logless("emplace signal[_]@_=_", i, mCurrentSignal.data()+i, *(mCurrentSignal.data()+i));
                    }

                    if (mCallback)
                    {
                        mCallback(mCurrentSignal);
                    }

                    mPlotBuffer.emplace_back(std::move(mCurrentSignal));

                    // limit plot buffer

                    if (mPlotBuffer.size()==1000)
                    {
                        mPlotBuffer.pop_front();
                    }
                    Logless("mPlotBuffer.size()=_", mPlotBuffer.size());
                }

                if (fromSignalIndex<fromSize)
                {
                    Logless("taking pSignal to mCurrentSignal");
                    mCurrentSignal = std::move(pSignal);
                    to = mCurrentSignal.data();
                }
            }

            if (fromSignalIndex>=fromSize)
            {
                break;
            }

            to[mCurrentSignalIndex] = from[fromSignalIndex];
            Logless("set to[_]@_ = from[_]@_ = _", mCurrentSignalIndex, to+mCurrentSignalIndex,
                fromSignalIndex, from+fromSignalIndex, from[fromSignalIndex]);

            fromSignalIndex += mDownsamplingRatio;
            mCurrentSignalIndex++;
        }
    }

    uint32_t mSampleRate;
    uint32_t mDownsamplingRatio;
    uint32_t mDownsamplingSampleSkip = 0;
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
        mDisplayBuffer.resize(1000);
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
        glLineWidth(1); 
        {
            std::unique_lock<std::mutex> lg(mPlotChannelMutex);
            for (auto& i : mPlotChannel)
            {
                glBegin(GL_LINES);
                {
                    i.second->getCurrentWindowSamples(mDisplayBuffer, mTriggerTime);
                    Logless("schedule plot: t=_", mTriggerTime);
                    for (std::size_t i=0; i<mDisplayBuffer.size()-1; i++)
                    {
                        double x0 = 2*(0.5-double(i)/mDisplayBuffer.size());
                        double x1 = 2*(0.5-double(i+1)/mDisplayBuffer.size());
                        glColor3f(double(i)/mDisplayBuffer.size(), double(1-i)/mDisplayBuffer.size(), 0.0);
                        glVertex2f(x0, mDisplayBuffer[i]*0.5);
                        glVertex2f(x1, mDisplayBuffer[i+1]*0.5);
                    }
                }
                glEnd();
            }
        }

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
        plotchannelptr->registerOnSignalCallback([this](const dsp::TimedRealSignal& pSig)
            {mTriggerTime = pSig.time()-1000*1000*100;});
        std::unique_lock<std::mutex> lg(mPlotChannelMutex);
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
    std::vector<Pipe::value_type::value_type> mDisplayBuffer;
    uint64_t mTriggerTime=0;
    std::vector<std::pair<uint32_t, std::unique_ptr<WindowPlotTimeChannel>>> mPlotChannel;
};

#endif // __WINDOWPLOTTIME_HPP__
