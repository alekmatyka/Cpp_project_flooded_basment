#include "game.h"
#include <raylib.h>
#include "grid.h"
#include <iostream>
#include <random>


Game::Game():difficulty_level(1),current_level(0),highscore(0),current_screen(HOME_SCREEN),gameOver(false){
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
    GuiSetStyle(BUTTON, TEXT_SIZE, 40);
    if(IsKeyPressed(KEY_SPACE)) startNewGrid();
    if(GuiButton((Rectangle){ 600-120, 300, 2*120, 100 }, "Easy")){
        difficulty_level=1;
        startNewGrid();
    }
    if(GuiButton((Rectangle){ 600-120, 300+150, 2*120, 100 }, "Hard")){
        difficulty_level=2;
        startNewGrid();
    }
}
void Game::Update_gameover_screen(){
    if(IsKeyPressed(KEY_SPACE) || GuiButton((Rectangle){ 600-120, 300, 2*120, 100 }, "Menu Glowne")){
        current_screen=HOME_SCREEN;
        if(current_level>highscore) highscore=current_level;
        current_level=0;
    }
    if(GuiButton((Rectangle){ 600-120, 300+150, 2*120, 100 }, "Zagraj Ponownie")){
        if(current_level>highscore) highscore=current_level;
        current_level=0;
        startNewGrid();
    }
}

void Game::Draw_game_screen(){
    ClearBackground(LIGHTGRAY);
    grid->Draw();
    DrawText("Gra",10,10,20,BLACK);
    std::string score = "Wynik: "+ std::to_string(current_level);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiLabel((Rectangle){ 600-120, 100, 400, 100 }, score.c_str());
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

}
void Game::Draw_home_screen(){
    ClearBackground(GREEN);
    DrawText("Menu",10,10,20,BLACK);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 96);
    GuiLabel((Rectangle){ 240, 50, 960, 100 }, "Zalana Piwnica!");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
}
void Game::Draw_gameover_screen(){
    ClearBackground(LIGHTGRAY);
    DrawText("koniec",10,10,20,BLACK);
    std::string score = "Wynik: "+ std::to_string(current_level);
    std::string hscore = "Highscore: "+ std::to_string(highscore);
    GuiLabel((Rectangle){ 600-120, 50, 2*120, 100 }, score.c_str());
    GuiLabel((Rectangle){ 600-120, 150, 2*120, 100 }, hscore.c_str());
}

void Game::startNewGrid(){
    std::random_device rd;                     
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disth(10, 25);
    std::uniform_int_distribution<> distw(15, 35);
    int h = disth(gen);
    int w = distw(gen);
    double diff = difficulty_level + (double)current_level/10;
    diff = std::min(diff,4.0);
    // std::cout<<"hallo "<<diff<<" "<<(double)current_level/10<<'\n';
    delete grid;
    grid = new Grid(h,w,diff);
    current_screen = GAME_SCRREN;
}