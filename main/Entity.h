#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Entity {
public:
    float dx, dy, x, y, speed, moveTimer;
    int w, h, health;
    bool life, isMove, onGround;
    sf::Texture texture;
    sf::Sprite sprite;
    std::string name;

    sf::FloatRect getRect();

    Entity(sf::Texture& texture, float X, float Y, int W, int H, std::string Name);
};