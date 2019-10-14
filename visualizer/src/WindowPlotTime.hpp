#ifndef __WINDOWPLOTTIME_HPP 
#define __WINDOWPLOTTIME_HPP

#include <string>
#include <thread>

#include <GLFW/glfw3.h>

#include <PipeManager.hpp>

class WindowPlotTime
{
public:
WindowPlotTime(PipeManager& pPipeManager)
    : mPipeManager(pPipeManager)
{
    mWindow = glfwCreateWindow(640, 480, "Time Plot", NULL, NULL);
    std::cout << "WindowPlotTime: window=" << mWindow << "\n";
    if (!mWindow)
    {
        throw std::runtime_error("unable to create window");
    }
}

WindowPlotTime(const WindowPlotTime&) = delete;

WindowPlotTime(WindowPlotTime&& pOther) = delete;

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

    glViewport( 0, 0, 640, 480 );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPointSize(10);
    glLineWidth(1.5); 
    glBegin(GL_LINES);
    {
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(-1,-1);
        glVertex2f(1,1);

        glColor3f(0.0, 1.0, 0.0);
        glVertex2f(1,1);
        glVertex2f(-1,1);

        glColor3f(0.0, 0.0, 1.0);
        glVertex2f(-1,1);
        glVertex2f(1,-1);
    }
    glEnd();

    glfwSwapBuffers(mWindow);
}

std::string execute(bfc::ArgsMap&& pArgs)
{
    using namespace std::string_literals;
    auto channel = pArgs.argAs<int>("channel");
    if (channel)
    {
        channel = 0;
    }
    return "";
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
    PipeManager& mPipeManager;
    GLFWwindow* mWindow = nullptr;
    std::mutex mPlotChannelMutex;
    std::vector<Pipe*> mPlotChannel;
};

#endif // __WINDOWPLOTTIME_HPP
