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
    //ruch gracza
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
    //ruch stolu obrotowego
    case KEY_W:
        if(turn_y>1) turn_y--;
        break;
    case KEY_S:
        if(turn_y<height-2) turn_y++;
        break;
    case KEY_A:
        if(turn_x>1) turn_x--;
        break;
    case KEY_D:
        if(turn_x<width-2) turn_x++;
        break;
    case KEY_E:
        flipClockwise();
        break;    
    case KEY_Q:
        flipCounterClockwise();
        break;
    default:
        break;
    }

    outline.x=(turn_x-1)*cell_width+startX;
    outline.y=(turn_y-1)*cell_width+startY;
}

void Grid::flipClockwise(){
    int temp[3][3];

    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
            temp[i+1][j+1] = grid[turn_y + j][turn_x + i];

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            grid[turn_y - 1 + i][turn_x + 1 - j] = temp[i][j];

    //przesuniecie entities
    flipEntity(&player,true);
}

void Grid::flipCounterClockwise(){
    int temp[3][3];

    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
            temp[i+1][j+1] = grid[turn_y + j][turn_x + i];
            
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            grid[turn_y + 1 - i][turn_x - 1 + j] = temp[i][j];

    //przesuniecie entities
    flipEntity(&player,false);
}

void Grid::flipEntity(Entity* e,bool clockwise){
    int relX = e->getX()-turn_x;
    int relY = e->getY()-turn_y;
    // std::cout<<relX<<' '<<relY<<'\n';
    if(abs(relX)>1 || abs(relY)>1) return;

    int newRelY, newRelX;
    if (!clockwise) {
        newRelY = -relX;
        newRelX = relY;
    } else {
        newRelY = relX;
        newRelX = -relY;
    }

    e->setPos(turn_x+newRelX,turn_y+newRelY);
}