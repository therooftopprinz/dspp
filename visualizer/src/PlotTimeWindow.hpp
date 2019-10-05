#ifndef __PLOTTIMEWINDOW_HPP__
#define __PLOTTIMEWINDOW_HPP__

#include <GLFW/glfw3.h>

class PlotTimeWindow
{
public:
PlotTimeWindow()
{
    mWindow = glfwCreateWindow(640, 480, "Time Plot", NULL, NULL);
    if (!mWindow)
    {
        throw std::runtime_error("unable to create window");
    }
}

~PlotTimeWindow()
{
    glfwDestroyWindow(mWindow);
}

void schedule()
{
    glfwMakeContextCurrent(mWindow);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(mWindow);
}

private:
    GLFWwindow* mWindow;
};

#endif // __PLOTTIMEWINDOW_HPP__