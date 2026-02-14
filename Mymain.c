#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "funcs.h"

Texture2D Ex1Image; Texture2D Ex2Image; Texture2D Ex3Image;
Texture2D Sh1TextureRight; Texture2D Sh2TextureRight; Texture2D Sh3TextureRight;
Texture2D Ex1TextureRight; Texture2D Ex2TextureRight; Texture2D Ex3TextureRight;
Texture2D Sh1TextureLeft; Texture2D Sh2TextureLeft; Texture2D Sh3TextureLeft;
Texture2D Ex1TextureLeft; Texture2D Ex2TextureLeft; Texture2D Ex3TextureLeft;
Texture2D LiTexture; Texture2D PresentTexture;

// Enum to represent the different screens or states of the game application.
// - TitleScreen: The initial screen where the game title is displayed and the user can start the game.
// - GameScreen: The main gameplay screen where the game logic and interactions occur.
// - EndScreen: The screen shown at the end of the game, possibly displaying results, scores, or restart options.
typedef enum {TitleScreen, GameScreen, EndScreen} Screen;

// Enum to represent the levels or phases within the game screen.
// - GET: Phase for getting initial inputs like map size, positions of elements, and walls.
// - MoveExs: Phase for moving the explorers (Exs stands for Explorers).
// - MoveShs: Phase for moving the shadow casters (Shs stands for ShadowCasters).
typedef enum {GET, MoveExs, MoveShs} Level;

bool ShowTitleNote3 = false;    // Flag to show third title note after click, for input prompt.
bool Win = false;   // Flag for player win, affects EndScreen.
bool Init_FadeSh = true;
Color BackColor = RAYWHITE;

