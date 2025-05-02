#include "game.h"
#include <raylib.h>
#include "grid.h"
#include <iostream>


Game::Game():difficulty_level(1),current_level(0),current_screen(HOME_SCREEN),gameOver(false){
    grid = new Grid;
}

Game::Game(GameScreen screen):current_screen(screen){
    grid = new Grid;
}

Game::~Game(){
    delete grid;
}


void Game::Loop(){
    switch (current_screen)
    {
    case HOME_SCREEN:
        Update_home_screen();
        Draw_home_screen();
        break;
    case GAME_SCRREN:
        Update_game_screen();
        Draw_game_screen();
        break;
    case GAMEOVER_SCREEN:
        Update_gameover_screen();
        Draw_gameover_screen();
        break;
    
    default:
        break;
    }
}

void Game::Update_game_screen(){
    if(IsKeyPressed(KEY_SPACE)) current_screen=GAMEOVER_SCREEN;
    grid->Update();
    switch (grid->getGameStatus())
    {
    case GAME_WON:
        current_level++;
        std::cout<<current_level<<'\n';
        startNewGrid();
        break;
    case GAME_LOST:
        current_screen=GAMEOVER_SCREEN;
        current_level=0;
        break;
    default:
        break;
    }

}
void Game::Update_home_screen(){
    if(IsKeyPressed(KEY_SPACE)) startNewGrid();
}
void Game::Update_gameover_screen(){
    if(IsKeyPressed(KEY_SPACE)) current_screen=HOME_SCREEN;
}

void Game::Draw_game_screen(){
    ClearBackground(LIGHTGRAY);
    grid->Draw();
    DrawText("Gra",10,10,20,BLACK);

}
void Game::Draw_home_screen(){
    ClearBackground(GREEN);
    DrawText("Menu",10,10,20,BLACK);
}
void Game::Draw_gameover_screen(){
    ClearBackground(BLUE);
    DrawText("koniec",10,10,20,BLACK);
}

void Game::startNewGrid(){
    delete grid;
    grid = new Grid;
    current_screen = GAME_SCRREN;
}