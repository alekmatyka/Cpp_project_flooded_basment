#pragma once
#include "entity.h"
#include <vector>

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
    GameStatus status = GAME_RUNNING;
    int turn = 0;
    int waterCycleLength;
    
    Rectangle outline;
    Rectangle waterCycleOutline;
    Entity player;

    Entity* YoMamaYoMama;
    bool YoMamaActive=false;
    bool YoMamaVertical=false;
    int YoMamaDirection = 1;

    void calculateStartPosition();
    void DrawGrid();

    void flipClockwise();
    void flipCounterClockwise();
    void flipEntity(Entity* e,bool clockwise);

    void spawnWater();
    void spawnYoMama();

    void updateEntites();
    void updateWater();
    void updateYoMama();

    bool checkPlayerCollison();
public:
    Grid();
    Grid(double diff);
    Grid(int h, int w, double diff);
    ~Grid();
    void Draw();
    void Update();
    GameStatus getGameStatus();
};



