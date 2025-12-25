#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "funcs.h"

const int WallTh = 3;   // Thick of walls
const int FPS = 60;
const int WindowWidth = 1100;
const int WindowHeight = 650;
const int WidthSpacing = 200;   // This is for width of Hint Box
const int Spacing = 30;    // This is for distance of top and bottom of map and used in the Side
const int Side = (WindowHeight - 2*Spacing)/12;     // Side length of every squre in map
int map[25][25];    // Max size for map  

Vector2 Lightcore;
int nExplorers;
Vector2 Explorers[3];
int nShadowCasters;
Vector2 ShadowCasters[3];

// Load textures
// Texture2D ShTextureRight;
// Texture2D ExTextureRight;
// Texture2D ShTextureLeft;
// Texture2D ExTextureLeft;
// Texture2D LiTexture;

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
}

Vector2 GET_StartPoint(int m, int n, int WidthSpacing)
{
/*
Calculates the top-left starting position for centering the map on screen.
MapSize is derived from (m, n) and tile size.
Horizontal spacing (WidthSpacing) is subtracted before centering.
*/
    Vector2 MapSize, StartPoint;
    MapSize.x = n*Side;
    MapSize.y = m*Side;
    StartPoint.x = ((WindowWidth - WidthSpacing) - MapSize.x)/2;
    StartPoint.y = ((WindowHeight) - MapSize.y)/2;
    return StartPoint;
}

void SET_Walls(WallPro Wall)
{
/*
Marks the corresponding map cell for a wall and updates its position.
For horizontal walls (H/h), the cleared cell is at (2y+2, 2x+1).
For vertical walls, the cleared cell is at (2y+1, 2x+2).
*/
    if(Wall.HorV == 'H' || Wall.HorV == 'h')
    {
        int j = 2*Wall.Position.y + 2;
        int i = 2*Wall.Position.x + 1;
        map[j][i] = 0;
    }
    else
    {
        int j = 2*Wall.Position.y + 1;
        int i = 2*Wall.Position.x + 2;
        map[j][i] = 0;
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
        S.y = StartPoint.y + ((Wall.Position.y)/2)*Side;
        S.x = StartPoint.x + ((Wall.Position.x - 1)/2)*Side;
        return S;
    }
    else 
    {
        S.y = StartPoint.y + ((Wall.Position.y - 1)/2)*Side;
        S.x = StartPoint.x + ((Wall.Position.x)/2)*Side;
        return S;
    }
}

Vector2 Return_Elements_Position(Vector2 Element)
{
/*
Converts a logical element position to its corresponding map-grid index.
Each element maps to (2x+1, 2y+1) in the expanded grid.
*/
    int i = 2*Element.x + 1;
    int j = 2*Element.y + 1;
    Vector2 Result; Result.x = i; Result.y = j;
    return Result;
}

Vector2 GET_Start_Elements_Position_for_Draw(Vector2 StartPoint, Vector2 Element)
{
/*
Converts a map-grid coordinate to the on-screen center of its tile.
Screen position = StartPoint + (Element * Side/2).
*/
        int i = StartPoint.x + (Element.x-1)*Side/2 + 2;
        int j = StartPoint.y + (Element.y-1)*Side/2 + 3;
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
        if (ShadowCaster.x>Explorers[i].x)
        {
            if (ShadowCaster.y>Explorers[i].y) Distance = (ShadowCaster.x - Explorers[i].x) + (ShadowCaster.y - Explorers[i].y);
            else Distance = (ShadowCaster.x - Explorers[i].x) + (Explorers[i].y - ShadowCaster.y);
        }
        else 
        {
            if (ShadowCaster.y>Explorers[i].y) Distance = (Explorers[i].x - ShadowCaster.x) + (ShadowCaster.y - Explorers[i].y);
            else Distance = (Explorers[i].x - ShadowCaster.x) + (Explorers[i].y - ShadowCaster.y);
        }
        if (Distance<MinDistance) 
        {
            MinDistance = Distance;
            NearExplorer = i;
        }
    }
    if (Explorers[NearExplorer].x>=ShadowCaster.x) return 1;
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
        if ((E.x == Explorers[i].x) && (E.y == Explorers[i].y)) return 0;    // Element is in explorer
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

