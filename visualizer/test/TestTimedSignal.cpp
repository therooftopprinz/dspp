#include <gtest/gtest.h>

#include <deque>

#include <bfc/Buffer.hpp>

#include <dsp/TimedSignal.hpp>

TEST(TestTimedSignal, shouldCompareLess)
{
    std::deque<dsp::TimedRealSignal> plotBuffer;
    plotBuffer.push_back(dsp::TimedRealSignal(5, bfc::Buffer()));
    plotBuffer.push_back(dsp::TimedRealSignal(4, bfc::Buffer()));
    plotBuffer.push_back(dsp::TimedRealSignal(10, bfc::Buffer()));
    plotBuffer.push_back(dsp::TimedRealSignal(15, bfc::Buffer()));
    plotBuffer.push_back(dsp::TimedRealSignal(20, bfc::Buffer()));
    {
        auto foundIt = std::upper_bound(plotBuffer.begin(), plotBuffer.end(),0, dsp::TimedLessCmp<dsp::TimedRealSignal>());
        ASSERT_TRUE(plotBuffer.end()!=foundIt);
        std::cout << foundIt->time() << "\n";
    }
    {
        auto foundIt = std::upper_bound(plotBuffer.begin(), plotBuffer.end(),12, dsp::TimedLessCmp<dsp::TimedRealSignal>());
        ASSERT_TRUE(plotBuffer.end()!=foundIt);
        std::cout << foundIt->time() << "\n";
    }
    {
        auto foundIt = std::upper_bound(plotBuffer.begin(), plotBuffer.end(),10, dsp::TimedLessCmp<dsp::TimedRealSignal>());
        ASSERT_TRUE(plotBuffer.end()!=foundIt);
        std::cout << foundIt->time() << "\n";
    }
}