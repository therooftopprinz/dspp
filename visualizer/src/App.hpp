#ifndef __APP_HPP__
#define __APP_HPP__

#include <unordered_map>
#include <list>
#include <mutex>
#include <atomic>

#include <BFC/CommandManager.hpp>
#include <BFC/FixedFunctionObject.hpp>
#include <PipeManager.hpp>
#include <TaskManager.hpp>
#include <CmdPlotTime.hpp>
#include <CmdTestSignal.hpp>

class App
{
public:
    App();
    ~App();
    App(const App&) = delete;
    App(App&&) = default;
    int run();
private:
    void cliLoop();
    std::string cmdPlotTime(bfc::ArgsMap&& pArgs);
    std::string cmdExit(bfc::ArgsMap&& pArgs);
    bfc::CommandManager mCmdMan;
    std::atomic_bool mCliLoopRunning;
    std::atomic_bool mAppRunning;
    TaskManager<bfc::LightFunctionObject<void()>> mTaskMan;
    PipeManager mPipeMan;

    CmdPlotTime mCmdPlotTime;
    CmdTestSignal mCmdTestSignal;
};

#endif // __APP_HPP__
