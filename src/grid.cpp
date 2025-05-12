#include "grid.h"
#include <raylib.h>
#include <random>
#include "entity.h"
#include <iostream>
#include <math.h>

double yomamaSpawnChance = 0.5;
int ratRandomnessLevel=25;



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
    waterCycleOutline.y = std::max(startY-cell_width*2,GetScreenHeight()/4);
    waterCycleOutline.width = cell_width*width;
    waterCycleOutline.height = cell_width;

    //spawn wody
    spawnWater();
    //spawn yomama
    spawnYoMama();
    //spawn scian nosnych
    spawnSupportingColumns();
    //spawn studzienek
    spawnDrains();
    //spawn szczurow
    spawnHarolds();

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
            }else if(grid[i][j]==4){
                color = BROWN;
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
    for(Drain& d:drains) d.Draw(cell_width,startX,startY);
    player.Draw(cell_width,startX,startY);
    for(Entity& harold:harolds) harold.Draw(cell_width,startX,startY);
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
        if(player.getX()<width-1 && canEntityMoveTo(player.getY(),player.getX()+1)) player.move(KEY_RIGHT);
        break;
    case KEY_LEFT:
        if(player.getX()>0 && canEntityMoveTo(player.getY(),player.getX()-1)) player.move(KEY_LEFT);
        break;
    case KEY_DOWN:
        if(player.getY()<height-1 && canEntityMoveTo(player.getY()+1,player.getX())) player.move(KEY_DOWN);
        break;
    case KEY_UP:
        if(player.getY()>0 && canEntityMoveTo(player.getY()-1,player.getX())) player.move(KEY_UP);
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
        // updateEntites();
        flipClockwise();
        // entityUpdate = false;
        break;    
    case KEY_Q:
        // updateEntites();
        flipCounterClockwise();
        // entityUpdate = false;
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
        for (int j = -1; j <= 1; ++j){
            if(grid[turn_y + j][turn_x + i]==4) return;
            temp[i+1][j+1] = grid[turn_y + j][turn_x + i];
        }
            

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            grid[turn_y - 1 + i][turn_x + 1 - j] = temp[i][j];

    //przesuniecie entities
    flipEntity(&player,true);

    for(Entity& harold:harolds) flipEntity(&harold,true);
}

void Grid::flipCounterClockwise(){
    int temp[3][3];

    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
        {
            if(grid[turn_y + j][turn_x + i]==4) return;
            temp[i+1][j+1] = grid[turn_y + j][turn_x + i];
        }
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            grid[turn_y + 1 - i][turn_x - 1 + j] = temp[i][j];

    //przesuniecie entities
    flipEntity(&player,false);
    for(Entity& harold:harolds) flipEntity(&harold,false);
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
    if(YoMamaActive)
        if(player.getX()==YoMamaYoMama->getX() && player.getY()==YoMamaYoMama->getY()) return true;
    for(Entity harold:harolds) 
        if(player.getX()==harold.getX() && player.getY()==harold.getY())  return true;

    return false;
}


