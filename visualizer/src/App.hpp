#ifndef __APP_HPP__
#define __APP_HPP__

#include <CommandManager.hpp>

#include <SFML/Graphics.hpp>

class App
{
public:
    App() = default;
    App(const App&) = delete;
    int run();
private:
    bfc::CommandManager mCmdMan;
};

#endif // __APP_HPP__