int main() {
InitWindow(WindowWidth, WindowHeight, "The Tale of the Labyrinth");
InitAudioDevice();
srand(time(NULL));    // randomize choices
// Load files
Sh1TextureRight = LoadTexture("output\\source\\shadowcaster1_right_image.png");
Sh2TextureRight = LoadTexture("output\\source\\shadowcaster2_right_image.png");
Sh3TextureRight = LoadTexture("output\\source\\shadowcaster3_right_image.png");
Sh1TextureLeft = LoadTexture("output\\source\\shadowcaster1_left_image.png");
Sh2TextureLeft = LoadTexture("output\\source\\shadowcaster2_left_image.png");
Sh3TextureLeft = LoadTexture("output\\source\\shadowcaster3_left_image.png");
Ex1TextureRight = LoadTexture("output\\source\\explorer1_right_image.png");
Ex2TextureRight = LoadTexture("output\\source\\explorer2_right_image.png");
Ex3TextureRight = LoadTexture("output\\source\\explorer3_right_image.png");
Ex1TextureLeft = LoadTexture("output\\source\\explorer1_left_image.png");
Ex2TextureLeft = LoadTexture("output\\source\\explorer2_left_image.png");
Ex3TextureLeft = LoadTexture("output\\source\\explorer3_left_image.png");
LiTexture = LoadTexture("output\\source\\light_core_image.png");
PresentTexture = LoadTexture("output\\source\\present_image.png");
Music music1 = LoadMusicStream("output\\source\\music1.ogg");
Music music2 = LoadMusicStream("output\\source\\music2.ogg");
Music music3 = LoadMusicStream("output\\source\\music3.ogg");
Music music4 = LoadMusicStream("output\\source\\music4.ogg");
Music musics[4] = {music1, music2, music3, music4};
Music EndGameMusic = LoadMusicStream("output\\source\\end_game.mp3");
Sound VictorySound = LoadSound("output\\source\\victory_sound.mp3");
Sound DieSound = LoadSound("output\\source\\die_sound.mp3");
Sound GiveGiftSound = LoadSound("output\\source\\give_gift_sound.mp3");
Sound MovingSound = LoadSound("output\\source\\force_sound.mp3");
Sound ShadowSound = LoadSound("output\\source\\shadow_sound.mp3");
Sound EarthquakeSound = LoadSound("output\\source\\earthquake.mp3");
Sound ForceSound = LoadSound("output\\source\\moving_sound.mp3");

Screen Current = TitleScreen;   // Current screen, starts at TitleScreen.
Level State = GET;   // Current level, starts at GET for inputs.

Texture2D ExTextures[3][2] = {{Ex1TextureLeft, Ex1TextureRight}, 
                            {Ex2TextureLeft, Ex2TextureRight},
                            {Ex3TextureLeft, Ex3TextureRight}} ;
Music GameMusic = musics[rand()%4];
PlayMusicStream(GameMusic);
SetTargetFPS(FPS);

int m=-1, n=-1, nWalls;
Vector2 StartPoint;

int FPScounter = 2*FPS; //That is for show TitleNote3
int l=0; 
int Round = 1;

bool allAreDied = false;
double TimeEnding;

while (!WindowShouldClose())
{
switch(Current)
{
    case TitleScreen:
    {
        // First phase, click on the screen to start the game
        Rectangle TitleRec = {(WindowWidth-450)/2, (WindowHeight- 180)/2, 450, 180};
        Color TitleColorNotes = { 112, 31, 126, 255};
        Vector2 Mous = GetMousePosition();
        BeginDrawing();
        ClearBackground(BackColor);
        DrawRectangleRoundedLinesEx(TitleRec, 0.4f, 25, 4, TitleColorNotes);
        int TitleNote1 = MeasureText("Click me to play", 40);
        int TitleNote2 = MeasureText("Explorer Game!", 50);
        DrawText("Click me to play", TitleRec.x+(TitleRec.width-TitleNote1)/2, TitleRec.y+40, 40, TitleColorNotes);
        DrawText("Explorer Game!", TitleRec.x+(TitleRec.width-TitleNote2)/2, TitleRec.y+40+50, 50, TitleColorNotes);
        if (CheckCollisionPointRec(Mous, TitleRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ShowTitleNote3 = true;
        if (ShowTitleNote3)
        { 
            PlayMusicStream(GameMusic);
            int TitleNote3 = MeasureText("now input game details.", 20);
            DrawText("now input game details.", TitleRec.x+(TitleRec.width-TitleNote3)/2, TitleRec.y+40+50+100, 20, RED);
            FPScounter --;
            if(FPScounter<0) Current = GameScreen;
        }

        Rectangle LoadingRec = {(WindowWidth-225)/2, (WindowHeight-90)/2+TitleRec.height+10, 225, 90};
        DrawRectangleRoundedLinesEx(LoadingRec, 0.4f, 25, 3, TitleColorNotes);
        DrawText(" Load the\nlast save.", (WindowWidth-225)/2+40, (WindowHeight-90)/2+TitleRec.height+20, 30, TitleColorNotes);
        if (CheckCollisionPointRec(Mous, LoadingRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Current = GameScreen;
            State = MoveExs;
            Load_Game(&m, &n, &l, &Round, &StartPoint);
            Draw_Map(0, StartPoint, m, n, Round, l);
            Explorers[0].avatar[0] = Ex1TextureLeft; Explorers[0].avatar[1] = Ex1TextureRight;
            Explorers[1].avatar[0] = Ex2TextureLeft; Explorers[1].avatar[1] = Ex2TextureRight;
            Explorers[2].avatar[0] = Ex3TextureLeft; Explorers[2].avatar[1] = Ex3TextureRight;
        }
        EndDrawing();
        break;
    }

    case GameScreen:
    {
    UpdateMusicStream(GameMusic);
    switch(State)
    {
        bool InputAgain;
        case GET: 
        {
            while(!WindowShouldClose())
            {
                BeginDrawing();
                ClearBackground(BackColor);
                Draw_Map_Infs();
                if (n == -1) DrawText("Click width of map", 400, 170, 30, RED);
                else DrawText("Click height of map", 390, 170, 30, RED);

                int click = Get_Map_Infs();
                if (click != -1) {if (n == -1) n = click; else if (m == -1) m = click;}
    
                EndDrawing();

                if (m != -1 && n != -1) break;
            }
            SET_Map_Array(map, m, n);
            
            Vector2 E;     // Gets coordinates of elements and save it as a vector2
            WallPro W;    // Gets coordinates of walls and save it as a vector2
            StartPoint = GET_StartPoint(m, n, WidthHintBox);    // The upper-left corner of the map

        // Reads the number of charcters.
            int MaxPlayer = 0;
            if (m*n < 42) MaxPlayer = 1;
            else if (m*n < 90) MaxPlayer = 2;
            else if (m*n <145) MaxPlayer = 3;
            while (nExplorers==0) {
                Get_Explorer_Count_UI(MaxPlayer);
                nExplorers = Get_Explorer_Count();  // default = 0
            }
            int MaxInterimWall = Calculate_Max_Interim_Wall(m, n);
            for (int i=0; i<nExplorers; i++) 
                Explorers[i] = (Explorer) {true, 1, (Vector2){0,0}, (Vector2){0,0}, MaxInterimWall, '\0', {ExTextures[i][0], ExTextures[i][1]}}; // initial explorers
            nShadowCasters = MaxPlayer;

        // Reads lightcore position. Accepts only coordinates in the range 0..n-1 (x) and 0..m-1 (y).
            Lightcore.x = (float) (rand()%n); Lightcore.y = (float) (rand()%m);
            Lightcore = Return_Elements_Position(Lightcore);

            int numberExNow = 0, numberShNow = 0;
        // Reads every coordinate of explorers. Only in the range 0..n-1 (x) and 0..m-1 (y).
            for (int i=0; i<nExplorers; i++)
            {
                InputAgain = true;
                do
                {
                    E.x = (float) (rand()%n); E.y = (float) (rand()%m); 
                    if (Check_Elements(E, numberExNow, numberShNow) && Distance_Check(E, Lightcore, Explorers, numberExNow, ShadowCasters, numberShNow)) 
                    {
                        InputAgain = false;
                        Explorers[i].mapPos = Return_Elements_Position(E);
                        Explorers[i].winPos = GET_Start_Elements_Position_for_Draw(StartPoint, Explorers[i].mapPos);
                        numberExNow ++;
                    }
                } while (InputAgain);
            }

        // Reads every coordinate of shadowcaster. Only int the range 0..n-1 (x) and 0..m-1 (y).
            for (int i=0; i<nShadowCasters; i++)
            {
                InputAgain = true;
                do
                {
                    E.x = (float) (rand()%n); E.y = (float) (rand()%m);
                    if (Check_Elements(E, numberExNow, numberShNow) && Distance_Check(E, Lightcore, Explorers, numberExNow, ShadowCasters, numberShNow)) 
                    {
                        InputAgain = false;
                        ShadowCasters[i] = Return_Elements_Position(E);
                        ShadowCastersP[i] = GET_Start_Elements_Position_for_Draw(StartPoint, ShadowCasters[i]);
                        numberShNow ++;
                    }
                } while (InputAgain);
            }
            if (Init_FadeSh)
            {
                Initializing_FadeSh();
                Init_FadeSh = false;
            }
        
        // Get wall count:
            char str[5];
            int inp; 
            int lenCounter=0;
            while (!WindowShouldClose())
            {
                inp = GetCharPressed();
                while (inp > 0) {
                    if (inp>='0' && inp<='9') {
                        str[lenCounter] = (char) inp;
                        lenCounter ++;
                        str[lenCounter] = '\0';
                    }
                    inp = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && lenCounter>0) {
                    lenCounter --;
                    str[lenCounter] = '\0';
                }
                BeginDrawing();
                ClearBackground(BackColor);
                Draw_Walls_Infs(str, n, m);
                EndDrawing();
                if (Submit_Button()) {
                    int sum=0;
                    for (int i=0; i<lenCounter; i++) {
                        sum *= 10;
                        sum += ((int) str[i] - '0');
                    } 
                    if (sum>=0 && sum<=((m-1)*(n-1))) {nWalls = sum; break;}
                }
            }

        // Randomly places valid walls while preserving full map connectivity (BFS-validated)
            WallPro Wall;
            bool InputWall;
            for (int i=0; i<nWalls; i++)
            {
                InputAgain = true;
                do
                {
                    Vector2 W;
                    InputWall = true;
                    do
                    {
                        Wall = Put_Wall(m, n);
                        if (Check_Walls(Wall))
                        {
                            W = SET_Walls(Wall);
                            InputWall = false;
                        }
                    } while (InputWall);
                    Reset_Map_Blocks_for_BFS(m, n);
                    int BlocksA[3*n][2];
                    int BlocksB[3*n][2];
                    int ACount = 1;
                    int BCount = 0;
                    BlocksA[0][0] = 1; BlocksA[0][1] = 1;
                    int Checked = 1;
                    BFS_Check('A', BlocksA, ACount, BlocksB, BCount, &Checked);
                    if (Checked == m*n)
                    {
                        InputAgain = false;
                    }
                    else 
                    {
                        map[(int)W.y][(int)W.x] = 1;
                    }
                } while (InputAgain);
            }
            Reset_Map_Blocks_for_Move_Elements(m, n);   //بسیار مهم برای قسمت MoveShs

            Number_Gifts(m, n); int i;
            for (i=0; i<nGifts; i++) {
                InputAgain = true;
                do {
                    int y, x;
                    do {
                        y = 2*(rand()%m) + 1; 
                        x = 2*(rand()%n) + 1;
                    } while(map[y][x] != 1);

                    int checked[m*n][2]; checked[0][0] = y; checked[0][1] = x;
                    Reset_Map_Blocks_for_Move_Elements(m, n); map[checked[0][0]][checked[0][1]] = 0;
                    int re = BFS_Gift(checked, 0, 0, m, n, 0);
                    
                    if (re) {
                        InputAgain = false;
                        Gifts[i].isGotten = false; Gifts[i].mapPos.y = (float)y; Gifts[i].mapPos.x = (float)x;
                        Gifts[i].winPos = GET_Start_Elements_Position_for_Draw(StartPoint, Gifts[i].mapPos);
                        Gifts[i].type = rand()%4;
                    }
                } while (InputAgain);
            }
            Reset_Map_Blocks_for_Move_Elements(m, n); //بسیار مهم برای قسمت MoveShs
            

            
            State = MoveExs;
            break;
        }

        // Next phase. Move and drawing characters.         
            case MoveExs: { 
                Check_Life_of_Interim_Walls();
                bool notChoosed = true;

            //for "get wall"
                bool GWall = false;                
                int  LockinRec = 0;
                
            //for "move"
                bool Move = false;
                char ExTextureDir;
                char ExMoveDir;
                double t0;
                bool ShouldShowError = false;
                bool ShouldMove = false;
                bool WitchRound = true;
                int l=0;

                if (!Are_All_Players_Dead())
                while (!WindowShouldClose() && l<nExplorers) {

                    if (IsKeyPressed(KEY_O)) Force_Shc(StartPoint, m, n, GameMusic, Round, l, ForceSound);
                    if (IsKeyPressed(KEY_P)) {Earthquake_Gift(m, n, StartPoint, Round, GameMusic, l, EarthquakeSound);}
                               
                    if (notChoosed) {
                        if (!Explorers[l].isAlive) {l ++; continue;}
                        UpdateMusicStream(GameMusic);
                        BeginDrawing();
                        DrawText("   To save: F1\n   To load: F2\n  Ch theme: F3", 915, 220, 20, (Color){205,50,0,255});
                        if (IsKeyPressed(KEY_F1)) Save_Game(Round, l, m, n, StartPoint);
                        if (IsKeyPressed(KEY_F2)) {
                            BeginDrawing();
                            Load_Game(&m, &n, &l, &Round, &StartPoint);
                            ClearBackground(BackColor);
                            Draw_Map(0, StartPoint, m, n, Round, l);
                            EndDrawing();
                        }
                        if (IsKeyPressed(KEY_F3)) {if (BackColor.g == 245) BackColor = (Color){170, 175, 180, 255}; else BackColor = RAYWHITE;}
                        if (IsKeyPressed(KEY_E)) {
                            if (Explorers[l].wallCount == 0) {
                                double t = GetTime();
                                while (!WindowShouldClose() && GetTime()-t <= 2.2 && !IsKeyPressed(KEY_W)
                                        && !IsKeyPressed(KEY_D) && !IsKeyPressed(KEY_S) && !IsKeyPressed(KEY_A)) {
                                    UpdateMusicStream(GameMusic);
                                    Show_Ended_Walls_Error(StartPoint, m, n, Round, l);
                                }
                            } else {
                                LockinRec = 0;
                                GWall = true;
                                notChoosed = false;
                            }
                        }
                        if (IsKeyPressed(KEY_W)) {
                            ExMoveDir = 'W';
                            ShouldMove = true; 
                            t0 = GetTime(); 
                            ShouldShowError = false;
                            notChoosed = false;
                        }      
                        if (IsKeyPressed(KEY_D)) { 
                            ExMoveDir = 'D'; 
                            ExTextureDir = 'R'; 
                            ShouldMove = true; 
                            t0 = GetTime(); 
                            ShouldShowError = false;
                            notChoosed = false;
                        }    
                        if (IsKeyPressed(KEY_S)) { 
                            ExMoveDir = 'S'; 
                            ShouldMove = true; 
                            t0 = GetTime(); 
                            ShouldShowError = false;
                            notChoosed = false;
                        }   
                        if (IsKeyPressed(KEY_A)) {
                            ExMoveDir = 'A'; 
                            ExTextureDir = 'L'; 
                            ShouldMove = true; 
                            t0 = GetTime(); 
                            ShouldShowError = false;
                            notChoosed = false;
                        }          
                        if (ShouldMove && Can_Ex_Move_for_Walls(Explorers[l].mapPos, ExMoveDir)) {
                            PlaySound(MovingSound);
                            Explorers[l].mapPos = Move_Element(Explorers[l].mapPos, ExMoveDir);
                            Vector2 EndPosition = GET_Start_Elements_Position_for_Draw(StartPoint, Explorers[l].mapPos);       
                            float Speed = 2.0f;
                            Exs_Animation(ExMoveDir, ExTextureDir, &Explorers[l].winPos, EndPosition, Speed, 0.1f, StartPoint, m, n, l, Round, GameMusic);
                            WitchRound = true; l++; Move = false; notChoosed = true; ShouldMove = false;

                            // Witch player is dead?
                            Check_Witch_Player_is_Dead(Round, DieSound);
                    
                            UpdateMusicStream(GameMusic);
                            ClearBackground(BackColor); // اگه خودش رفت تو سایه نگر، مردنش نمایش داده شه
                            Draw_Map(0, StartPoint, m, n, Round, l);
                            
                            int WhichGift = Is_Present_Gotten();
                            if (WhichGift) {
                                PlaySound(GiveGiftSound);
                                Gifts[WhichGift-1].isGotten = true;
                                bool isShown = false;
                                Show_Present_Rec(StartPoint, m, n, Round, l, GameMusic);
                                do {
                                    Show_Present(StartPoint, m, n, Round, l, Gifts[WhichGift-1].type, GameMusic);
                                    if (IsKeyPressed(KEY_SPACE)) isShown = true;
                                } while (!isShown); // Shows the present box while plyer do not click space.

                                
                                if (Gifts[WhichGift-1].type == Replay) {int *p; p = &(l); ReplayGift(p);}
                                else if (Gifts[WhichGift-1].type == InWallIncrease) InWallIncreaseGift(l-1);
                                else if (Gifts[WhichGift-1].type == ForceEnemy) {
                                    Force_Shc(StartPoint, m, n, GameMusic, Round, l, ForceSound);
                                    Check_Witch_Player_is_Dead(Round, DieSound);                                 
                                    
                                    UpdateMusicStream(GameMusic);
                                    ClearBackground(BackColor);
                                    Draw_Map(0, StartPoint, m, n, Round, l); //اگه هل داده شد توی سایه نگر، مردنش نمایش داده شه
                                }
                                else if (Gifts[WhichGift-1].type == Earthquake) Earthquake_Gift(m, n, StartPoint, Round, GameMusic, l, EarthquakeSound);
                            }
                        }
                        else if (ShouldMove && !(Can_Ex_Move_for_Walls(Explorers[l].mapPos, ExMoveDir))) { 
                            ShouldShowError = true;
                            ShouldMove = false;notChoosed = true;
                        }           
                        while (ShouldShowError) {
                            ShouldMove = false; notChoosed = true; 
                            if (!WindowShouldClose() && GetTime()-t0 <= 1.8) {
                                UpdateMusicStream(GameMusic);
                                Show_Invalid_Move_Error(StartPoint, m, n, Round, l);
                            }
                            else {ShouldShowError = false; break;}
                            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_S) ||
                            IsKeyPressed(KEY_W) || IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_E))
                                {ShouldShowError = false;}
                        }
                        if (IsKeyPressed(KEY_Q)) {
                            l++;
                            notChoosed = true;
                            WitchRound = true;
                        }
                        
                        ClearBackground(BackColor);
                        Pointer_To_Player(l, StartPoint);
                        Draw_Map(0, StartPoint, m, n, Round, l);
                        EndDrawing();
                    
                        for (int i=0; i<nExplorers; i++)
                            if (Explorers[i].isAlive && Explorers[i].mapPos.x==Lightcore.x && Explorers[i].mapPos.y==Lightcore.y)
                                Win_Explorer(i, VictorySound, Round);
                    }

                    if (GWall) {
                        UpdateMusicStream(GameMusic);
                        BeginDrawing();
                        ClearBackground(BackColor);
                        Draw_Map(0, StartPoint, m, n, Round, l);
                        Rectangle Recs[m][n];
                        int i, j;
                        for (j=1; j<2*m+1; j+=2) {
                            for (i=1; i<2*n+1; i+=2) {                        
                                Vector2 A = {i, j};
                                Vector2 B = GET_Start_Elements_Position_for_Draw(StartPoint, A);
                                B.x -= 2; B.y -= 3; //مربوط به تابع GET_Elements_Position_for_Draw
                                Rectangle R = {B.x, B.y, Side, Side};
                                Recs[(j-1)/2][(i-1)/2] = R;
                            }
                        }
                        Vector2 Mouse;
                        Mouse = GetMousePosition();                                                       
                        int tempi, tempj;
                        Rectangle tempDis;
                        Rectangle tempRforChoose[4];

                        for (j=0; j<m; j++) {
                            for (i=0; i<n; i++) {
                                Rectangle Dis;

                                if (j == 0) {
                                    Rectangle temp = {Recs[j][i].x-70, Recs[j][i].y+50, 140+Side, 50};
                                    Dis = temp;
                                } else {
                                    Rectangle temp = {Recs[j][i].x-70, Recs[j][i].y-50, 140+Side, 50};
                                    Dis = temp;
                                }

                                SidesAR A = CheckSides((2*j+1), (2*i+1));
                                Rectangle RforChoose[4];
                                Rec_for_Choose((Dis.x+(Side/2-5)), Dis.y+10, A, RforChoose);

                                if (LockinRec == 0) {
                                    int re = Show_Allowable_Walls(Recs[j][i], Dis, RforChoose, Mouse);
                                    if (re == 1) {
                                        LockinRec = 1;
                                        tempi = i;
                                        tempj = j;
                                        tempDis = Dis;
                                        int k;
                                        for (k=0; k<4; k++) {
                                            tempRforChoose[k] = RforChoose[k];
                                        }
                                    }
                                }                                       
                            }
                        }                        
                        if (LockinRec==1) {
                            int re = Lock_in_Rectangle(Recs[tempj][tempi], tempDis, tempRforChoose, tempj, tempi, l, Mouse);
                            if (re == 1) {
                                WitchRound = true;
                                GWall = false;
                                notChoosed = true;
                                l++;
                                LockinRec=0;
                            }
                            if (!WindowShouldClose() && IsKeyPressed(KEY_Q)) {
                            LockinRec=0;
                            BeginDrawing(); EndDrawing();
                            }
                        }

                        if (IsKeyPressed(KEY_Q)) {
                                notChoosed = true;
                                GWall = false;
                        }  
                        EndDrawing();    //اگه این نمیبود، با فشردن Q هم از این بخش میرفت بیرون و هم از انتخاب کردن
                    }
                        
                }
                if (Are_All_Players_Have_Won()) {Current = EndScreen; break;}  
                else {State = MoveShs; break;}
            }
            
            case MoveShs: {
                
                l = nExplorers-1;
                Vector *BFSlistChecked;
                int BFSListCounter;
                Vector **WayShcArray; 
                int WayShcCounter[nShadowCasters];
                do {
                    WayShcArray = malloc(nShadowCasters*sizeof(Vector *)); 
                } while (!WayShcArray);
                int k;
                for (k=0; k<nShadowCasters; k++) {
                    do {
                        WayShcArray[k] = malloc((m*n)*sizeof(Vector));
                    } while (!WayShcArray[k]);
                }
                
                int i;
                for (i=0; i<nShadowCasters; i++) {
                    PlaySound(ShadowSound);
                    do{
                        BFSlistChecked = malloc((m*n)*sizeof(Vector));
                    }while (!BFSlistChecked);
                    BFSListCounter = 1;
                    (*BFSlistChecked).y = (int)ShadowCasters[i].y; (*BFSlistChecked).x = (int)ShadowCasters[i].x; (*BFSlistChecked).beg = '\0';
                    WayShcCounter[i] = 0;
                    Reset_Map_Blocks_for_Move_Elements(m, n);
                    map[(int)ShadowCasters[i].y][(int)ShadowCasters[i].x] = 0;
                    ShadowCastersDir[i] = BFS_Way(BFSlistChecked, BFSlistChecked, &BFSListCounter, &WayShcCounter[i], WayShcArray[i]);
                    
                    if (WayShcCounter[i]) {
                        Reset_Map_Blocks_for_Move_Elements(m, n);
                        WayShcCounter[i] = Find_Way(WayShcArray[i], BFSListCounter, WayShcCounter[i], BFSlistChecked, WayShcArray[i]);
                        if (WayShcCounter[i]<3) {
                            Vector2 w; w = Wall_Coordinate((WayShcArray[i]+WayShcCounter[i]-1-1));
                            if (map[(int)w.y][(int)w.x] == 1) {
                                float Speed = 0.2f;
                                Vector2 End; Vector2 EndPosition;
                                End.x = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).x; End.y = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).y;
                                EndPosition = GET_Start_Elements_Position_for_Draw(StartPoint, End);

                                int tempShDir = ShadowCastersDir[i];
                                char Dir = (*(WayShcArray[i]+WayShcCounter[i]-1-1)).beg;
                                Shcs_Animation(Dir, &ShadowCastersP[i], EndPosition, Speed, 0.2f, StartPoint, m, n, i, Round, GameMusic);   

                                ShadowCastersDir[i] = tempShDir;
                                ShadowCasters[i].y = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).y; // = *(*(WayShcArray+i)+WayShcCounter)
                                ShadowCasters[i].x = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).x;
                            }
                        } else {
                            int tempx1, tempy1, tempx2, tempy2; Vector2 w1, w2;
                            tempx1 = (*(WayShcArray[i]+WayShcCounter[i]-1-1)).x; tempy1 = (*(WayShcArray[i]+WayShcCounter[i]-1-1)).y;
                            tempx2 = (*(WayShcArray[i]+WayShcCounter[i]-1-1-1)).x; tempy2 = (*(WayShcArray[i]+WayShcCounter[i]-1-1-1)).y;
                            w1 = Wall_Coordinate((WayShcArray[i]+WayShcCounter[i]-1-1)); w2 = Wall_Coordinate((WayShcArray[i]+WayShcCounter[i]-1-1-1));
                            if ((map[(int)w1.y][(int)w1.x] == 1 && map[(int)w2.y][(int)w2.x] == 1) && !(map[tempy2][tempx2] == 3)) {
                                int j;
                                int tempShDir = ShadowCastersDir[i];
                                for (j=1; j<=2; j++) {
                                    float Speed = 0.2f;
                                    Vector2 End; Vector2 EndPosition;
                                    End.x = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-j)).x; End.y = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-j)).y;
                                    EndPosition = GET_Start_Elements_Position_for_Draw(StartPoint, End);

                                    char Dir = (*(WayShcArray[i]+WayShcCounter[i]-1-j)).beg;
                                    Shcs_Animation(Dir, &ShadowCastersP[i], EndPosition, Speed, 0.2f, StartPoint, m, n, i, Round, GameMusic);

                                    if (j==1) {
                                        ShadowCasters[i].y = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).y; ShadowCasters[i].x = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).x;
                                    } else {
                                        ShadowCastersDir[i] = tempShDir;
                                        ShadowCasters[i].y = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1-1)).y; ShadowCasters[i].x = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1-1)).x;
                                    }
                                    
                                }                                
                            }
                            else if (map[(int)w1.y][(int)w1.x] == 1 && !(map[tempy1][tempx1] == 3)) {
                                float Speed = 0.2f;
                                Vector2 End; Vector2 EndPosition;
                                End.x = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).x; End.y = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).y;
                                EndPosition = GET_Start_Elements_Position_for_Draw(StartPoint, End);

                                int tempShDir = ShadowCastersDir[i];
                                char Dir = (*(WayShcArray[i]+WayShcCounter[i]-1-1)).beg;
                                Shcs_Animation(Dir, &ShadowCastersP[i], EndPosition, Speed, 0.2f, StartPoint, m, n, i, Round, GameMusic);

                                ShadowCastersDir[i] = tempShDir;
                                ShadowCasters[i].y = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).y; ShadowCasters[i].x = (float)(*(WayShcArray[i]+WayShcCounter[i]-1-1)).x;                                 
                            }
                        }                      
                    }
                    // Witch player is dead?
                    Check_Witch_Player_is_Dead(Round, DieSound);
                    UpdateMusicStream(GameMusic);
                    BeginDrawing();
                    ClearBackground(BackColor);
                    Draw_Map(0, StartPoint, m, n, Round, l); // اگه سایه نگر کشتش، مردنش نمایش داده شه
                    EndDrawing();

                    free(BFSlistChecked);
                }
                Reset_Map_Blocks_for_Move_Elements(m, n);

            // free mallocs
                for (k=0; k<nShadowCasters; k++) {
                    free(WayShcArray[k]);
                }
                free(WayShcArray);

            // Are all players have died?
                int x = 0;
                for (int i=0; i<nExplorers; i++) {if (Explorers[i].isAlive) x ++;} 
                if (x == 0) {Current = EndScreen; TimeEnding = GetTime(); break;}

                Round++;
                State = MoveExs;
                break;
            }

        }
        break;
    }

    case EndScreen: 
    {
        PlayMusicStream(EndGameMusic);
        bool showResult;
        while (GetTime() - TimeEnding <= 2.5) {
            showResult = false;
        }
        showResult = true;
        if (showResult)
            while (!WindowShouldClose()) {
                UpdateMusicStream(EndGameMusic);
                BeginDrawing();
                ClearBackground(BackColor);
                Show_End_Screen();
                EndDrawing();
            }




        break;
    }
}

}
CloseAudioDevice(); CloseWindow();
UnloadTexture(Ex1Image); UnloadTexture(Ex3Image); UnloadTexture(Ex3Image);
UnloadTexture(Sh1TextureRight); UnloadTexture(Sh2TextureRight); UnloadTexture(Sh3TextureRight);
UnloadTexture(Ex1TextureRight); UnloadTexture(Ex2TextureRight); UnloadTexture(Ex3TextureRight);
UnloadTexture(Sh1TextureLeft); UnloadTexture(Sh2TextureLeft); UnloadTexture(Sh3TextureLeft);
UnloadTexture(Ex1TextureLeft); UnloadTexture(Ex2TextureLeft); UnloadTexture(Ex3TextureLeft);
UnloadTexture(LiTexture); UnloadTexture (PresentTexture);
UnloadMusicStream(GameMusic); UnloadMusicStream(music1); UnloadMusicStream(music2); UnloadMusicStream(music3);
UnloadSound(DieSound); UnloadSound(VictorySound); UnloadSound(MovingSound); UnloadSound(ShadowSound); 
UnloadSound(GiveGiftSound); UnloadMusicStream(EndGameMusic);

return 0;

}