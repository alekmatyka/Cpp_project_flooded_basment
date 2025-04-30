#pragma once
#include <raylib.h>

class Entity
{
private:
    int x;
    int y;
    Color color;
public:
    Entity(int x_, int y_, Color col);
    ~Entity();

    const int getX();
    const int getY();

    void setX(int x_);
    void setY(int y_);
    void setPos(int x_,int y_);
    void move(int key);

    void Draw(int cell_width, int startX, int startY);
};


