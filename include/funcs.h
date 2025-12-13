#ifndef FUNCS_H
#define FUNCS_H

#include "raylib.h"

extern const int Side;
extern const int FPS;
extern const int WallTh;
extern const int WindowWidth;
extern const int WindowHeight;
extern const int WidthSpacing;
extern const int Spacing;

extern int map[25][25];
extern Vector2 Lightcore;
extern int nExplorers;
extern Vector2 Explorers[3];
extern int nShadowCasters;
extern Vector2 ShadowCasters[3];

extern Texture2D ShTextureRight;
extern Texture2D ExTextureRight;
extern Texture2D ShTextureLeft;
extern Texture2D ExTextureLeft;
extern Texture2D LiTexture;

typedef struct WallProperty {
    Vector2 Position;
    char HorV;
} WallPro;

void SET_Map_Array(int M[][25], int m, int n);
Vector2 GET_StartPoint(int m, int n, int WidthSpacing);
void SET_Walls(WallPro Wall);
Vector2 GET_Start_Walls_Position_for_Draw(Vector2 StartPoint, WallPro Wall);
Vector2 Return_Elements_Position(Vector2 Element);
Vector2 GET_Start_Elements_Position_for_Draw(Vector2 StartPoint, Vector2 Element);
int Direction_of_Explorers(Vector2 Explorer);
int Direction_of_ShadowCasters(Vector2 ShadowCaster);
int Check_Elements(Vector2 E, int numberExNow, int numberShNow);
int Check_Walls(WallPro W);
void Draw_Map(Vector2 StartPoint, int m, int n);
#endif