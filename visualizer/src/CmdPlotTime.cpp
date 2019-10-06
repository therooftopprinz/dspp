#include <CmdPlotTime.hpp>

CmdPlotTime::CmdPlotTime (TaskManager<bfc::LightFunctionObject<void()>>& pTaskMan, PipeManager& pPipeMan)
    : mTaskMan(pTaskMan)
    , mPipeMan(pPipeMan)
{}

std::string CmdPlotTime::execute(bfc::ArgsMap&& pArgs)
{
    using namespace std::string_literals;
    auto id = pArgs.argAs<int>("id");
    if (id) // update
    {
        auto foundIt = mWindows.find(*id);
        if (mWindows.end()==foundIt)
        {
            return "Time plot with id="s + std::to_string(*id) + "is not found!";
        }
        return "Time plot updated!";
    }

    auto pipeInId = pArgs.argAs<int>("pipe_in");
    if (!pipeInId)
    {
        return "Input pipe (pipe_in) not specified!";
    }

    auto pipe = mPipeMan.getPipe(*pipeInId);
    if (!pipe)
    {
        return "Input pipe specified not found!";
    }

    auto winEmp = mWindows.emplace(mIdGen++, WindowPlotTime(*pipe));
    auto& tpw = winEmp.first->second;
    mTaskMan.addTask(bfc::LightFunctionObject<void()>([&tpw](){tpw.schedule();}));
    return "Time plot created!";

}
