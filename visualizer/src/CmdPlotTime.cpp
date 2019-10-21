#include <CmdPlotTime.hpp>
#include <logless/Logger.hpp>

CmdPlotTime::CmdPlotTime (TaskManager<bfc::LightFunctionObject<void()>>& pTaskMan, PipeManager& pPipeMan)
    : mTaskMan(pTaskMan)
    , mPipeMan(pPipeMan)
{}

std::string CmdPlotTime::execute(bfc::ArgsMap&& pArgs)
{
    LoglessTrace trace{"CmdPlotTime::execute"};

    using namespace std::string_literals;
    auto id = pArgs.argAs<int>("id");
    if (id)
    {
        Logless("id=_", *id);
        auto foundIt = mWindows.find(*id);
        if (mWindows.end()!=foundIt)
        {
            return foundIt->second->execute(std::move(pArgs));
        }
    }
    else
    {
        return "id not specified!";
    }
    auto winEmp = mWindows.emplace(*id, std::make_unique<WindowPlotTime>(mPipeMan));
    auto& tpw = winEmp.first->second;
    mTaskMan.addTask(bfc::LightFunctionObject<void()>([&tpw](){tpw->schedule();}));
    return tpw->execute(std::move(pArgs));
}
