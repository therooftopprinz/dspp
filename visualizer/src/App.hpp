#ifndef __APP_HPP__
#define __APP_HPP__

#include <unordered_map>
#include <list>
#include <mutex>
#include <atomic>

#include <BFC/CommandManager.hpp>
#include <BFC/FixedFunctionObject.hpp>
#include <PlotTimeWindow.hpp>


template <typename TaskFn>
class TaskManager
{
public:
    using value_type = TaskFn;

    uint32_t addTask(TaskFn&& pTask)
    {
        std::unique_lock<std::mutex> lg(mTaskMutex);
        mTasks.emplace(mIdGen, std::move(pTask));
        return mIdGen++;
    }

    int removeTask(uint32_t pId)
    {
        std::unique_lock<std::mutex> lg(mTaskMutex);
        return mTasks.erase(pId);
    }

    void scheduleAll()
    {
        std::unique_lock<std::mutex> lg(mTaskMutex);
        for (auto& task : mTasks)
        {
            task.second();
        }
    }

private:
    std::unordered_map<uint32_t, TaskFn> mTasks;
    int mIdGen = 0;
    std::mutex mTaskMutex;
};

class App
{
public:
    App();
    ~App();
    App(const App&) = delete;
    App(App&&) = default;
    int run();
private:
    using TaskFn = bfc::LightFunctionObject<void()>;
    void cliLoop();
    std::string cmdPlotTime(bfc::ArgsMap&& pArgs);
    std::string cmdExit(bfc::ArgsMap&& pArgs);
    bfc::CommandManager mCmdMan;
    std::atomic_bool mCliLoopRunning;
    std::atomic_bool mAppRunning;
    TaskManager<TaskFn> mTasks;
    std::list<PlotTimeWindow> mPlotTimeWindows;
};

#endif // __APP_HPP__