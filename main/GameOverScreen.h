#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class GameOverScreen {
public:
    GameOverScreen();
    void draw(sf::RenderWindow& window);
    bool isRestart(sf::RenderWindow& window);
private:
    sf::Font font;
    sf::Text message;
};