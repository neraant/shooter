#include "Entity.h"

sf::FloatRect Entity::getRect() {
    return sf::FloatRect(x, y, w, h);
}

Entity::Entity(sf::Texture& texture, float X, float Y, int W, int H, std::string Name) {
    x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
    speed = 0; health = 100; dx = 0; dy = 0;
    life = true; onGround = false;
    this->texture = texture;
    sprite.setTexture(this->texture);
    sprite.setOrigin(w / 2, h / 2);
}