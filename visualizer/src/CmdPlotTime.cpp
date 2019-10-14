#include <CmdPlotTime.hpp>

CmdPlotTime::CmdPlotTime (TaskManager<bfc::LightFunctionObject<void()>>& pTaskMan, PipeManager& pPipeMan)
    : mTaskMan(pTaskMan)
    , mPipeMan(pPipeMan)
{}

std::string CmdPlotTime::execute(bfc::ArgsMap&& pArgs)
{
    using namespace std::string_literals;
    auto id = pArgs.argAs<int>("id");
    if (id)
    {
        auto foundIt = mWindows.find(*id);
        if (mWindows.end()==foundIt)
        {
            return "Time plot with id="s + std::to_string(*id) + "is not found!";
        }
        auto& tpw = foundIt->second;
        return tpw->execute(std::move(pArgs));
    }
    auto winEmp = mWindows.emplace(mIdGen++, std::make_unique<WindowPlotTime>(mPipeMan));
    auto& tpw = winEmp.first->second;
    mTaskMan.addTask(bfc::LightFunctionObject<void()>([&tpw](){tpw->schedule();}));
    return "id="s + std::to_string(*id) + "\n" + tpw->execute(std::move(pArgs));
}
