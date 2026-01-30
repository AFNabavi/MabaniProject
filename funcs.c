#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "funcs.h"

const float WallTh = 3;   // Thick of walls
const int FadeCo = 7;
const int FPS = 60;
const int WindowWidth = 1100;
const int WindowHeight = 650;
const int WidthHintBox = 200;   // This is for width of Hint Box
const int MarginSpace = 30;    // This is for distance of top and bottom of map and used in the Side
const int Side = (WindowHeight - 2*MarginSpace)/12;     // Side length of every squre in map
int map[25][25];    // Max size for map  

Vector2 Lightcore = {0.0f};

int nExplorers = 0;
Explorer Explorers[3];
// Vector2 Explorers[3] = {0.0f};
// Vector2 ExplorersP[3] = {0.0f};     // Position in Window
// char ExplorersDir[3] = {'\0'};
// int nInterimWalls[3] = {2, 2, 2};
InterimWalls InWalls[30];
int nInWalls = 0; 

int nShadowCasters;
Vector2 ShadowCasters[3] = {0.0f};
Vector2 ShadowCastersP[3] = {0.0f};     // Position in Window
int ShadowCastersDir[3] = {-1, -1, -1};
int FadeSh[3];
int swF[3] = {1, 1, 1};

int minlength;
char FirstMove;
char SecondMove;

void SET_Map_Array(int M[][25], int m, int n)
{
/*
Initializes the expanded map grid.
Inner cells (non-border) are set to 1.
Border cells are set to -1.
Grid size: (2*n + 1) × (2*m + 1).
*/
    int i, j;
    for (i=0; i<2*n+1; i++)
    {
        for (j=0; j<2*m+1; j++)
        {
            if ((i>0 && i<2*n) && (j>0 && j<2*m)) M[j][i] = 1;
            else M[j][i] = -1;
        }
    }
    // map[1][1] = 0;
}

Vector2 GET_StartPoint(int m, int n, int WidthSpace)
{
/*
Calculates the top-left starting position for centering the map on screen.
MapSize is derived from (m, n) and tile size.
Horizontal Space (WidthSpace) is subtracted before centering.
*/
    Vector2 MapSize, StartPoint;
    MapSize.x = (float)n*Side;
    MapSize.y = (float)m*Side;
    StartPoint.x = ((WindowWidth - WidthSpace) - MapSize.x)/2.0;
    StartPoint.y = ((WindowHeight) - MapSize.y)/2.0;
    return StartPoint;
}

Vector2 SET_Walls(WallPro Wall)
{
/*
Marks the corresponding map cell for a wall and updates its position.
For horizontal walls (H/h), the cleared cell is at (2y+2, 2x+1).
For vertical walls, the cleared cell is at (2y+1, 2x+2).
*/
    if(Wall.HorV == 'H' || Wall.HorV == 'h')
    {
        Vector2 Coordinate;
        int j = 2*Wall.Position.y + 2;
        int i = 2*Wall.Position.x + 1;
        map[j][i] = 0;
        Coordinate.x = i;
        Coordinate.y = j;
        return Coordinate;
    }
    else
    {
        Vector2 Coordinate;
        int j = 2*Wall.Position.y + 1;
        int i = 2*Wall.Position.x + 2;
        map[j][i] = 0;
        Coordinate.x = i;
        Coordinate.y = j;
        return Coordinate;
    }
}

Vector2 GET_Start_Walls_Position_for_Draw(Vector2 StartPoint, WallPro Wall)
{
/*
Computes the screen draw position of a wall based on its map coordinates.
For horizontal walls:
    Uses (y/2)*Side and ((x-1)/2)*Side offsets.
For vertical walls:
    Uses ((y-1)/2)*Side and (x/2)*Side offsets.
Returned Vector2 is the top-left draw point.
*/
    Vector2 S;
    if(Wall.HorV == 'H' || Wall.HorV == 'h')
    {
        S.y = StartPoint.y + ((Wall.Position.y)/2.0)*Side;
        S.x = StartPoint.x + ((Wall.Position.x - 1)/2.0)*Side;
        return S;
    }
    else 
    {
        S.y = StartPoint.y + ((Wall.Position.y - 1)/2.0)*Side;
        S.x = StartPoint.x + ((Wall.Position.x)/2.0)*Side;
        return S;
    }
}

Vector2 Return_Elements_Position(Vector2 Element)
{
/*
Converts a logical element position to its corresponding map-grid index.
Each element maps to (2x+1, 2y+1) in the expanded grid.
*/
    float i = 2*Element.x + 1.0;
    float j = 2*Element.y + 1.0;
    Vector2 Result; Result.x = i; Result.y = j;
    return Result;
}

Vector2 GET_Start_Elements_Position_for_Draw(Vector2 StartPoint, Vector2 Element)
{
/*
Converts a map-grid coordinate to the on-screen center of its tile.
Screen position = StartPoint + (Element * Side/2).
*/
        float i = StartPoint.x + (Element.x-1.0f)*Side/2 + 2.0f;
        float j = StartPoint.y + (Element.y-1.0f)*Side/2 + 3.0f;
        Vector2 Position = {i, j};
        return Position;
}

int Direction_of_Explorers(Vector2 Explorer)
{
/*
Determines explorer's facing direction:
1 (right) if Lightcore x >= explorer x,
else -1 (left).
*/
    if (Lightcore.x>=Explorer.x) return 1;
    else return -1;
}

int Direction_of_ShadowCasters(Vector2 ShadowCaster)
{
/*
Determines shadow caster's facing direction: 
1 (right) if nearest explorer's x >= shadow caster's x, 
else -1 (left). Nearest by Manhattan distance.
*/
    int Distance;
    int MinDistance = 500, NearExplorer;
    for (int i=0; i<nExplorers; i++)
    {
        if (ShadowCaster.x>Explorers[i].mapPos.x)
        {
            if (ShadowCaster.y>Explorers[i].mapPos.y) Distance = (ShadowCaster.x - Explorers[i].mapPos.x) + (ShadowCaster.y - Explorers[i].mapPos.y);
            else Distance = (ShadowCaster.x - Explorers[i].mapPos.x) + (Explorers[i].mapPos.y - ShadowCaster.y);
        }
        else 
        {
            if (ShadowCaster.y>Explorers[i].mapPos.y) Distance = (Explorers[i].mapPos.x - ShadowCaster.x) + (ShadowCaster.y - Explorers[i].mapPos.y);
            else Distance = (Explorers[i].mapPos.x - ShadowCaster.x) + (Explorers[i].mapPos.y - ShadowCaster.y);
        }
        if (Distance<MinDistance) 
        {
            MinDistance = Distance;
            NearExplorer = i;
        }
    }
    if (Explorers[NearExplorer].mapPos.x>=ShadowCaster.x) return 1;
    else return -1;
}

