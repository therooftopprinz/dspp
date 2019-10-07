#include <string>
#include <sstream>
#include <thread>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include <App.hpp>

App::App()
    : mCmdPlotTime  (mTaskMan, mPipeMan)
    , mCmdTestSignal(mPipeMan)
{
    if (!glfwInit())
    {
        throw std::runtime_error("glfwInit failed!");
    }

    mCmdMan.addCommand("exit",        [this](bfc::ArgsMap&& pArgs) -> std::string {return cmdExit(std::move(pArgs));});
    mCmdMan.addCommand("plot_time",   [this](bfc::ArgsMap&& pArgs) -> std::string {return mCmdPlotTime.execute(std::move(pArgs));});
    mCmdMan.addCommand("test_signal", [this](bfc::ArgsMap&& pArgs) -> std::string {return mCmdTestSignal.execute(std::move(pArgs));});
}

App::~App()
{
    glfwTerminate();
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
    mCliLoopRunning = true;
    std::thread cliLoopThread([this](){cliLoop();});

    while (mAppRunning)
    {
        mTaskMan.scheduleAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    cliLoopThread.join();
    return 0;
}
