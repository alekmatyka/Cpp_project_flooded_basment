#pragma once
#include <raylib.h>

class Entity
{
protected:
    int x;
    int y;
    Color color;
public:
    Entity(int x_, int y_, Color col);
    virtual ~Entity();

    const int getX();
    const int getY();

    void setX(int x_);
    void setY(int y_);
    void setPos(int x_,int y_);
    void move(int key);

    virtual void Draw(int cell_width, int startX, int startY);
};

class Drain:public Entity{

    public:
    Drain(int x_, int y_, Color col) : Entity(x_, y_, col) {}

    void Draw(int cell_width, int startX, int startY) override; 
};