int Check_Elements(Vector2 E, int numberEx, int numberSh)
{
/*
Checks if position E overlaps with lightcore or existing explorers/shadow casters. 
Returns 1 if unique, 0 otherwise.
*/
    int i;
    if ((E.x == Lightcore.x) && (E.y == Lightcore.y)) return 0;     // Element is in lightcore
    for (i=0; i<numberEx; i++)
    {
        if ((E.x == Explorers[i].mapPos.x) && (E.y == Explorers[i].mapPos.y)) return 0;    // Element is in explorer
    }
    for (i=0; i<numberSh; i++)
    {
        if ((E.x == ShadowCasters[i].x) && (E.y == ShadowCasters[i].y)) return 0;   // Element is in shadow caster
    }
    return 1;
}

int Check_Walls(WallPro W)
{
/*
Checks if wall can be placed: 
computes map indices based on position and orientation (H/V), 
returns 1 if map[j][i] == 1 (valid), 
else 0.
*/
    int i, j;
    if (W.HorV == 'H' || W.HorV == 'h')
        {
            j = 2*W.Position.y + 2;
            i = 2*W.Position.x + 1;
        }
        else
        {
            j = 2*W.Position.y + 1;
            i = 2*W.Position.x + 2;
        }
    if (map[j][i] == 1) return 1;
    else return 0;
}

