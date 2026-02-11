#ifndef FUNCS_H
#define FUNCS_H

#include "raylib.h"

// Replay: Player plays his round again.
// InWallIncrease: Player got 2 interim wall.
// Earthquake: Every charachter in map, moves to a near block.
// ForceEnemy: Player can force an enemy to a near block. 
typedef enum {Replay, InWallIncrease, Earthquake, ForceEnemy} Gift;

typedef struct SidesAroundRectangle {
    char U;
    char R;
    char D;
    char L;
} SidesAR;
typedef struct {
    int y;
    int x;
    char beg;
} Vector;
typedef struct WallProperty {
    Vector2 Position; //بهتر بود int باشه
    char HorV;
} WallPro;
typedef struct {
    int y;
    int x;
    int life;
} InterimWalls;
typedef struct Explorer {
    bool isAlive;
    int age;
    Vector2 mapPos;   // coordinate in map array
    Vector2 winPos;    // coordinate in window
    int wallCount;
    char direction;
    Texture2D avatar[2];    // 0 = left texture & 1 = right texture
} Explorer;
typedef struct Present {
    bool isGotten;
    Vector2 mapPos;    // coordinate in map array
    Vector2 winPos;     // coordinate in window
    Gift type;
} Present;
typedef struct EarthquakeWall {
    int level;
    Vector2 Start;
    Vector2 End;
    char HorV;
    int dir;
    float pixels;
} EarthqWall;

extern const int Side;
extern const int FPS;
extern const int FadeCo;
extern const float WallTh;
extern const int WindowWidth;
extern const int WindowHeight;
extern const int WidthHintBox;
extern const int MarginSpace;

extern int map[25][25];
extern Vector2 Lightcore;
extern int nExplorers;
extern Explorer Explorers[3];
// extern Vector2 ExplorersP[3];
// extern char ExplorersDir[3];
// extern int nInterimWalls[3];
extern int nInWalls; 
extern InterimWalls InWalls[30];
extern Present Gifts[3];
extern int nGifts;

extern int nShadowCasters;
extern Vector2 ShadowCasters[3];
extern Vector2 ShadowCastersP[3];
extern int ShadowCastersDir[3];
extern int FadeSh[3];
extern int swF[3];

extern Texture2D Ex1Image;
extern Texture2D Ex2Image;
extern Texture2D Ex3Image;
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
extern Texture2D PresentTexture;

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
void Draw_Map(int Earthquake, Vector2 StartPoint, int m, int n, int Round, int ExRound);
int Distance_Check(Vector2 v1, Vector2 v2, Explorer arr1[], int arr1c, Vector2 arr2[], int arr2c);
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
int BFS_Way(Vector *start, Vector *end, int *ACount, int *resCount, Vector *result);
int Find_Way(Vector *end, const int ACount, int resCount, Vector *Alist, Vector *result);
void Draw_Way(Vector *Way, Vector2 StartPoint, const int resCount);
void Rec_for_Choose(float x, float y, SidesAR A, Rectangle R[]);
SidesAR CheckSides(int j, int i);
void Shcs_Animation(const char beg, Vector2 *ShcP, const Vector2 EndP, float Speed, const float SIncrease, Vector2 StartPoint, int m, int n, int i, int Round, Music music);
void Exs_Animation(const char Mdir, const char Tdir, Vector2 *ExsP, const Vector2 EndP, float Speed, const float SIncrease, Vector2 StartPoint, int m, int n, int i, int ExRound, Music music);
int Lock_in_Rectangle(Rectangle R, Rectangle RBack, Rectangle RecsforCh[], int j, int i, int Exindex, Vector2 Mous);
int Show_Allowable_Walls(Rectangle R, Rectangle BackR, Rectangle Recs[], Vector2 Mous);
void Check_Life_of_Interim_Walls();
void Pointer_To_Player(int index, Vector2 StartPoint);
int Get_Explorer_Count_UI(int MaxPlayer);
int Get_Explorer_Count();
// void Choose_Players_Texture_UI(int PlayersCount, int Player);
// Texture2D Choose_Players_Texture();
void Show_Invalid_Move_Error(Vector2 StartPoint, int m, int n, int Round, int ExRound);
void Show_Ended_Walls_Error(Vector2 StartPoint, int m, int n, int Round, int ExRound);
void Dead_Explorer(int l, Sound DieSound, int Round);
void Win_Explorer(int l, Sound WinSound, int Round);
int Are_All_Players_Have_Won();
int Are_All_Players_Dead();
int Calculate_Max_Interim_Wall(int m, int n);
int Show_End_Screen();
int Is_Present_Gotten();
void Show_Present_Rec(Vector2 StartPoint, int m, int n, int Round, int ExRound, Music music);
void Show_Present(Vector2 StartPoint, int m, int n, int Round, int ExRound, Gift name, Music music);
void ReplayGift(int *l);
void InWallIncreaseGift(int l);
int BFS_Gift(int checked[][2], int start, int end, int m, int n, int len);
void Number_Gifts(int m, int n);
int Rectangles_Around_Shc(Vector2 RecsMapP[], Rectangle RecsAround[], Vector2 Shc, Vector2 StartPoint);
void Force_Shc(Vector2 StartPoint, int m, int n, Music GameMusic, int Round, int ExRound);
void Add_Earthquake_Wall(EarthqWall Walls[], int *nWalls, Vector2 StartPoint, int m, int n, int EarthqMap[][2*n+1], int N, float pixels);
void Coordinate_Around_for_Earthquake(Vector2 mapP, Vector2 Around[], int *nAround);
void Choose_RandomPositions_Around_Element(int ShOrEx, Vector2 *mapP, Vector2 StartPoint, int m, int n, int Index, int Round, Music GameMusic);
void Earthquake_Gift(int m, int n, Vector2 StartPoint, int Round, Music GameMusic);
void Shcs_Animation_without_Change_Direction(const char beg, Vector2 *ShcP, const Vector2 EndP, float Speed, const float SIncrease, Vector2 StartPoint, int m, int n, int i, int Round, Music music);
void Exs_Animation_without_Change_Direction(const char Mdir, Vector2 *ExsP, const Vector2 EndP, float Speed, const float SIncrease, Vector2 StartPoint, int m, int n, int i, int Round, Music music);

#endif