void Grid::updateEntites(){
    turn++;
    //update wody
    if(turn%waterCycleLength==0)
    {
        updateWater();
        updateDrains();
    }
        
    if(YoMamaActive)
        updateYoMama();
    updateHarolds();

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
    if (rdist(gen)<yomamaSpawnChance*difficulty){
        YoMamaActive=true;
        // std::cout<<"Yomama is here!\n";
        if(rdist(gen)<0.5){//pionowo
            std::uniform_int_distribution<> dist(1,width-2);
            YoMamaVertical=true;
            YoMamaYoMama = new Entity(dist(gen),0,YOMAMACOL);
        }else{
            std::uniform_int_distribution<> dist(1,height-2);
            YoMamaYoMama = new Entity(0,dist(gen),YOMAMACOL);
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

bool Grid::inBounds(int y,int x){
    return x>=0 && x<width && y>=0 && y<height;
}

bool Grid::canEntityMoveTo(int y,int x){
    if(!inBounds(y,x)) return false;
    return grid[y][x]!=1 && grid[y][x]!=4 ;
}

void Grid::spawnSupportingColumns(){
    std::random_device rd;                     
    std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> dist((int)difficulty, 5+(int)difficulty);
    std::uniform_int_distribution<> hdist(height/2-4,height/2+4);
    std::uniform_int_distribution<> wdist(width/2-5,width/2+5);

    int columCount = dist(gen);

    for(int i=0;i<columCount;i++){
        int h = hdist(gen);
        int w = wdist(gen);
        grid[h][w]=4;
    }

}


void Grid::spawnDrains(){
    std::random_device rd;                     
    std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> dist(0, (int)(10/difficulty));
    std::uniform_int_distribution<> distX(0, width-1);
    std::uniform_int_distribution<> distY(0, height-1);

    int drainCount = dist(gen);

    for(int i=0;i<drainCount;i++){
        int h = distX(gen);
        int w = distY(gen);

        Drain d(h,w,(Color){100,100,100,200});
        drains.push_back(d);
    }
}


void Grid::updateDrains(){
    for(Drain& d:drains){
        int x = d.getX();
        int y = d.getY();
        if(grid[y][x]==2){
            //usuwamy wode w promieniu 5x5
            for(int i =-2;i<3;i++){
                for(int j=-2;j<3;j++){
                    if(canEntityMoveTo(y+i,x+j) && grid[y+i][x+j]==2) grid[y+i][x+j]=0;
                }
            }
        }
    }
}

void Grid::spawnHarolds(){
    std::random_device rd;                     
    std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> dist(0, 3);
    std::uniform_int_distribution<> distX(0, width-1);
    std::uniform_int_distribution<> distY(0, height-1);

    int haroldCount = dist(gen);

    while(haroldCount>0){
        int ratX,ratY;
        do
        {
            ratX=distX(gen);
            ratY=distY(gen);
        } while (!(canEntityMoveTo(ratY,ratX) && (ratX!=0 || ratY!=0) && (ratX!=width-1 || ratY!=height-1)));
        Entity harold(ratX,ratY,RED);
        harolds.push_back(harold);
        haroldCount--;
    }
}


void Grid::updateHarolds(){

    std::random_device rd;                     
    std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> dist(0, ratRandomnessLevel);

    for(Entity& harold:harolds){
        switch (dist(gen))
        {
            //losowy ruch
        case 1:
            if(canEntityMoveTo(harold.getY(),harold.getX()+1)) harold.move(KEY_RIGHT);
            else burrow(&harold,KEY_RIGHT);
            break;
        case 2:
            if(canEntityMoveTo(harold.getY(),harold.getX()-1)) harold.move(KEY_LEFT);
            else burrow(&harold,KEY_LEFT);
            break;
        case 3:
            if(canEntityMoveTo(harold.getY()+1,harold.getX())) harold.move(KEY_DOWN);
            else burrow(&harold,KEY_DOWN);
            break;
        case 4:
            if(canEntityMoveTo(harold.getY()-1,harold.getX())) harold.move(KEY_UP);
            else burrow(&harold,KEY_UP);
            break;
        case 5:{//ruch w strone gracza
            int dx = player.getX()-harold.getX();
            int dy = player.getY()-harold.getY();

            if(abs(dx)>abs(dy)){//ruch poziomy
                if(dx>0){
                    if(canEntityMoveTo(harold.getY(),harold.getX()+1)) harold.move(KEY_RIGHT);
                    else burrow(&harold,KEY_RIGHT);
                }else{
                    if(canEntityMoveTo(harold.getY(),harold.getX()-1)) harold.move(KEY_LEFT);
                    else burrow(&harold,KEY_LEFT);
                }
            }else{
                if(dy>0){
                    if(canEntityMoveTo(harold.getY()+1,harold.getX())) harold.move(KEY_DOWN);
                    else burrow(&harold,KEY_DOWN);
                }else{
                    if(canEntityMoveTo(harold.getY()-1,harold.getX())) harold.move(KEY_UP);
                    else burrow(&harold,KEY_UP);
                }
            }
            break;
            }
        default:
            break;
        }

    }

}

void Grid::burrow(Entity* e,int dir){
    int xdir,ydir;
    switch (dir)
    {
    case KEY_LEFT:
        xdir=-1;
        ydir=0;
        break;
    case KEY_RIGHT:
        xdir=1;
        ydir=0;
        break;
    case KEY_UP:
        xdir=0;
        ydir=-1;
        break;
    case KEY_DOWN:
        xdir=0;
        ydir=1;
        break;                    
    default:
        break;
    }


    int x = e->getX();
    int y  = e->getY();

    do
    {
        x+=xdir;
        y+=ydir;
    } while (inBounds(y,x) && (grid[y][x]==1 || grid[y][x]));

    if(inBounds(y,x)) e->setPos(x,y);   

}