#ifndef __TASKMANAGER_HPP__
#define __TASKMANAGER_HPP__

#include <map>
#include <list>
#include <mutex>
#include <atomic>

#include <bfc/FixedFunctionObject.hpp>

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
    std::map<uint32_t, TaskFn> mTasks;
    int mIdGen = 0;
    std::mutex mTaskMutex;
};

#endif // __TASKMANAGER_HPP__
