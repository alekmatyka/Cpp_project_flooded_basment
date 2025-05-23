#include "entity.h"


Entity::Entity(int x_, int y_, Color col):x(x_),y(y_),color(col)
{
}

Entity::~Entity()
{
}

const int Entity::getX(){
    return x;
}

const int Entity::getY(){
    return y;
}



void Entity::move(int key){
    if(key==KEY_LEFT) x--;
    else if(key==KEY_RIGHT) x++;
    else if(key==KEY_UP) y--;
    else if(key==KEY_DOWN) y++;
}

void Entity::setX(int x_){
    x=x_;
}

void Entity::setY(int y_){
    y=y_;
}

void Entity::setPos(int x_,int y_){
    setX(x_);
    setY(y_);
}

void Entity::Draw(int cell_width, int startX, int startY){
    DrawRectangle(x*cell_width+startX+2,y*cell_width+startY+2,cell_width-4,cell_width-4,color);
}

void Drain::Draw(int cell_width, int startX, int startY){
    DrawCircle(x*cell_width+startX+cell_width/2,y*cell_width+startY+cell_width/2,cell_width/2-3,color);
}