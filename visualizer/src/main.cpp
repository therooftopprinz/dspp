#include <logless/Logger.hpp>

#include <App.hpp>

int main()
{
    Logger::getInstance("visualizer.logless").logless();
    return App().run();
}
