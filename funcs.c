#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "funcs.h"

const float WallTh = 3;   // Thick of walls
const int FadeCo = 7;
int swF[3] = {1, 1, 1};
const int FPS = 60;
const int WindowWidth = 1100;
const int WindowHeight = 650;
const int WidthSpace = 200;   // This is for width of Hint Box
const int Space = 30;    // This is for distance of top and bottom of map and used in the Side
const int Side = (WindowHeight - 2*Space)/12;     // Side length of every squre in map
int map[25][25];    // Max size for map  

Vector2 Lightcore = {0.0f};
int nExplorers;
Vector2 Explorers[3] = {0.0f};
int nShadowCasters;
Vector2 ShadowCasters[3] = {0.0f};
int FadeSh[3];

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
    map[1][1] = 0;
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
            W.Position.x = (float)i; W.Position.y = (float)j; W.HorV = 'H';
            StartP = GET_Start_Walls_Position_for_Draw(StartPoint, W);
            EndP.x = StartP.x + Side; EndP.y = StartP.y;
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
            W.Position.x = (float)i; W.Position.y = (float)j; W.HorV = 'V';
            StartP = GET_Start_Walls_Position_for_Draw(StartPoint, W);
            EndP.x = StartP.x; EndP.y = StartP.y + Side;
            if (map[j][i] == 1) DrawLineEx(StartP, EndP, 1, B);
            else if(map[j][i] == 0) DrawLineEx(StartP, EndP, WallTh, R);
            else if(map[j][i] == -1) DrawLineEx(StartP, EndP, WallTh, BLACK);
            else DrawLineEx(StartP, EndP, WallTh, O);
        }
    }

// Drawing lightcore
    Vector2 S = GET_Start_Elements_Position_for_Draw(StartPoint, Lightcore);
    DrawTexture(LiTexture, S.x, S.y, WHITE);

// // Draw explorers (facing toward lightcore)
//     for (i=0; i<nExplorers; i++)
//     {
//         Vector2 S = GET_Start_Elements_Position_for_Draw(StartPoint, Explorers[i]);
//         if (i==0)
//         {
//         int Direction = Direction_of_Explorers(Explorers[i]);
//         if (Direction == 1) DrawTexture(Ex1TextureRight, S.x, S.y, WHITE);
//         else DrawTexture(Ex1TextureLeft, S.x, S.y, WHITE);
//         }
//         else if (i==1)
//         {
//         int Direction = Direction_of_Explorers(Explorers[i]);
//         if (Direction == 1) DrawTexture(Ex2TextureRight, S.x, S.y, WHITE);
//         else DrawTexture(Ex2TextureLeft, S.x, S.y, WHITE);
//         }
//         else if (i==2)
//         {
//         int Direction = Direction_of_Explorers(Explorers[i]);
//         if (Direction == 1) DrawTexture(Ex3TextureRight, S.x, S.y, WHITE);
//         else DrawTexture(Ex3TextureLeft, S.x, S.y, WHITE);
//         }
//     }

// Draw shadow casters (facing toward nearest explorer)
    for (i=0; i<nShadowCasters; i++)
    {
        Color ColorSh = {255, 255, 255, ((float)FadeSh[i]/(10.0f*FadeCo))*255};
        Vector2 S = GET_Start_Elements_Position_for_Draw(StartPoint, ShadowCasters[i]);
        if (i==0)
        {
        int Direction = Direction_of_ShadowCasters(ShadowCasters[i]);
        if (Direction == 1) DrawTexture(Sh1TextureRight, S.x, S.y, ColorSh);
        else DrawTexture(Sh1TextureLeft, S.x, S.y, ColorSh);
        }
        else if (i==1)
        {
        int Direction = Direction_of_ShadowCasters(ShadowCasters[i]);
        if (Direction == 1) DrawTexture(Sh2TextureRight, S.x, S.y, ColorSh);
        else DrawTexture(Sh2TextureLeft, S.x, S.y, ColorSh);
        }
        else if (i==2)
        {
        int Direction = Direction_of_ShadowCasters(ShadowCasters[i]);
        if (Direction == 1) DrawTexture(Sh3TextureRight, S.x, S.y, ColorSh);
        else DrawTexture(Sh3TextureLeft, S.x, S.y, ColorSh);
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
    dx = 2*v1.x+1.0f - v2.x;
    dy = 2*v1.y+1.0f - v2.y;
    if (dx*dx + dy*dy < 16.0f) return 0;

    // check arr1
    for (int i = 0; i < arr1c; i++)
    {
        dx = 2*v1.x+1.0f - arr1[i].x;
        dy = 2*v1.y+1.0f - arr1[i].y;
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

void Reset_Map_Blocks(int m, int n)
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
    const float ElementSpeed = 2.0;
    Vector2 v = E;
    if (Dir == 'D') v.x += ElementSpeed;
    else if (Dir == 'W') v.y -= ElementSpeed;
    else if (Dir == 'A') v.x -= ElementSpeed;
    else if (Dir == 'S') v.y += ElementSpeed;
    return v;
}

int Can_Ex_Move_for_Walls(Vector2 E, char Dir)
{
    int x = E.x, y = E.y;
    if (Dir == 'W') {if (map[y-1][x] == 0  || map[y-1][x] == -1) return 0;}
    else if (Dir == 'S') {if (map[y+1] [x] == 0  ||  map[y+1][x] == -1) return 0;}
    else if (Dir == 'A') {if (map[y][x-1] == 0  ||  map[y][x-1] == -1) return 0;}
    else if (Dir == 'D') {if (map[y][x+1] == 0  ||  map[y][x+1] == -1) return 0;}
    
    return  1;
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
        // Rectangle UpRecs = {HorMargin+i*(Distance+RecWidth), VerMargin, RecWidth, RecHeight};
        // DrawRectangleRounded(UpRecs, 2.0f, 1.0f, YELLOW);
        itoa(n+i, nn, 10);
        DrawText(nn, HorMargin+i*(Distance+RecWidth)+60, VerMargin+35, 30, BLACK);
        DrawRectangleGradientH(HorMargin+i*(Distance+RecWidth), (VerMargin+Distance+RecHeight), RecWidth, RecHeight, YELLOW, GOLD);
        // Rectangle DownRecs = {HorMargin+i*(Distance+RecWidth), (VerMargin+Distance+RecHeight), RecWidth, RecHeight};
        // DrawRectangleRounded(DownRecs, 2.0f, 1.0f, YELLOW);
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
    for (int i=0; n; i++) {
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
        // if (CheckCollisionPointRec(GetMousePosition(), SubmitButton))
        if (MousePos.x >= SubmitButton.x &&
             MousePos.x <= (SubmitButton.x+SubmitButton.width) &&
             MousePos.y >= SubmitButton.y &&
             MousePos.y <= (SubmitButton.y+SubmitButton.height)) 
                if (InpNumber>0 && InpNumber<=((n-1)*(m-1)))
    return 0;
}

