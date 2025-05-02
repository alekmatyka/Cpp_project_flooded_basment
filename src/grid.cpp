#include "grid.h"
#include <raylib.h>
#include <random>
#include "entity.h"
#include <iostream>



Grid::Grid(int h, int w,double diff):height(h),width(w),difficulty(diff),player(0,0,YELLOW){
    calculateStartPosition();
    grid = new int*[height];
    for(int i = 0; i<height;i++){
        grid[i] = new int[width]();
    }

        std::random_device rd;                     
    std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> dist(0, 1);

    for(int i = 0; i<height;i++){
        for(int j =0; j<width;j++){
            grid[i][j] = dist(gen);
        }
    }
    grid[0][0]=0;
    grid[height-1][width-1]=0;

    outline.x=(turn_x-1)*cell_width+startX;
    outline.y=(turn_y-1)*cell_width+startY;
    outline.width = 3*cell_width;
    outline.height = 3*cell_width;
}

Grid::Grid(double diff):Grid(10,20,diff){}

Grid::Grid():Grid(1.0){}

Grid::~Grid(){
    for(int i = 0; i<height;i++){
        delete[] grid[i];
    }
    delete[] grid;
}

void Grid::calculateStartPosition(){
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    //znajdz maksymalna szerokosc komorki
    int maxPossibleWidth = (int)(screenWidth*0.8/width);
    int maxPossibleHeitgh = (int)(screenHeight*3*0.8/(4*height));

    cell_width = maxPossibleHeitgh>maxPossibleWidth ? maxPossibleWidth : maxPossibleHeitgh;

    //srodek planszy
    int middleX = screenWidth/2;
    int middleY = screenHeight*5/8;

    //poczatek
    if(height%2==0){
        startY = middleY-(cell_width*height/2);
    }else{
        startY = middleY-(cell_width*height/2 + cell_width/2);
    }

    if(width%2==0){
        startX = middleX-(cell_width*width/2);
    }else{
        startX = middleX-(cell_width*width/2 + cell_width/2);
    }
}


void Grid::DrawGrid(){
    Color color;
    for(int i = 0; i<height;i++){
        for(int j =0; j<width;j++){
            if(grid[i][j]==0){
                color=WHITE;
            }else{
                color=BLACK;
            }
            DrawRectangle(j*cell_width+startX,i*cell_width+startY,cell_width,cell_width,color);
            // std::cout<<grid[i][j];
        }
        // std::cout<<'\n';
    }
    DrawRectangle((height-1)*cell_width+startX,(width-1)*cell_width+startY,cell_width,cell_width,GREEN);
}

void Grid::Draw(){
    DrawGrid();
    player.Draw(cell_width,startX,startY);
    DrawRectangleLinesEx(outline,outline_width, RED);
}

void Grid::Update(){
    //logika zakonczenia

    //logika ruchu
    switch (GetKeyPressed())
    {
    case KEY_RIGHT:
        if(player.getX()<width-1 && grid[player.getY()][player.getX()+1]==0) player.move(KEY_RIGHT);
        break;
    case KEY_LEFT:
        if(player.getX()>0 && grid[player.getY()][player.getX()-1]==0) player.move(KEY_LEFT);
        break;
    case KEY_DOWN:
        if(player.getY()<height-1 && grid[player.getY()+1][player.getX()]==0) player.move(KEY_DOWN);
        break;
    case KEY_UP:
        if(player.getY()>0 && grid[player.getY()-1][player.getX()]==0) player.move(KEY_UP);
        break;
    default:
        break;
    }
}