void Draw_Map(Vector2 StartPoint, int m, int n, int Round, int ExRound)
{
/*
Renders the entire game map including walls, lightcore, explorers, and shadow casters.

Map grid structure (internal representation in 'map' array):
- Horizontal walls: checked at map[j][i] where j even (0..2*m), i odd (1..2*n-1)
- Vertical walls: checked at map[j][i] where i even (0..2*n), j odd (1..2*m-1)

Wall color and thickness rules based on map[j][i] value:
    1   → default/open path: thin semi-transparent black line (preview/guide)
    0   → active wall (user-placed): thick red line
   -1   → map border (fixed outer walls): thick solid black line
   other (e.g., 2+) → wall placed by explorer: thick orange line
*/
    int i, j;
    WallPro W;
    Vector2 StartP, EndP;
    Color O = {255, 161, 0, 255};     // Orange - explorer-placed walls
    Color R = {230, 41, 55, 255};     // Red - user-placed walls
    Color B = {0, 0, 0, 255*(0.4f)};          // Black - Semi-transparent black - guide lines

    // Draw horizental walls
    for (j=0; j<2*m+1; j+=2)
    {
        for (i=1; i<2*n+1; i+=2)
        {
            W.Position.x = (float)i; W.Position.y = (float)j; W.HorV = 'H';
            StartP = GET_Start_Walls_Position_for_Draw(StartPoint, W);
            EndP.x = StartP.x + Side; EndP.y = StartP.y;
            if (map[j][i] == 1) DrawLineEx(StartP, EndP, 1.0f, B);
            else if (map[j][i] == 0) DrawLineEx(StartP, EndP, WallTh, R);
            else if(map[j][i] == -1) DrawLineEx(StartP, EndP, WallTh, BLACK);
            else DrawLineEx(StartP, EndP, WallTh, O);
        }
    }

// Draw vertical walls
    for (i=0 ; i<2*n+1; i+=2)
    {
        for (j=1 ; j<2*m+1; j+=2)
        {
            W.Position.x = (float)i; W.Position.y = (float)j; W.HorV = 'V';
            StartP = GET_Start_Walls_Position_for_Draw(StartPoint, W);
            EndP.x = StartP.x; EndP.y = StartP.y + Side;
            if (map[j][i] == 1) DrawLineEx(StartP, EndP, 1.0f, B);
            else if(map[j][i] == 0) DrawLineEx(StartP, EndP, WallTh, R);
            else if(map[j][i] == -1) DrawLineEx(StartP, EndP, WallTh, BLACK);
            else DrawLineEx(StartP, EndP, WallTh, O);
        }
    }

// Drawing lightcore
    Vector2 S = GET_Start_Elements_Position_for_Draw(StartPoint, Lightcore);
    DrawTexture(LiTexture, S.x, S.y, WHITE);

// Draw explorers (facing toward lightcore)
    for (i=0; i<nExplorers; i++) {
        if (Explorers[i].isAlive)
            if (!Explorers[i].direction) {
                int Direction = Direction_of_Explorers(Explorers[i].mapPos);
                if (Direction>0) {
                    if (i==0) DrawTexture(Ex1TextureRight, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==1) DrawTexture(Ex2TextureRight, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==2) DrawTexture(Ex3TextureRight, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);        
                } else {
                    if (i==0) DrawTexture(Ex1TextureLeft, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==1) DrawTexture(Ex2TextureLeft, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==2) DrawTexture(Ex3TextureLeft, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                }
            } else {
                if(Explorers[i].direction=='R') {
                    if (i==0) DrawTexture(Ex1TextureRight, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==1) DrawTexture(Ex2TextureRight, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==2) DrawTexture(Ex3TextureRight, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);        
                } else {
                    if (i==0) DrawTexture(Ex1TextureLeft, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==1) DrawTexture(Ex2TextureLeft, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                    if (i==2) DrawTexture(Ex3TextureLeft, Explorers[i].winPos.x, Explorers[i].winPos.y, WHITE);
                }
            }
    }

    
// Draw shadow casters (facing toward nearest explorer)
    for (i=0; i<nShadowCasters; i++) {
        Color ColorSh = {255, 255, 255, ((float)FadeSh[i]/(10.0f*FadeCo))*255};
        if (ShadowCastersDir[i] == -2 || ShadowCastersDir[i] == -3) {
            if (ShadowCastersDir[i] == -2) {
                if (i==0) DrawTexture(Sh1TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                if (i==1) DrawTexture(Sh2TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                if (i==2) DrawTexture(Sh3TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
            }
            if (ShadowCastersDir[i] == -3) {
                if (i==0) DrawTexture(Sh1TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                if (i==1) DrawTexture(Sh2TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);            
                if (i==2) DrawTexture(Sh3TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);
            }
        }
        else {
            if (ShadowCastersDir[i] != -1) {
                int Direction = Explorers[ShadowCastersDir[i]].mapPos.x - ShadowCasters[i].x;  
                if (Direction>=0) {
                    if (i==0) DrawTexture(Sh1TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                    if (i==1) DrawTexture(Sh2TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);            
                    if (i==2) DrawTexture(Sh3TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                } else {
                    if (i==0) DrawTexture(Sh1TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                    if (i==1) DrawTexture(Sh2TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                    if (i==2) DrawTexture(Sh3TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                }            
            } else {            
                int Direction = Direction_of_ShadowCasters(ShadowCasters[i]);
                if (Direction == 1) {
                    if (i==0) DrawTexture(Sh1TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                    if (i==1) DrawTexture(Sh2TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                    if (i==2) DrawTexture(Sh3TextureRight, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                } else {
                    if (i==0) DrawTexture(Sh1TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                    if (i==1) DrawTexture(Sh2TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                    if (i==2) DrawTexture(Sh3TextureLeft, ShadowCastersP[i].x, ShadowCastersP[i].y, ColorSh);                
                }
            }
        }
    }  
    Fade_ShadowCasters(); 

    Rectangle HintGame = {WindowWidth-WidthHintBox, MarginSpace, WidthHintBox-MarginSpace, (WindowHeight-2*MarginSpace)};
    Color MyRed = {205, 50, 0, 255};
    DrawText("Move up: 'W'\n", HintGame.x+22, HintGame.y+20, 20, MyRed);
    DrawText("Move down: 'S'\n", HintGame.x+14, HintGame.y+45, 20, MyRed);
    DrawText("Move right: 'D'\n", HintGame.x+12, HintGame.y+70, 20, MyRed);
    DrawText("Move left: 'A'\n", HintGame.x+18, HintGame.y+95, 20, MyRed);
    DrawText("Place wall: 'E'\n", HintGame.x+17, HintGame.y+120, 20, MyRed);
    DrawText("Skip round: 'Q'\n", HintGame.x+11, HintGame.y+145, 20, MyRed);
    DrawText("-----------------", HintGame.x+1, HintGame.y+175, 20, MyRed);
    DrawRectangleRoundedLinesEx(HintGame, 0.1f, 20, 1.0f, MyRed);

    DrawText("-----------------", 901, 490, 20, MyRed);
    char s[3];
    ItoS(s, Round);
    DrawText("Round: ", 945, 580, 20, MyRed);
    DrawText(s, 1018, 580, 20, MyRed);
    ItoS(s, ExRound+1);
    DrawText("Player: ", 942, 520, 20, MyRed);
    DrawText(s, 1020, 520, 20, MyRed);
    ItoS(s, Explorers[ExRound].wallCount);
    DrawText("Wall count: ", 922, 550, 20, MyRed);
    if (Explorers[ExRound].wallCount == 0) DrawText("0", 1041, 550, 20, MyRed);
    else DrawText(s, 1041, 550, 20, MyRed);
}

int Distance_Check(Vector2 v1, Vector2 v2, Explorer arr1[], int arr1c, Vector2 arr2[], int arr2c)
{
/*
Computes distance between v1 and v2 and return 1 if it is valid otherwise, 0.
*/
    float dx, dy;

    // check v2
    dx = 2*v1.x+1.0f - v2.x;
    dy = 2*v1.y+1.0f - v2.y;
    if (dx*dx + dy*dy < 16.0f) return 0;

    // check arr1
    for (int i = 0; i < arr1c; i++)
    {
        dx = 2*v1.x+1.0f - arr1[i].mapPos.x;
        dy = 2*v1.y+1.0f - arr1[i].mapPos.y;
        if (dx*dx + dy*dy < 16.0f) return 0;
    }

    // check arr2
    for (int i = 0; i < arr2c; i++)
    {
        dx = 2*v1.x+1.0f - arr2[i].x;
        dy = 2*v1.y+1.0f - arr2[i].y;
        if (dx*dx + dy*dy < 16.0f) return 0;
    }

    return 1;
}

WallPro Put_Wall(int m, int n)
{
/*
Generates a random wall within map bounds.
Randomly selects a cell (x, y) and wall orientation.
*/
    int x, y, HorV;
    x = rand()%n;
    y = rand()%m;
    HorV = rand()%2; // 0=horizental  and  1=vertical
    if (HorV==0) HorV = 'H';
    else HorV = 'V';
    
    WallPro w;
    w.Position.x = (float)x;
    w.Position.y = (float)y;
    w.HorV = HorV;

    return w;
}

void BFS_Check(char sw, int BlocksA[][2], int ACount, int BlocksB[][2], int BCount, int *Checked) 
{
/*
Performs a BFS-based connectivity check on the map grid.
Expands reachable cells alternately using two block lists (A and B),
marking visited cells and counting total reachable blocks via `Checked`.
*/

// m and n must be declared
    if (sw=='A' && ACount==0) return;
    if (sw=='B' && BCount==0) return;
    if (sw=='A') {
        int k;
        int i, j;
        for(k=0; k<ACount; k++) 
        {
            int j = BlocksA[k][0];
            int i = BlocksA[k][1];
            if(map[j-1][i]==1 && map[j-2][i]==1) 
            {
                map[j-2][i] = 0;
                (*Checked)++;
                BlocksB[BCount][0] = j-2;
                BlocksB[BCount][1] = i;
                BCount++;
            }
            if(map[j][i+1]==1 && map[j][i+2]==1) 
            {
                map[j][i+2] = 0;
                (*Checked)++;
                BlocksB[BCount][0] = j;
                BlocksB[BCount][1] = i+2;
                BCount++;
            }
            if(map[j+1][i]==1 && map[j+2][i]==1) 
            {
                map[j+2][i] = 0;
                (*Checked)++;
                BlocksB[BCount][0] = j+2;
                BlocksB[BCount][1] = i;
                BCount++;
            }
            if(map[j][i-1]==1 && map[j][i-2]==1) 
            {
                map[j][i-2] = 0;
                (*Checked)++;
                BlocksB[BCount][0] = j;
                BlocksB[BCount][1] = i-2;
                BCount++;
            }
        }
        ACount = 0;
        BFS_Check('B', BlocksA, ACount, BlocksB, BCount, Checked);
    } else {
        int k;
        int i, j;
        for(k=0; k<BCount; k++) 
        {
            j = BlocksB[k][0];
            i = BlocksB[k][1];
            if(map[j-1][i]==1 && map[j-2][i]==1) 
            {
                map[j-2][i] = 0;
                (*Checked)++;
                BlocksA[ACount][0] = j-2;
                BlocksA[ACount][1] = i;
                ACount++;
            }
            if(map[j][i+1]==1 && map[j][i+2]==1) 
            {
                map[j][i+2] = 0;
                (*Checked)++;
                BlocksA[ACount][0] = j;
                BlocksA[ACount][1] = i+2;
                ACount++;
            }
            if(map[j+1][i]==1 && map[j+2][i]==1) 
            {
                map[j+2][i] = 0;
                (*Checked)++;
                BlocksA[ACount][0] = j+2;
                BlocksA[ACount][1] = i;
                ACount++;
            }
            if(map[j][i-1]==1 && map[j][i-2]==1) 
            {
                map[j][i-2] = 0;
                (*Checked)++;
                BlocksA[ACount][0] = j;
                BlocksA[ACount][1] = i-2;
                ACount++;
            }
        }
        BCount = 0;
        BFS_Check('A', BlocksA, ACount, BlocksB, BCount, Checked);
    }

}

void Reset_Map_Blocks_for_BFS(int m, int n)
{
/*
Resets all logical map blocks to unvisited state.
Sets inner block cells to 1 and marks the start cell (1,1) as visited.
*/
    int i, j;
    for (j=1; j<2*m+1; j+=2)
    {
        for (i=1; i<2*n+1; i+=2)
        {
            map[j][i] = 1;
        }
    }
    map[1][1] = 0;
}

void Reset_Map_Blocks_for_Move_Elements(int m, int n) {
    int i, j;
    for (j=1; j<2*m+1; j+=2) {
        for (i=1; i<2*n+1; i+=2) {
            map[j][i] = 1;
        }
    }
    map[(int)Lightcore.y][(int)Lightcore.x] = 3;
    for (i=0; i<nShadowCasters; i++) {
        map[(int)ShadowCasters[i].y][(int)ShadowCasters[i].x] = -1;
    }
    for (i=0; i<nExplorers; i++) {
        if (Explorers[i].isAlive) {
        map[(int)Explorers[i].mapPos.y][(int)Explorers[i].mapPos.x] = 2;
        for (int j=0; j<nShadowCasters; j++) ShadowCastersDir[j] = -1;
        }
    }
}

void Initializing_FadeSh()
{
/*
Initializes fade intensity values for shadow casters.
Each caster receives an increasing fade offset based on FadeCo.
*/
    int i, j;
    i=FadeCo*2;
    for (j=0; j<nShadowCasters; j++)
    {
        if (i>(FadeCo*10)) i = FadeCo*3;
        FadeSh[j] = i; 
        i += (FadeCo*3);
    }
}

void Fade_ShadowCasters()
{
/*
Updates fade values of shadow casters to create a pulsating effect.
Fade direction switches at predefined min and max thresholds.
*/
    int j;
    for (j=0; j<nShadowCasters; j++)
    {
        if (FadeSh[j]==FadeCo*10) swF[j] = -1;
        if (FadeSh[j]==FadeCo*3) swF[j] = 1;
        if (swF[j]==1) FadeSh[j]++;
        else FadeSh[j]--;
    }
}

Vector2 Move_Element(Vector2 E, char Dir)
{
/*
draws an element when it is moving with a constant speed.
*/
    Vector2 v = E;
    if (Dir == 'D') {
        v.x += 2.0f;
    }
    else if (Dir == 'W') {
        v.y -= 2.0f;
    }
    else if (Dir == 'A') {
        v.x -= 2.0f;
    }
    else if (Dir == 'S') {
        v.y += 2.0f;
    }
    return v;
}

int Can_Ex_Move_for_Walls(Vector2 E, char Dir)
{
    int x = E.x, y = E.y;
    if (Dir == 'W') {
        if (map[y-1][x] == 1) return 1;
    }
    else if (Dir == 'S') {
        if (map[y+1][x] == 1) return 1;
    }
    else if (Dir == 'A') {
        if (map[y][x-1] == 1) return 1;
    }
    else if (Dir == 'D') {
        if (map[y][x+1] == 1) return 1;
    }
    return  0;
}

int Win_or_Lose(Vector2 E, Vector2 Sh[], int nSh, Vector2 L)
{
    int Exx = E.x, Exy = E.y, Lx = L.x, Ly = L.y;
    int Shx, Shy;
    for (int i=0; i<nSh; i++)
    {
        Shx = Sh[i].x; Shy = Sh[i].y;
        if (Exx == Shx  &&  Exy == Shy) return 0;
    }
    if (Exx == Lx  &&  Exy == Ly) return 1;
    return -1;
}

void Draw_Map_Infs()
{
    const int VerMargin=220, HorMargin=225, RecHeight=100, RecWidth=130, Distance=30;
    int n=5, m=9;
    char nn[3], mm[3];
    for (int i=0; i<4; i++)
    {
        DrawRectangleGradientV(HorMargin+i*(Distance+RecWidth), VerMargin, RecWidth, RecHeight, GOLD, YELLOW);
        itoa(n+i, nn, 10);
        DrawText(nn, HorMargin+i*(Distance+RecWidth)+60, VerMargin+35, 30, BLACK);
        DrawRectangleGradientH(HorMargin+i*(Distance+RecWidth), (VerMargin+Distance+RecHeight), RecWidth, RecHeight, YELLOW, GOLD);
        itoa(m+i, mm, 10);
        DrawText(mm, HorMargin+i*(Distance+RecWidth)+60, VerMargin+Distance+RecHeight+35, 30, BLACK);
    }
    
}

int Get_Map_Infs()
{
    const int VerMargin=220, HorMargin=225, RecHeight=100, RecWidth=130, Distance=30;

    Vector2 MousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        for (int i=0; i<4; i++)
        {
        if (MousePos.y >= VerMargin && MousePos.y <= (VerMargin+RecHeight)) 
            if (MousePos.x >= (HorMargin+i*(Distance+RecWidth)) && MousePos.x <= (HorMargin+i*(Distance+RecWidth)+RecWidth)) return (i+5);

        if (MousePos.y >= VerMargin+Distance+RecHeight && MousePos.y <= VerMargin+Distance+RecHeight+RecHeight)
            if (MousePos.x >= (HorMargin+i*(Distance+RecWidth)) && MousePos.x <= (HorMargin+i*(Distance+RecWidth)+RecWidth)) return (i+9);
        }  
    return -1;
}

void ItoS(char *str, int n) {
    char *start;
    start = str;
    for (int i=0; n>0; i++) {
        *str = (n%10) + '0';
        str ++;
        n /= 10;
    }
    *str = '\0';
    str --;
    for(; start<=str; str--, start++) {
        char temp = *str;
        *str = *start;
        *start = temp;
    }
}

int StoI(char s[], int sLen) {
    int InpNumber = 0;
    for (int i=0; i<sLen; i++) {InpNumber += (s[i] - '0'); InpNumber*=10;} InpNumber /= 10;
    return InpNumber;
}

void Draw_Walls_Infs(char s[], int n, int m)
{
    Rectangle OutRecLines = {200, 150, 700, 350};
    Rectangle InpShower = {460, 350, 85, 50};
    Rectangle SubmitButton = {550, 350, 85, 50};
    Color color = {195, 226, 245, 255};
    int nm = (n-1)*(m-1), i, j, k;
    char str1[5] = {'\0'};
    ItoS(str1, nm);
    char str0[] = "Enter the number of walls\nyou want (between 0 and ";
    char str2[] = "):\n";
    char str[70];
    for (i=0; str0[i]; i++) str[i] = str0[i]; 
    for (j=0; str1[j]; j++) str[i+j] = str1[j];
    for (k=0; str2[k]; k++) str[i+j+k] = str2[k];
    str[i+j+k] = '\0';

    DrawRectangleRounded(OutRecLines, 0.3, 2.0, color);
    DrawRectangleRoundedLines(InpShower, 0.5, 4.0, GRAY);
    DrawRectangleRoundedLines(SubmitButton, 0.5, 4.0, GRAY);    
    DrawText(str, OutRecLines.x+105, OutRecLines.y+50, 30, RED);
    DrawText("(press R to remove your input.)", OutRecLines.x+155, OutRecLines.y+120, 20, RED);
    DrawText(s, InpShower.x+25, InpShower.y+15, 20, BLACK);
    DrawText("SUBMIT", SubmitButton.x+8, SubmitButton.y+15, 19, DARKGRAY);
}

char Print_Number_In_String(char s[], char ch, int len)
{
    if (len<4)
    {
        s[len] = ch;
        s[len+1] = '\0';
    }
    if (ch == 'r' || ch == 'R')
    {
        s[len-1] = '\0';
    }
    return '\0';
}

int Submit_Button(int n, int m, char inp[], int inpLen)
{   
    Rectangle SubmitButton = {550, 350, 85, 50};
    Vector2 MousePos = GetMousePosition();
    int InpNumber = StoI(inp, inpLen);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        if (MousePos.x >= SubmitButton.x &&
             MousePos.x <= (SubmitButton.x+SubmitButton.width) &&
             MousePos.y >= SubmitButton.y &&
             MousePos.y <= (SubmitButton.y+SubmitButton.height)) 
                if (InpNumber>0 && InpNumber<=((n-1)*(m-1)))
                    return 1;
    return 0;
}

int BFS_Way(Vector *start, Vector *end, int *ACount, int *resCount, Vector *result) {    
    Vector *startcpy;
    Vector *endcpy;
    startcpy = start;
    endcpy = end;
    int k;
    int i=0;
    for (k=0; k<=(end-start); k++) {
        
        Vector temp;
        if (map[(*(start+k)).y-1][(*(start+k)).x] == 1 && !(map[(*(start+k)).y-2][(*(start+k)).x] == 0)) {            
            (*ACount)++;
           temp.beg = 'D'; temp.y = (*(start+k)).y-2; temp.x = (*(start+k)).x;
           if (map[temp.y][temp.x] == 2) {
            *result = temp;
            (*resCount)++;
            int l;
            for (l=0; l<nExplorers; l++) {
                if(temp.x == (int)Explorers[l].mapPos.x && temp.y == (int)Explorers[l].mapPos.y) {
                    return l;
                }
            }
           }
           map[(*(start+k)).y-2][(*(start+k)).x] = 0;
           i++;
           *(end+i) = temp;
        }
        if (map[(*(start+k)).y][(*(start+k)).x+1] == 1 && !(map[(*(start+k)).y][(*(start+k)).x+2] == 0)) {
            (*ACount)++;
           temp.beg = 'L'; temp.y = (*(start+k)).y; temp.x = (*(start+k)).x+2;
           if (map[temp.y][temp.x] == 2) {
            *result = temp;
            (*resCount)++;
            int l;
            for (l=0; l<nExplorers; l++) {
                if(temp.x == (int)Explorers[l].mapPos.x && temp.y == (int)Explorers[l].mapPos.y) {
                    return l;
                }
            }
           }
           map[(*(start+k)).y][(*(start+k)).x+2] = 0;
           i++;
           *(end+i) = temp;
        }
        if (map[(*(start+k)).y+1][(*(start+k)).x] == 1 && !(map[(*(start+k)).y+2][(*(start+k)).x] == 0)) {
            (*ACount)++;
           temp.beg = 'U'; temp.y = (*(start+k)).y+2; temp.x = (*(start+k)).x;
           if (map[temp.y][temp.x] == 2) {
            *result = temp;
            (*resCount)++;
            int l;
            for (l=0; l<nExplorers; l++) {
                if(temp.x == (int)Explorers[l].mapPos.x && temp.y == (int)Explorers[l].mapPos.y) {
                    return l;
                }
            }
           }
           map[(*(start+k)).y+2][(*(start+k)).x] = 0;
           i++;
           *(end+i) = temp;
        }
        if (map[(*(start+k)).y][(*(start+k)).x-1] == 1 && !(map[(*(start+k)).y][(*(start+k)).x-2] == 0)) {
            (*ACount)++;
           temp.beg = 'R'; temp.y = (*(start+k)).y; temp.x = (*(start+k)).x-2;
           if (map[temp.y][temp.x] == 2) {
            *result = temp;
            (*resCount)++;
            int l;
            for (l=0; l<nExplorers; l++) {
                if(temp.x == (int)Explorers[l].mapPos.x && temp.y == (int)Explorers[l].mapPos.y) {
                    return l;
                }
            }
           }
           map[(*(start+k)).y][(*(start+k)).x-2] = 0;
           i++;
           *(end+i) = temp;
        }
    }
    if (!i) return -1;
    startcpy = end+1;
    endcpy = end+i;
    int R = BFS_Way(startcpy, endcpy, ACount, resCount, result);
    return R;
    
}

int Find_Way(Vector *end, const int ACount, int resCount, Vector *Alist, Vector *result) {
    if ((*end).beg == '\0') return resCount;
    Vector temp;
    if ((*end).beg == 'U') {
        temp.y = (*end).y-2;
        temp.x = (*end).x;
    }
    if ((*end).beg == 'R') {
        temp.y = (*end).y;
        temp.x = (*end).x+2;
    }
    if ((*end).beg == 'L') {
        temp.y = (*end).y;
        temp.x = (*end).x-2;
    }
    if ((*end).beg == 'D') {
        temp.y = (*end).y+2;
        temp.x = (*end).x;
    }
    int i=0;
    while (i<ACount) {
        if ((*(Alist+i)).y == temp.y && (*(Alist+i)).x == temp.x) {
            temp.beg = (*(Alist+i)).beg;
            break;
        }
        i++;
    }
    *(result+resCount) = temp;
    resCount++;

    return Find_Way(&temp, ACount, resCount, Alist, result);
}

void Draw_Way(Vector *Way, Vector2 StartPoint, const int resCount) {
    Vector *A;
    Vector2 S, E, temp1, temp2;
    Color Magneta = { 255, 0, 255, (0.7f)*255 };
    Color MagnetaFade = { 255, 0, 255, (0.3f)*255 };
    for (A=Way; A<Way+resCount-1; A++) {
        temp1.x = (float)(*A).x; temp1.y = (float)(*A).y;
        temp2.x = (float)(*(A+1)).x; temp2.y = (float)(*(A+1)).y;  
        S = GET_Start_Elements_Position_for_Draw(StartPoint, temp1); 
        E = GET_Start_Elements_Position_for_Draw(StartPoint, temp2);
        S.x += (Side/2); S.y += (Side/2);
        E.x += (Side/2); E.y += (Side/2);
        if ((*A).x != (*(Way+resCount-1)).x && (*A).y != (*(Way+resCount-1)).y) {
            DrawLineEx(S, E, 2.0f, Magneta);
        } else {
            DrawLineEx(S, E, 2.0f, MagnetaFade);
        }
    }
}// not used

int Calculate_Max_Interim_Wall(int m, int n) {
    // result = max(min(m, n)/3 , 1)
    int temp; 
    if (m>n) temp = n;
    else temp = m;
    temp /= 3;
    if (temp > 1) return temp;
    else return 1;
}

SidesAR CheckSides(int j, int i) {
    SidesAR A = {'\0'};
    if (map[j-1][i] == 1) {
        A.U = 'O';
    }
    if (map[j][i+1] == 1) {
        A.R = 'O';
    }
    if (map[j+1][i] == 1) {
        A.D = 'O';
    }
    if (map[j][i-1] == 1) {
        A.L = 'O';
    }
    return A;
}

void Rec_for_Choose(float x, float y, SidesAR A, Rectangle R[]) {
    float RSpace = 0;
    int coef = 0;
    float RSide = 30;
    if (A.U == 'O') {
        Rectangle Z = {x+RSpace+coef*(RSide), y, RSide, RSide};
        R[0] = Z;
        RSpace = RSpace + 10.0;
        coef++;
    } else R[0].x = 1.0;
    if (A.R == 'O') {
        Rectangle Z = {x+RSpace+coef*(RSide), y, RSide, RSide};
        R[1] = Z;
        RSpace = RSpace + 10.0;
        coef++;
    } else R[1].x = 1.0;
    if (A.D == 'O') {
        Rectangle Z = {x+RSpace+coef*(RSide), y, RSide, RSide};
        R[2] = Z;
        RSpace = RSpace + 10.0;
        coef++;
    } else R[2].x = 1.0;
    if (A.L == 'O') {
        Rectangle Z = {x+RSpace+coef*(RSide), y, RSide, RSide};
        R[3] = Z;
        RSpace = RSpace + 10.0;
        coef++;
    } else R[3].x = 1.0;
}

void Shcs_Animation(const char beg, Vector2 *ShcP, const Vector2 EndP, float Speed,
                                  const float SIncrease, Vector2 StartPoint, int m, int n, int i, int Round, Music music) {

    if (beg == 'R' || beg == 'L') {
        if (beg == 'R') ShadowCastersDir[i] = -2; //-2 ~ left
        else ShadowCastersDir[i] = -3; //-3 ~ right
    }
    while (!((*ShcP).x == EndP.x && (*ShcP).y == EndP.y)) {
        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Draw_Map(StartPoint, m, n, Round, nExplorers-1); 
        EndDrawing();                                        

        if (beg == 'U') {
            (*ShcP).y += Speed;
            if ((*ShcP).y>EndP.y) break;
        }
        else if (beg == 'R') {
            (*ShcP).x -= Speed;
            if ((*ShcP).x<EndP.x) break;
        }
        else if (beg == 'D') {
            (*ShcP).y -= Speed;
            if ((*ShcP).y<EndP.y) break;
        }
        else if (beg == 'L') {
            (*ShcP).x += Speed;
            if ((*ShcP).x>EndP.x) break;
        }
        Speed += SIncrease;                                        
    }
    (*ShcP) = EndP;
}

void Exs_Animation(const char Mdir, const char Tdir, Vector2 *ExsP, const Vector2 EndP, float Speed,
                                const float SIncrease, Vector2 StartPoint, int m, int n, int i, int Round, Music music) {

    if (Mdir == 'A' || Mdir == 'D') {                              
        Explorers[i].direction = Tdir;
    }
    while (!((*ExsP).x == EndP.x && (*ExsP).y == EndP.y)) {
        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Draw_Map(StartPoint, m, n, Round, i);
        EndDrawing();

        if (Mdir == 'W') {                                    
            (*ExsP).y -= Speed;
            if ((*ExsP).y<EndP.y) break;
        }
        if (Mdir == 'S') {                                    
            (*ExsP).y += Speed;
            if ((*ExsP).y>EndP.y) break;
        }
        if (Mdir == 'A') {                                    
            (*ExsP).x -= Speed;
            if ((*ExsP).x<EndP.x) break;
        }
        if (Mdir == 'D') {                                    
            (*ExsP).x += Speed;
            if ((*ExsP).x>EndP.x) break;
        }                                      
        Speed += SIncrease;                                        
            
    }
    (*ExsP) = EndP;
}

int Lock_in_Rectangle(Rectangle R, Rectangle RBack, Rectangle RecsforCh[], int j, int i, int Exindex, Vector2 Mous) {
    Color Magneta = {180, 20, 200, 150};
    Color Yellow = {253, 249, 0, 150};
    Color Green = {0, 117, 44, 150};
    Color Red = {205, 50, 0, 255};

    DrawText("To quit choosing\n  wall state: 'Q'", 901, 235, 20, Red);
    DrawRectangle(R.x+0.5, R.y+0.5, R.width-1, R.height-1, Magneta);
    DrawRectangleRec(RBack, Yellow);
    if (RecsforCh[0].x != 1.0) {
        DrawRectangleRec(RecsforCh[0], Green);
        DrawText("U", RecsforCh[0].x+9, RecsforCh[0].y+5, 20, WHITE);
    }
    if (RecsforCh[1].x != 1.0) {
        DrawRectangleRec(RecsforCh[1], Green);
        DrawText("R", RecsforCh[1].x+9, RecsforCh[1].y+5, 20, WHITE);
    }
    if (RecsforCh[2].x != 1.0) {
        DrawRectangleRec(RecsforCh[2], Green);
        DrawText("D", RecsforCh[2].x+9, RecsforCh[2].y+5, 20, WHITE);
    }
    if (RecsforCh[3].x != 1.0) {
        DrawRectangleRec(RecsforCh[3], Green);
        DrawText("L", RecsforCh[3].x+9, RecsforCh[3].y+5, 20, WHITE);
    }

    if (RecsforCh[0].x != 1.0 && CheckCollisionPointRec(Mous, RecsforCh[0]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
        Explorers[Exindex].wallCount--;
        nInWalls++;
        InWalls[nInWalls-1].y = 2*j+1+(-1); InWalls[nInWalls-1].x = 2*i+1; InWalls[nInWalls-1].life = 2; 
        map[2*j+1+(-1)][2*i+1] = 2;
        return 1;
    }
    if (RecsforCh[1].x != 1.0 && CheckCollisionPointRec(Mous, RecsforCh[1]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
        Explorers[Exindex].wallCount--;
        nInWalls++;
        InWalls[nInWalls-1].y = 2*j+1; InWalls[nInWalls-1].x = 2*i+1+(+1); InWalls[nInWalls-1].life = 2;
        map[2*j+1][2*i+1+(+1)] = 2;
        return 1;
    }
    if (RecsforCh[2].x != 1.0 && CheckCollisionPointRec(Mous, RecsforCh[2]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
        Explorers[Exindex].wallCount--;
        nInWalls++;
        InWalls[nInWalls-1].y = 2*j+1+(+1); InWalls[nInWalls-1].x = 2*i+1; InWalls[nInWalls-1].life = 2;
        map[2*j+1+(+1)][2*i+1] = 2;
        return 1;
    }
    if (RecsforCh[3].x != 1.0 && CheckCollisionPointRec(Mous, RecsforCh[3]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
        Explorers[Exindex].wallCount--;
        nInWalls++;
        InWalls[nInWalls-1].y = 2*j+1; InWalls[nInWalls-1].x = 2*i+1+(-1); InWalls[nInWalls-1].life = 2;
        map[2*j+1][2*i+1+(-1)] = 2;
        return 1;
    }
    return 0;
}

int Show_Allowable_Walls(Rectangle R, Rectangle BackR, Rectangle Recs[], Vector2 Mouse) {
    Color Yellow = {253, 249, 0, 150};
    Color Red = {230, 41, 55, 150};    
    Color RedText = {205, 50, 0, 255};

    DrawText("To quit choosing\n  wall state: 'Q'", 901, 235, 20, RedText);
    if (CheckCollisionPointRec(Mouse, R)) {
        DrawRectangleRec(BackR, Yellow);
        if (Recs[0].x != 1.0) {
            DrawRectangleRec(Recs[0], Red);
            DrawText("U", Recs[0].x+9, Recs[0].y+5, 20, WHITE);
        }
        if (Recs[1].x != 1.0) {
            DrawRectangleRec(Recs[1], Red);
            DrawText("R", Recs[1].x+9, Recs[1].y+5, 20, WHITE);
        }
        if (Recs[2].x != 1.0) {
            DrawRectangleRec(Recs[2], Red);
            DrawText("D", Recs[2].x+9, Recs[2].y+5, 20, WHITE);
        }
        if (Recs[3].x != 1.0) {
            DrawRectangleRec(Recs[3], Red);
            DrawText("L", Recs[3].x+9, Recs[3].y+5, 20, WHITE);
        }
        if (CheckCollisionPointRec(Mouse, R) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return 1;
        }
    }       
    return 0;


}

void Check_Life_of_Interim_Walls() {
    int i;
    for (i=0; i<nInWalls; i++) {
        if (InWalls[i].life==1) {
            map[InWalls[i].y][InWalls[i].x] = 1;
            int j;
            for (j=i+1; j<nInWalls; j++) {
                InWalls[j-1] = InWalls[j];
            }
            nInWalls--;
            i--;
        } else {
            InWalls[i].life--;
        }
    }
}

void Pointer_To_Player(int index, Vector2 StartPoint) {
    // Draws a triangle on the player texture that it is his round. 
    double t = GetTime();
    float h = 1.5*sin(3*t) + 7.5;
    Color Blue = {50, 100, 255, 200};
    Vector2 v1 = GET_Start_Elements_Position_for_Draw(StartPoint, Explorers[index].mapPos);
    v1.x += 21; v1.y -= 12;
    if (!(v1.x==StartPoint.x && v1.y==StartPoint.y) && Explorers[index].isAlive) DrawPoly(v1, 3, h, 90, Blue); 
}

int Get_Explorer_Count_UI(int MaxPlayer) {
    Color color1 = {170, 30, 240, 255};
    Color color2 = {100, 25, 180, 255};
    Rectangle rec = {275, 260, 150, 150};
    const int space = 50;
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Enter the number of players", 370, 200, 25, RED);
    for (int i=0; i<MaxPlayer; i++) {
        DrawRectangleGradientH(rec.x+(i*rec.width+i*space), rec.y, rec.width, rec.height, color1, color2);
        char s[2] = "0";
        ItoS(s, i+1);
        DrawText(s, 1.25*rec.x+(i*rec.width+i*50), 1.2*rec.y, 50, WHITE);
    }
        EndDrawing();
}

int Get_Explorer_Count() {
    Rectangle rec = {275, 260, 150, 150};
    const int space = 50;
    Vector2 MousePos = GetMousePosition();
    int a = rec.width + space;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if ((MousePos.x>=rec.x && MousePos.x<=(rec.x+rec.width)) && (MousePos.y>=rec.y && MousePos.y<=(rec.y+rec.height))) 
            return 1;
        if ((MousePos.x>=(rec.x+a) && MousePos.x<=(rec.x+rec.width+a)) && (MousePos.y>=rec.y && MousePos.y<=(rec.y+rec.height))) 
            return 2;
        if ((MousePos.x>=(rec.x+a+a) && MousePos.x<=(rec.x+rec.width+a+a)) && (MousePos.y>=rec.y && MousePos.y<=(rec.y+rec.height)))
           return 3;
    }
    return 0;
}

// void Choose_Players_Texture_UI(int PlayersCount, int Player) {
//     const int MarginHor = 150;
//     const int MarginVer = 200;
//     const int space = 100;
//     char PlayerStr[2];
//     Rectangle ex1 = {MarginHor+(0*200+0*space), MarginVer+50, 170, 200};
//     Rectangle ex2 = {MarginHor+(1*200+1*space), MarginVer+50, 170, 200};
//     Rectangle ex3 = {MarginHor+(2*200+2*space), MarginVer+50, 170, 200};
//     BeginDrawing();
//     ClearBackground(RAYWHITE);
//     DrawText("Player -  -, choose your texture.", MarginHor+135, MarginVer-10, 30, RED);
//     ItoS(PlayerStr, Player);
//     DrawText(PlayerStr, MarginHor+265, MarginVer-10, 30, RED);
//     DrawTexture(Ex1Image, MarginHor+(0*200+0*space), MarginVer+50, RAYWHITE);
//     DrawTexture(Ex2Image, MarginHor+(1*200+1*space), MarginVer+50, RAYWHITE);
//     DrawTexture(Ex3Image, MarginHor+(2*200+2*space), MarginVer+50, RAYWHITE);
//     DrawRectangleLinesEx(ex1, 2.0, BLACK);
//     DrawRectangleLinesEx(ex2, 2.0, BLACK);
//     DrawRectangleLinesEx(ex3, 2.0, BLACK);
//     EndDrawing();
// }
// Texture2D Choose_Players_Texture() {
//     const int MarginHor = 150;
//     const int MarginVer = 200;
//     const int space = 100;
//     Vector2 v = GetMousePosition();
//     Rectangle ex1 = {MarginHor+(0*200+0*space), MarginVer+50, 170, 200};
//     Rectangle ex2 = {MarginHor+(1*200+1*space), MarginVer+50, 170, 200};
//     Rectangle ex3 = {MarginHor+(2*200+2*space), MarginVer+50, 170, 200};
//     if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//         if (CheckCollisionPointRec(v, ex1)) return Ex1TextureRight;
//         if (CheckCollisionPointRec(v, ex2)) return Ex2TextureRight;
//         if (CheckCollisionPointRec(v, ex3)) return Ex3TextureRight;
//     }
//     return;
// }

void Show_Invalid_Move_Error(Vector2 StartPoint, int m, int n, int Round, int ExRound) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    Draw_Map(StartPoint, m, n, Round, ExRound);      
    Color Red = {230, 41, 55, 150};
    Color Gray = {130, 130, 130, 150};
    DrawRectangle(206, 3, 491, 22, Gray);        
    DrawText("You can't go there. Pay attention to walls!", 233, 4, 20, Red);
    EndDrawing();
}

void Show_Ended_Walls_Error(Vector2 StartPoint, int m, int n, int Round, int ExRound) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    Draw_Map(StartPoint, m, n, Round, ExRound);
    Color Red = {230, 41, 55, 150};
    Color Gray = {130, 130, 130, 150};
    DrawRectangle(206, 3, 491, 22, Gray);
    DrawText("Your walls capacity ended. Try another choice.", 212, 4, 20, Red);
    EndDrawing();
}

void Dead_Explorer(int l, Sound DieSound, int Round) {
    Explorers[l].isAlive = false;
    Explorers[l].age = Round;
    map[(int)Explorers[l].mapPos.y][(int)Explorers[l].mapPos.x] = 1;
    PlaySound(DieSound);
}

void Win_Explorer(int l, Sound WinSound, int Round) {
        Explorers[l].isAlive = false;
        Explorers[l].age = Round;
        map[(int)Explorers[l].mapPos.y][(int)Explorers[l].mapPos.x] = 1;
        PlaySound(WinSound);
    }

int Are_All_Players_Have_Won() {
/*
result = 
0 : nobody won. 
1: all players won.
*/
    int x = 0;
    for (int i=0; i<nExplorers; i++) 
        if (Explorers[i].mapPos.x==Lightcore.x && Explorers[i].mapPos.y==Lightcore.y) 
            x ++;
    if (x == nExplorers) return 1; 
    else return 0;
}

int Are_All_Players_Dead() {
    int x=0;
    for (int i=0; i<nExplorers; i++) if (Explorers[i].isAlive) x ++;
    if (x == 0) return 1;
    return 0;
}

int Show_End_Screen() {    
    int i, WinnerCount=0, LoserCount=0;
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("WINNERS:" , 200, 190, 40, RED);
    DrawText("LOSERS:", 700, 190, 40, RED);
    for (i=0; i<nExplorers; i++) {
        char who[10]; char ExAge[3];
        if (Explorers[i].mapPos.x==Lightcore.x && Explorers[i].mapPos.y==Lightcore.y) {
            ItoS(who, i+1);
            ItoS(ExAge, Explorers[i].age);
            DrawText("Player", 220, 210+40*(WinnerCount+1), 30, RED);
            DrawText(who, 330, 210+40*(WinnerCount+1), 30, RED);
            DrawText(":", 348, 210+40*(WinnerCount+1), 30, RED);
            DrawText(ExAge, 355, 210+40*(WinnerCount+1), 30, RED);
            WinnerCount ++;
        }
        else {
            ItoS(who, i+1);
            ItoS(ExAge, Explorers[i].age);
            DrawText("Player", 720, 210+40*(LoserCount+1), 30, RED);
            DrawText(who, 830, 210+40*(LoserCount+1), 30, RED);
            DrawText(":", 848, 210+40*(LoserCount+1), 30, RED);
            DrawText(ExAge, 855, 210+40*(LoserCount+1), 30, RED);
            LoserCount ++;
        }
    }
    DrawRectangle(500, 450, 100, 50, LIGHTGRAY);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 v = GetMousePosition();
        if (v.x>=500 && v.x<=600 && v.y>=450 && v.y<=550) return 1;
    } 
    EndDrawing(); 
    return 0;
}



