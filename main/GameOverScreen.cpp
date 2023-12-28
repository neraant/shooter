#include "GameOverScreen.h"

GameOverScreen::GameOverScreen() {
    if (!font.loadFromFile("fonts/ArialBold.ttf")) {
        // ��������� ������ ��� �������� ������
    }
    message.setFont(font);
    message.setCharacterSize(40); // � ��������, � �� � ������!
    message.setFillColor(sf::Color::White);
    message.setString("PRESS ENTER TO RESTART");
}

void GameOverScreen::draw(sf::RenderWindow& window) {
    // �������� ������� ����
    sf::Vector2u windowSize = window.getSize();

    // ������������� ������� ������ � ������ ������
    sf::FloatRect textRect = message.getLocalBounds();
    message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    message.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f));

    // ������������ ����� �� ������
    window.draw(message);
}

bool GameOverScreen::isRestart(sf::RenderWindow& window) {
    // ���������, ������ �� ������� Enter
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        return true;
    }
    return false;
}