#ifndef __CMDPLOTTIME_HPP__
#define __CMDPLOTTIME_HPP__

#include <string>
#include <memory>

#include <GLFW/glfw3.h>

#include <bfc/CommandManager.hpp>

#include <PipeManager.hpp>
#include <TaskManager.hpp>
#include <WindowPlotTime.hpp>

class CmdPlotTime
{
public:
    CmdPlotTime (TaskManager<bfc::LightFunctionObject<void()>>& pTaskMan, PipeManager& pPipeMan);
    std::string execute(bfc::ArgsMap&& pArgs);

private:
    TaskManager<bfc::LightFunctionObject<void()>>& mTaskMan;
    PipeManager& mPipeMan;
    std::map<uint32_t, std::unique_ptr<WindowPlotTime>> mWindows;
    uint32_t mIdGen = 0;
};

#endif // __CMDPLOTTIME_HPP__
