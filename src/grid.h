#pragma once
#include "entity.h"

enum GameStatus{
    GAME_RUNNING,
    GAME_WON,
    GAME_LOST
};


class Grid
{
private:
    int** grid;
    int height;
    int width;
    double difficulty;
    int cell_width;
    int startX,startY;
    int turn_x=1;
    int turn_y=1;
    float outline_width = 3.5f;        
    Rectangle outline;

    Entity player;

    void calculateStartPosition();
    void DrawGrid();

    void flipClockwise();
    void flipCounterClockwise();

    void flipEntity(Entity* e,bool clockwise);

public:
    Grid();
    Grid(double diff);
    Grid(int h, int w, double diff);
    ~Grid();
    void Draw();
    void Update();
};



