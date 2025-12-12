#ifndef FUNCS_H
#define FUNCS_H

#include "raylib.h"

extern int map[25][25];
extern const int Side;
extern const int FPS;
extern const int WallTh;
extern const int WindowWidth;
extern const int WindowHeight;
extern const int WidthSpacing;
extern const int Spacing;

//ساختار دریافت دیوارها از کاربر که اول باید y گرفته شود
typedef struct WallProperty {
    Vector2 Position;
    char HorV;
} WallPro;

void SET_Map_Array(int M[][25], int m, int n);
Vector2 GET_StartPoint(int m, int n, int WidthSpacing);
void SET_Walls_and_Return(WallPro Wall);
Vector2 GET_Start_Walls_Position_for_Draw(Vector2 StartPoint, WallPro Wall);
Vector2 Return_Elements_Position(Vector2 Element);
Vector2 GET_Start_Elements_Positin_for_Draw(Vector2 StartPoint, Vector2 Element);
void Draw_Map(Vector2 StartPoint, int m, int n);
#endif