void Draw_Map(Vector2 StartPoint, int m, int n)
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
            W.Position.x = i, W.Position.y = j, W.HorV = 'H';
            StartP = GET_Start_Walls_Position_for_Draw(StartPoint, W);
            EndP.x = StartP.x + Side, EndP.y = StartP.y;
            if (map[j][i] == 1) DrawLineEx(StartP, EndP, 1, B);
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
            W.Position.x = i, W.Position.y = j, W.HorV = 'V';
            StartP = GET_Start_Walls_Position_for_Draw(StartPoint, W);
            EndP.x = StartP.x, EndP.y = StartP.y + Side;
            if (map[j][i] == 1) DrawLineEx(StartP, EndP, 1, B);
            else if(map[j][i] == 0) DrawLineEx(StartP, EndP, WallTh, R);
            else if(map[j][i] == -1) DrawLineEx(StartP, EndP, WallTh, BLACK);
            else DrawLineEx(StartP, EndP, WallTh, O);
        }
    }

// Drawing lightcore
    Vector2 S = GET_Start_Elements_Position_for_Draw(StartPoint, Lightcore);
    DrawTexture(LiTexture, S.x, S.y, WHITE);

// Draw explorers (facing toward lightcore)
    for (i=0; i<nExplorers; i++)
    {
        Vector2 S = GET_Start_Elements_Position_for_Draw(StartPoint, Explorers[i]);
        if (i==0)
        {
        int Direction = Direction_of_Explorers(Explorers[i]);
        if (Direction == 1) DrawTexture(Ex1TextureRight, S.x, S.y, WHITE);
        else DrawTexture(Ex1TextureLeft, S.x, S.y, WHITE);
        }
        else if (i==1)
        {
        int Direction = Direction_of_Explorers(Explorers[i]);
        if (Direction == 1) DrawTexture(Ex2TextureRight, S.x, S.y, WHITE);
        else DrawTexture(Ex2TextureLeft, S.x, S.y, WHITE);
        }
        else if (i==2)
        {
        int Direction = Direction_of_Explorers(Explorers[i]);
        if (Direction == 1) DrawTexture(Ex3TextureRight, S.x, S.y, WHITE);
        else DrawTexture(Ex3TextureLeft, S.x, S.y, WHITE);
        }
    }

// Draw shadow casters (facing toward nearest explorer)
    for (i=0; i<nShadowCasters; i++)
    {
        Vector2 S = GET_Start_Elements_Position_for_Draw(StartPoint, ShadowCasters[i]);
        if (i==0)
        {
        int Direction = Direction_of_ShadowCasters(ShadowCasters[i]);
        if (Direction == 1) DrawTexture(Sh1TextureRight, S.x, S.y, WHITE);
        else DrawTexture(Sh1TextureLeft, S.x, S.y, WHITE);
        }
        else if (i==1)
        {
        int Direction = Direction_of_ShadowCasters(ShadowCasters[i]);
        if (Direction == 1) DrawTexture(Sh2TextureRight, S.x, S.y, WHITE);
        else DrawTexture(Sh2TextureLeft, S.x, S.y, WHITE);
        }
        else if (i==2)
        {
        int Direction = Direction_of_ShadowCasters(ShadowCasters[i]);
        if (Direction == 1) DrawTexture(Sh3TextureRight, S.x, S.y, WHITE);
        else DrawTexture(Sh3TextureLeft, S.x, S.y, WHITE);
        }
    }
}

int Distance_Check(Vector2 v1, Vector2 v2, Vector2 arr1[], int arr1c, Vector2 arr2[], int arr2c)
{
/*
Computes distance between v1 and v2 and return 1 if it is valid otherwise, 0.
*/
    float dx, dy;

    // check v2
    dx = 2*v1.x+1 - v2.x;
    dy = 2*v1.y+1 - v2.y;
    if (dx*dx + dy*dy < 16.0f) return 0;
    // printf("\nE&light dx=%.1f dy=%.1f", dx, dy);

    // check arr1
    for (int i = 0; i < arr1c; i++)
    {
        dx = 2*v1.x+1 - arr1[i].x;
        dy = 2*v1.y+1 - arr1[i].y;
        if (dx*dx + dy*dy < 16.0f) return 0;
    // else printf("\nE&ex dx=%.1f dy=%.1f", dx, dy);
    }

    // check arr2
    for (int i = 0; i < arr2c; i++)
    {
        dx = 2*v1.x+1 - arr2[i].x;
        dy = 2*v1.y+1 - arr2[i].y;
        if (dx*dx + dy*dy < 16.0f) return 0;
    // else printf("\nE&sh dx=%.1f dy=%.1f", dx, dy);
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
    w.Position.x = x;
    w.Position.y = y;
    w.HorV = HorV;

    return w;
}