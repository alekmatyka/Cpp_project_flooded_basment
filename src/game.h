#pragma once
#include <raylib.h>
#include "grid.h"


enum GameScreen{
    HOME_SCREEN,
    GAME_SCRREN,
    GAMEOVER_SCREEN
};

class Game
{
private:
    int difficulty_level;
    int current_level;
    GameScreen current_screen;
    Grid *grid;
    
    void Draw_home_screen();
    void Update_home_screen();
    void Draw_game_screen();
    void Update_game_screen();
    void Draw_gameover_screen();
    void Update_gameover_screen();
    void startNewGrid();

    bool gameOver=false;
public:
    Game();
    Game(GameScreen screen);
    ~Game();
    void Loop();
};

