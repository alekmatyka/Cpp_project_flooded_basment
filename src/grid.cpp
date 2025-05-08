#include "grid.h"
#include <raylib.h>
#include <random>
#include "entity.h"
#include <iostream>



Grid::Grid(int h, int w,double diff):height(h),width(w),difficulty(diff),player(0,0,YELLOW){
    calculateStartPosition();
    waterCycleLength = (int) (h+w)/diff;
    grid = new int*[height];
    for(int i = 0; i<height;i++){
        grid[i] = new int[width]();
    }

    // std::cout<<difficulty<<'\n';

    std::random_device rd;                     
    std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> dist(0, 1);


    //TODO: zmiana mechanik na podstawie difficulty

    //wypelnianie scian
    for(int i = 0; i<height;i++){
        for(int j =0; j<width;j++){
            grid[i][j] = dist(gen);
        }
    }
    grid[0][0]=0;
    grid[height-1][width-1]=0;

    //stol obrotowy
    outline.x=(turn_x-1)*cell_width+startX;
    outline.y=(turn_y-1)*cell_width+startY;
    outline.width = 3*cell_width;
    outline.height = 3*cell_width;

    waterCycleOutline.x = startX;
    waterCycleOutline.y = startY-cell_width*2;
    waterCycleOutline.width = cell_width*width;
    waterCycleOutline.height = cell_width;

    //spawn wody
    spawnWater();
    //spawn yomama
    spawnYoMama();

}

Grid::Grid(double diff):Grid(10,20,diff){}

Grid::Grid():Grid(1.0){}

Grid::~Grid(){
    for(int i = 0; i<height;i++){
        delete[] grid[i];
    }
    delete[] grid;
    if(YoMamaActive)    delete YoMamaYoMama;
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
            }else if(grid[i][j]==1){
                color=BLACK;
            }else{
                color=SKYBLUE;
            }
            DrawRectangle(j*cell_width+startX,i*cell_width+startY,cell_width,cell_width,color);
            // std::cout<<grid[i][j];
        }
        // std::cout<<'\n';
    }
    if(grid[height-1][width-1]==0) 
        DrawRectangle((width-1)*cell_width+startX,(height-1)*cell_width+startY,cell_width,cell_width,GREEN);
    else
        DrawRectangle((width-1)*cell_width+startX,(height-1)*cell_width+startY,cell_width,cell_width,ORANGE);
}

void Grid::Draw(){
    DrawGrid();
    player.Draw(cell_width,startX,startY);
    if(YoMamaActive)
        YoMamaYoMama->Draw(cell_width,startX,startY);
    DrawRectangleLinesEx(outline,outline_width, RED);

    //prgressbar cyklu wody
    int waterLength = (int) (turn%waterCycleLength)*width*cell_width/waterCycleLength;
    DrawRectangle(waterCycleOutline.x,waterCycleOutline.y,waterLength,cell_width,WHITE);

    DrawRectangleLinesEx(waterCycleOutline,outline_width,BLACK);

}

