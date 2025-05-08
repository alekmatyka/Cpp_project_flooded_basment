#include <raylib.h>
#include "game.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


int main() 
{
    
    constexpr int screenWidth = 1200;
    constexpr int screenHeight = 960; 
    
    
    InitWindow(screenWidth, screenHeight, "Zalana Piwnica!");
    SetTargetFPS(60);
    Game game;

    Font customFont = LoadFontEx("./assets/ComicMono.ttf", 82, 0, 0);
    GuiSetFont(customFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36);

    while (!WindowShouldClose())
    {        
        BeginDrawing();
            game.Loop();
            DrawLine(0,screenHeight/4,screenWidth,screenHeight/4,BLACK);
        EndDrawing();
    }

    UnloadFont(customFont);
    CloseWindow();
}