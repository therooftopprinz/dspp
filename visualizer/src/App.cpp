#include <string>

#include <App.hpp>

int App::run()
{
    while (true)
    {
        std::string line;
        std::cout << "$ ";
        std::getline(std::cin, line);
        std::cout <<  mCmdMan.executeCommand(line) << "\n";
    }
    // sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    // sf::CircleShape shape(100.f);
    // shape.setFillColor(sf::Color::Green);

    // while (window.isOpen())
    // {
    //     sf::Event event;
    //     while (window.pollEvent(event))
    //     {
    //         if (event.type == sf::Event::Closed)
    //             window.close();
    //     }

    //     window.clear();
    //     window.draw(shape);
    //     window.display();
    // }
    return 0;
}
