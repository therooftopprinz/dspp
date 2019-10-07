#ifndef __WINDOWPLOTTIME_HPP 
#define __WINDOWPLOTTIME_HPP

#include <string>

#include <GLFW/glfw3.h>

#include <PipeManager.hpp>

class WindowPlotTime
{
public:
WindowPlotTime(Pipe& pInputPipe)
    : mInputPipe(pInputPipe)
{
    mWindow = glfwCreateWindow(640, 480, "Time Plot", NULL, NULL);
    std::cout << "WindowPlotTime: window=" << mWindow << "\n";
    if (!mWindow)
    {
        throw std::runtime_error("unable to create window");
    }
}

WindowPlotTime(const WindowPlotTime&) = delete;

WindowPlotTime(WindowPlotTime&& pOther)
    : mInputPipe (pOther.mInputPipe)
{
    reset();
    mWindow = pOther.mWindow;
    pOther.mWindow = nullptr;
}

~WindowPlotTime()
{
    reset();
}

void schedule()
{
    if (!mWindow)
    {
        return;
    }

    glfwMakeContextCurrent(mWindow);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(mWindow);
}

private:
    void reset()
    {
        if (mWindow)
        {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }
    }
    Pipe& mInputPipe;
    GLFWwindow* mWindow = nullptr;
};

#endif // __WINDOWPLOTTIME_HPP
