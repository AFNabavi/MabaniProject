#ifndef FUNCS_H
#define FUNCS_H

#include "raylib.h"

extern const int Side;
extern const int FPS;
extern int swF[3];
extern const int FadeCo;
extern const float WallTh;
extern const int WindowWidth;
extern const int WindowHeight;
extern const int WidthSpace;
extern const int Space;

extern int map[25][25];
extern Vector2 Lightcore;
extern int nExplorers;
extern Vector2 Explorers[3];
extern int nShadowCasters;
extern Vector2 ShadowCasters[3];
extern int FadeSh[3];

extern int minlength;
extern char FirstMove;
extern char SecondMove;
typedef struct{
    int Length;
    char FM;
    char SM;
    int indexEx;
}ShcMoveData;

typedef struct {
    int x;
    int y; 
    struct vector* parent;
} vector;

extern Texture2D Sh1TextureRight;
extern Texture2D Sh2TextureRight;
extern Texture2D Sh3TextureRight;
extern Texture2D Ex1TextureRight;
extern Texture2D Ex2TextureRight;
extern Texture2D Ex3TextureRight;
extern Texture2D Sh1TextureLeft;
extern Texture2D Sh2TextureLeft;
extern Texture2D Sh3TextureLeft;
extern Texture2D Ex1TextureLeft;
extern Texture2D Ex2TextureLeft;
extern Texture2D Ex3TextureLeft;
extern Texture2D LiTexture;

typedef struct WallProperty {
    Vector2 Position;
    char HorV;
} WallPro;

void SET_Map_Array(int M[][25], int m, int n);
Vector2 GET_StartPoint(int m, int n, int WidthSpace);
Vector2 SET_Walls(WallPro Wall);
Vector2 GET_Start_Walls_Position_for_Draw(Vector2 StartPoint, WallPro Wall);
Vector2 Return_Elements_Position(Vector2 Element);
Vector2 GET_Start_Elements_Position_for_Draw(Vector2 StartPoint, Vector2 Element);
int Direction_of_Explorers(Vector2 Explorer);
int Direction_of_ShadowCasters(Vector2 ShadowCaster);
int Check_Elements(Vector2 E, int numberExNow, int numberShNow);
int Check_Walls(WallPro W);
void Draw_Map(Vector2 StartPoint, int m, int n);
int Distance_Check(Vector2 v1, Vector2 v2, Vector2 arr1[], int arr1c, Vector2 arr2[], int arr2c);
WallPro Put_Wall(int m, int n);
void BFS_Check(char sw, int BlocksA[][2], int ACount, int BlocksB[][2], int BCount, int *Checked);
void Reset_Map_Blocks_for_BFS(int m, int n);
void Fade_ShadowCasters();
void Initializing_FadeSh();
Vector2 Move_Element(Vector2 E, char Dir);
int Can_Ex_Move_for_Walls(Vector2 E, char Dir);
int Win_or_Lose(Vector2 E, Vector2 Sh[], int nSh, Vector2 L);
int Get_Map_Infs();
void Draw_Map_Infs();
void Draw_Walls_Infs(char s[], int n, int m);
char Print_Number_In_String(char s[], char ch, int len);
void ItoS(char *str, int n);
int StoI(char s[], int sLen);
int Submit_Button(int n, int m, char inp[], int inpLen);
void Reset_Map_Blocks_for_Move_Elements(int m, int n); 
void Move_Shcs(Vector2 Ex, float j, float i, int Len, char FMove, char SMove);
void Set_Move_of_Sh_in_Map(ShcMoveData ShM, int index);
void *BFS_for_Way(vector begining, vector end);
void Draw_Hint_Box();


#endif