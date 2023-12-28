#pragma once
#include <SFML/Graphics.hpp>
#include "Map.h"

using namespace sf;

View view;

View GetPlayerCoordinateForView(float x, float y) {
    float viewWidth = view.getSize().x;
    float viewHeight = view.getSize().y;

    // ������� ����� � ��������
    float mapWidth = WIDTH_MAP * 32; // ������ ����� � ��������
    float mapHeight = HEIGHT_MAP * 32; // ������ ����� � ��������

    if (x - viewWidth / 2 < 0)
        x = viewWidth / 2;
    if (y - viewHeight / 2 < 0)
        y = viewHeight / 2;
    if (x + viewWidth / 2 > mapWidth)
        x = mapWidth - viewWidth / 2;
    if (y + viewHeight / 2 > mapHeight)
        y = mapHeight - viewHeight / 2;

    view.setCenter(x, y);
    return view;
}