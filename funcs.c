#include "raylib.h"
#include "funcs.h"

const int WallTh = 3;   // Thick of walls
const int FPS = 60;
const int WindowWidth = 1100;
const int WindowHeight = 650;
const int WidthSpacing = 200;   // This is for width of Hint Box
const int Spacing = 30;    // This is for height of Hint Box
const int Side = (WindowHeight - 2*Spacing)/12;     // Side length of every squre in map
int map[25][25];    // Max size for map  


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

// تغییر خانه مربوط به دیوار در آرایه map و بازگشت مختصات آن در آرایه
void SET_Walls_and_Return(WallPro Wall)
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

// دریافت مختصات دیوار در آرایه و بازگشت مختصات شروع ترسیم ان به تفکیک HorV
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

//تبدیل عدد کاربر به مختصات نقشه و بازگشت آن
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

// تبدیل مختصات نقشه به مختصات مرکز مربع
Vector2 GET_Start_Elements_Positin_for_Draw(Vector2 StartPoint, Vector2 Element)
{
/*
Converts a map-grid coordinate to the on-screen center of its tile.
Screen position = StartPoint + (Element * Side/2).
*/
        int i = StartPoint.x + (Element.x)*Side/2;
        int j = StartPoint.y + (Element.y)*Side/2;
        Vector2 Position = {i, j};
        return Position;
}

void Draw_Map(Vector2 StartPoint, int m, int n)
{
/*
Renders all horizontal and vertical walls of the map.
Horizontal scan:
    j = 0..2*m (step 2), i = 1..2*n (step 2)
    Each (i, j) is drawn as a horizontal segment.
Vertical scan:
    i = 0..2*n (step 2), j = 1..2*m (step 2)
    Each (i, j) is drawn as a vertical segment.
Color rules:
    1  → thin preview line
    0  → active wall (red)
    -1  → map border (black)
    other →  wall from explorer (orange)
*/
    int i, j;
    WallPro W;
    Vector2 StartP, EndP;
    Color O = {255, 161, 0, 255};     // Orange
    Color R = {230, 41, 55, 255};     // Red
    Color B = {0, 0, 0, 255*(0.4f)};         // Black

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
}