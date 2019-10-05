#include <string>
#include <sstream>
#include <thread>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include <App.hpp>

App::App()
{
    if (!glfwInit())
    {
        throw std::runtime_error("glfwInit failed!");
    }

    mCmdMan.addCommand("plot_time", [this](bfc::ArgsMap&& pArgs) -> std::string {return cmdPlotTime(std::move(pArgs));});
    mCmdMan.addCommand("exit",      [this](bfc::ArgsMap&& pArgs) -> std::string {return cmdExit    (std::move(pArgs));});
}

App::~App()
{
    glfwTerminate();
}

std::string App::cmdPlotTime(bfc::ArgsMap&& pArgs)
{
    using namespace std::string_literals;
    mPlotTimeWindows.emplace_back();
    auto& tpw = mPlotTimeWindows.back();
    mTasks.addTask(TaskFn([&tpw](){tpw.schedule();}));
    return "Time plot created!";
}

std::string App::cmdExit(bfc::ArgsMap&& pArgs)
{
    mCliLoopRunning = false;
    mAppRunning = false;
    return "exiting...";
}

void App::cliLoop()
{
    while (mCliLoopRunning)
    {
        std::string line;
        std::cout << "$ ";
        std::getline(std::cin, line);
        std::cout <<  mCmdMan.executeCommand(line) << "\n";
    }
}

int App::run()
{
    mAppRunning = true;
    std::thread cliLoopThread([this](){cliLoop();});

    while (mAppRunning)
    {
        mTasks.scheduleAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    cliLoopThread.join();
    return 0;
}
