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
}

~WindowPlotTime()
{
    reset();
}

void schedule()
{
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
        }
    }
    Pipe& mInputPipe;
    GLFWwindow* mWindow;
};

#endif // __WINDOWPLOTTIME_HPP