void Grid::Update(){

    bool entityUpdate = true;
    //logika zakonczenia
    if(player.getX()==width-1 && player.getY()==height-1) status = GAME_WON;
    if(checkPlayerCollison()) status = GAME_LOST;
    //TODO: spowolnic rozprzestrzenianie sie wody - done
    //logika ruchu
    switch (GetKeyPressed())
    {
    //ruch gracza
    case KEY_RIGHT:
        if(player.getX()<width-1 && grid[player.getY()][player.getX()+1]!=1) player.move(KEY_RIGHT);
        break;
    case KEY_LEFT:
        if(player.getX()>0 && grid[player.getY()][player.getX()-1]!=1) player.move(KEY_LEFT);
        break;
    case KEY_DOWN:
        if(player.getY()<height-1 && grid[player.getY()+1][player.getX()]!=1) player.move(KEY_DOWN);
        break;
    case KEY_UP:
        if(player.getY()>0 && grid[player.getY()-1][player.getX()]!=1) player.move(KEY_UP);
        break;
    //ruch stolu obrotowego, nie updateujemy entities gdy tylko przesuwamy ramke
    case KEY_W:
        if(turn_y>1) turn_y--;
        entityUpdate = false;
        break;
    case KEY_S:
        if(turn_y<height-2) turn_y++;
        entityUpdate = false;
        break;
    case KEY_A:
        if(turn_x>1) turn_x--;
        entityUpdate = false;
        break;
    case KEY_D:
        if(turn_x<width-2) turn_x++;
        entityUpdate = false;
        break;
    case KEY_E:
        updateEntites();
        flipClockwise();
        entityUpdate = false;
        break;    
    case KEY_Q:
        updateEntites();
        flipCounterClockwise();
        entityUpdate = false;
        break;
    default:
        entityUpdate = false;
        break;
    }
    
    //updateujemy wszystkie entities;
    if(entityUpdate) updateEntites();

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

GameStatus Grid::getGameStatus(){
    return status;
}

void Grid::spawnWater(){
    //setup maszyny losujacej
    std::random_device rd;                     
    std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> distX(0, width-1);
    std::uniform_int_distribution<> distY(0, height-1);

    int waterCount = (int) width*height*0.075;

    while(waterCount>0){
        int waterX = distX(gen);
        int waterY = distY(gen);
        //nie zalewac poczatku i konca
        if((waterX == 0 && waterY==0) || (waterX == width-1 && waterY==height-1)) continue;
        //nie zalewac scian
        if(grid[waterY][waterX]==1){
            waterCount--;
            continue;
        }
        grid[waterY][waterX]=2;
        waterCount--;
    }

}

bool Grid::checkPlayerCollison(){
    if(grid[player.getY()][player.getX()]==2) return true;
    if(player.getX()==YoMamaYoMama->getX() && player.getY()==YoMamaYoMama->getY()) return true; 

    return false;
}


void Grid::updateEntites(){
    turn++;
    //update wody
    if(turn%waterCycleLength==0)
        updateWater();
    if(YoMamaActive)
        updateYoMama();

}

void Grid::updateWater(){
    for(int i = 0; i<height;i++){
        for(int j =0; j<width;j++){
            if(grid[i][j]==0 && (i!=height-1 || j!=width-1)){
                if(i>0 && grid[i-1][j]==2) grid[i][j]=3;
                else if(i<height-1 && grid[i+1][j]==2) grid[i][j]=3;
                else if(j>0 && grid[i][j-1]==2) grid[i][j]=3;
                else if(j<width-1 && grid[i][j+1]==2) grid[i][j]=3;
            }  
        }
    }

    for(int i = 0; i<height;i++){
        for(int j =0; j<width;j++){
            if(grid[i][j]==3) grid[i][j]=2;   
        }
    }

}

void Grid::spawnYoMama(){
    std::random_device rd;                     
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> rdist(0,1);
    if (rdist(gen)<1.05*difficulty){
        YoMamaActive=true;
        std::cout<<"Yomama is here!\n";
        if(rdist(gen)<0.5){//pionowo
            std::uniform_int_distribution<> dist(1,width-2);
            YoMamaVertical=true;
            YoMamaYoMama = new Entity(dist(gen),0,PINK);
        }else{
            std::uniform_int_distribution<> dist(1,height-2);
            YoMamaYoMama = new Entity(0,dist(gen),PINK);
        }
    }
}

void Grid::updateYoMama(){
    if(YoMamaVertical){
        int nextPosition = YoMamaYoMama->getY()+YoMamaDirection;
        if(nextPosition>=height || nextPosition<0) YoMamaDirection*=-1;
        else{
            if(YoMamaDirection==1) YoMamaYoMama->move(KEY_DOWN);
            else YoMamaYoMama -> move(KEY_UP);
        }
    }else{
        int nextPosition = YoMamaYoMama->getX()+YoMamaDirection;
        if(nextPosition>=width || nextPosition<0) YoMamaDirection*=-1;
        else{
            if(YoMamaDirection==1) YoMamaYoMama->move(KEY_RIGHT);
            else YoMamaYoMama -> move(KEY_LEFT);
        }
    }
}