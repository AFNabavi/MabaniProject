// TODO: 
// 0. write a function for Explorer moving (line 240 to 260).
//  1. fix shadow caster based on the closest explorer in direction of sh func (wall)
//  2. amend hint box

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "funcs.h"

Texture2D Sh1TextureRight; Texture2D Sh2TextureRight; Texture2D Sh3TextureRight;
Texture2D Ex1TextureRight; Texture2D Ex2TextureRight; Texture2D Ex3TextureRight;
Texture2D Sh1TextureLeft; Texture2D Sh2TextureLeft; Texture2D Sh3TextureLeft;
Texture2D Ex1TextureLeft; Texture2D Ex2TextureLeft; Texture2D Ex3TextureLeft;
Texture2D LiTexture;

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

int main(void)
{
InitWindow(WindowWidth, WindowHeight, "The Tale of the Labyrinth");
InitAudioDevice();
srand(time(NULL));    // randomize choices

// Load textures
Sh1TextureRight = LoadTexture("source\\shadowcaster1_right_image.png");
Sh2TextureRight = LoadTexture("source\\shadowcaster2_right_image.png");
Sh3TextureRight = LoadTexture("source\\shadowcaster3_right_image.png");
Ex1TextureRight = LoadTexture("source\\explorer1_right_image.png");
Ex2TextureRight = LoadTexture("source\\explorer2_right_image.png");
Ex3TextureRight = LoadTexture("source\\explorer3_right_image.png");
Sh1TextureLeft = LoadTexture("source\\shadowcaster1_left_image.png");
Sh2TextureLeft = LoadTexture("source\\shadowcaster2_left_image.png");
Sh3TextureLeft = LoadTexture("source\\shadowcaster3_left_image.png");
Ex1TextureLeft = LoadTexture("source\\explorer1_left_image.png");
Ex2TextureLeft = LoadTexture("source\\explorer2_left_image.png");
Ex3TextureLeft = LoadTexture("source\\explorer3_left_image.png");
LiTexture = LoadTexture("source\\light_core_image.png");
Music music = LoadMusicStream("source\\main_music.ogg");
Sound VictorySound = LoadSound("source\\victory_sound.wav");
Sound GameOverSound = LoadSound("source\\game_over_sound.wav");

Screen Current = TitleScreen;   // Current screen, starts at TitleScreen.
Level State = GET;   // Current level, starts at GET for inputs.

PlayMusicStream(music);
SetTargetFPS(FPS);

int m=-1, n=-1, nWalls;
Vector2 StartPoint;

int FPScounter = 2*FPS; //That is for show TitleNote3

while (!WindowShouldClose())
{
switch(Current)
{
    case TitleScreen:
    {
        // First phase, click on the screen to start the game
        Rectangle TitleRec = {(WindowWidth-450)/2, (WindowHeight- 180)/2, 450, 180};
        Color TitleColorNotes = { 112, 31, 126, 255};
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRoundedLinesEx(TitleRec, 0.4f, 25, 4, TitleColorNotes);
        int TitleNote1 = MeasureText("Click me to play", 40);
        int TitleNote2 = MeasureText("Explorer Game!", 50);
        DrawText("Click me to play", TitleRec.x+(TitleRec.width-TitleNote1)/2, TitleRec.y+40, 40, TitleColorNotes);
        DrawText("Explorer Game!", TitleRec.x+(TitleRec.width-TitleNote2)/2, TitleRec.y+40+50, 50, TitleColorNotes);
        Vector2 Mous = GetMousePosition();
        if (CheckCollisionPointRec(Mous, TitleRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ShowTitleNote3 = true;
        if (ShowTitleNote3)
        { 
            PlayMusicStream(music);
            int TitleNote3 = MeasureText("now input game details.", 20);
            DrawText("now input game details.", TitleRec.x+(TitleRec.width-TitleNote3)/2, TitleRec.y+40+50+100, 20, RED);
            FPScounter --;
            if(FPScounter<0) Current = GameScreen;
        }

        EndDrawing();
        break;
    }

    case GameScreen:
    {
    UpdateMusicStream(music);
    switch(State)
    {
        bool InputAgain;
        case GET: 
        {
        while(!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Draw_Map_Infs();
            if (n == -1) DrawText("Click width of map", 400, 170, 30, RED);
            else DrawText("Click height of map", 390, 170, 30, RED);
            
            int click = Get_Map_Infs();
            if (click != -1) {if (n == -1) n = click; else if (m == -1) m = click;}
            
            EndDrawing();

            if (m != -1 && n != -1) break;
        }
        // printf("%d %d\n", m, n);
            // Reads height and width of the map of game
            // InputAgain = true;    // Flag for re-input in validation loops.
            // do 
            // {
            //     printf("\nEnter width and height of map (between 5 and 12): ");
            //     scanf("%d %d", &m, &n);    // m: height map, n: width map
            //     if ((m<=12 && m>=5) && (n<=12 && n>=5)) InputAgain = false; 
            //     else printf("Pay attention to limits! try again.");
            // } while (InputAgain);
            SET_Map_Array(map, m, n);
            
            Vector2 E;    // Gets coordinates of elements and save it as a vector2
            WallPro W;    // Gets coordinates of walls and save it as a vector2
            StartPoint = GET_StartPoint(m, n, WidthSpace);    // The upper-left corner of the map

        // // Reads the number of explorers. Only in the range 1 and 3.
        //     InputAgain = true;
        //     do
        //     {
        //         printf("\nEnter explorer(s) number (between 1 and 3): ");
        //         scanf("%d", &nExplorers);
        //         if (nExplorers>=1 && nExplorers<=3) InputAgain = false;
        //         else printf("Pay attention to limits! try again.");
        //     } while (InputAgain);
        
            nExplorers = 1; 

        // // Reads the number of shadowcasters. Only in the range 1 and 3.
        //     InputAgain = true;
        //     do 
        //     {
        //         printf("\nEnter shadowcaster(s) number (between 1 and 3): ");
        //         scanf("%d", &nShadowCasters);
        //         if (nShadowCasters>=1 && nShadowCasters<=3) InputAgain = false;
        //         else printf("Pay attention to limits! Try again.");
        //     } while (InputAgain);
            nShadowCasters = 1;

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
                        Explorers[i] = Return_Elements_Position(E);
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
                        numberShNow ++;
                    }
                } while (InputAgain);
            }
            if (Init_FadeSh)
            {
                Initializing_FadeSh();
                Init_FadeSh = false;
            }
            

        // // Reads the number of walls. Only int the range 0 and (m-1)*(n-1).
        //     InputAgain = true;
        //     do
        //     {
        //         printf("\nEnter wall(s) number (between 0 and %d): ", (m-1)*(n-1));
        //         scanf("%d", &nWalls);
        //         if (nWalls>=0 && nWalls<=((m-1)*(n-1))) InputAgain = false;
        //         else printf("Pay attention to limits! Try again. ");
        //     } while (InputAgain);

        char s[5];
        int tempN;
        while (!WindowShouldClose())
            {
                BeginDrawing();
                ClearBackground(RAYWHITE);
                Draw_Walls_Infs(s, n, m);
    
                char inp = GetKeyPressed();
                if ((inp>='0' && inp<='9') || inp == 'r' || inp == 'R') {Print_Number_In_String(s, inp, strlen(s));}
                tempN = StoI(s, strlen(s));
                if (Submit_Button(n, m, s, strlen(s))) {nWalls = tempN; break;}
                EndDrawing();
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

            State = MoveExs;
            break;
        }

// //----------------------------------------------------------------------------------------------------------------------------
//     // UPDATE   
//         // width of map:
//         InfInputs = GetKeyPressed();
//         InfInputs = strtol()
//         if (InfInputs > 0 && InfInputs <= 12) n = InfInputs;

//         // height of map:
//         if (InfInputs > 0 && InfInputs <= 12) m = InfInputs;

//         // walls count:
//         if (InfInputs >= 0 && InfInputs <= (n-1)*(m-1)) nWalls = InfInputs;

// //----------------------------------------------------------------------------------------------------------------------------
//     // DRAWING  
//         BeginDrawing();
//         ClearBackground(RAYWHITE);
//         DrawRectangleRec(PopupRec, YELLOW);
//         EndDrawing();

        // Next phase. Move and drawing characters.         
            char ExTextureDir = 'R';  // L = left  ,  R = right
            char ExMoveDir;
            double t0;
            bool ShouldShowError = false;
            case MoveExs: 
            {
                // UPDATE
            // --------------------------------------------------------------------------------------------------------------------------
                Vector2 NewPos = GET_Start_Elements_Position_for_Draw(StartPoint, Explorers[0]);
                bool ShouldMove = false;
                if (IsKeyPressed(KEY_W))   {ExMoveDir = 'W'; ShouldMove = true; ShouldShowError = false; t0 = GetTime();}
                if (IsKeyPressed(KEY_S))    {ExMoveDir = 'S'; ShouldMove = true; ShouldShowError = false; t0 = GetTime();}
                if (IsKeyPressed(KEY_A))    {ExMoveDir = 'A'; ExTextureDir = 'L'; ShouldMove = true; ShouldShowError = false; t0 = GetTime();}
                if (IsKeyPressed(KEY_D))    {ExMoveDir = 'D'; ExTextureDir = 'R'; ShouldMove = true; ShouldShowError = false; t0 = GetTime();}

                if (ShouldMove && Can_Ex_Move_for_Walls(Explorers[0], ExMoveDir)) 
                    {Explorers[0] = Move_Element(Explorers[0], ExMoveDir); State = MoveShs;}
                else if (ShouldMove && !(Can_Ex_Move_for_Walls(Explorers[0], ExMoveDir))) 
                    ShouldShowError = true;

                if (ShouldMove && !Win_or_Lose(Explorers[0], ShadowCasters, nShadowCasters, Lightcore)) 
                    {Current = EndScreen; Win = false; break;}
                else if (ShouldMove && Win_or_Lose(Explorers[0], ShadowCasters, nShadowCasters, Lightcore) == 1)
                    {Current = EndScreen; Win = true; break;}
            // --------------------------------------------------------------------------------------------------------------------------

                // DRAWING
            // --------------------------------------------------------------------------------------------------------------------------
                BeginDrawing();
                ClearBackground(RAYWHITE);
                Draw_Map(StartPoint, m, n);
                
                if (ExTextureDir == 'R') DrawTexture(Ex1TextureRight, NewPos.x, NewPos.y, WHITE);
                else DrawTexture(Ex1TextureLeft, NewPos.x, NewPos.y, WHITE);

                if (ShouldShowError) if (GetTime() - t0 <= 1.8) DrawText("\nYou can't go there. Pay attention to walls!", 300, 50, 24, RED);
                Draw_Hint_Box();
                EndDrawing();
            // --------------------------------------------------------------------------------------------------------------------------
                break;
            }

            case MoveShs: {
                
                ShcMoveData ShM[nShadowCasters];
                int i;
                for (i=0; i<nShadowCasters; i++) {
                    ShM[i].Length = 200; ShM[i].FM = '\0'; ShM[i].SM = '\0';
                    int j;
                    for (j=0; j<nExplorers; j++) {
                        Reset_Map_Blocks_for_Move_Elements(m, n);
                        minlength = 200;
                        map[(int)Explorers[j].y][(int)Explorers[j].x] = 1;
                        Move_Shcs(Explorers[j], ShadowCasters[i].y, ShadowCasters[i].x, 0, '\0', '\0');
                        printf(" Ex: %.0f %.0f\nMinLenght: %d  ", Explorers[j].x, Explorers[j].y, minlength);
                        if(minlength<ShM[i].Length) {
                            ShM[i].Length = minlength;
                            ShM[i].FM = FirstMove;
                            ShM[i].SM = SecondMove;
                            ShM[i].indexEx = j;
                        }
                    }map[(int)Explorers[j].y][(int)Explorers[j].x] = 2;
                    Set_Move_of_Sh_in_Map(ShM[i], i);
                              if (!Win_or_Lose(Explorers[0], ShadowCasters, nShadowCasters, Lightcore)) 
                    {Current = EndScreen; Win = false; break;}
                }
                // int delay = 2*FPS;
                State = MoveExs;
                break;
            }

        }
        break;
    }

    bool HasBeenPlayed = false;
    case EndScreen: 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (Win == false) {DrawText("GAME OVER!", 442, 305, 40, RED); if (!HasBeenPlayed) {PlaySound(GameOverSound); HasBeenPlayed = true;}}
        else {DrawText("VICTORY!", 466, 305, 40, RED); if (!HasBeenPlayed) {PlaySound(VictorySound); HasBeenPlayed = true;}}
        EndDrawing();
        break;
    }
}
if (Current == GameScreen && State != GET) Fade_ShadowCasters();
}

CloseAudioDevice(); CloseWindow();
UnloadTexture(LiTexture);
UnloadTexture(Sh1TextureRight); UnloadTexture(Sh2TextureRight); UnloadTexture(Sh3TextureRight);
UnloadTexture(Ex1TextureRight); UnloadTexture(Ex2TextureRight); UnloadTexture(Ex3TextureRight);
UnloadTexture(Sh1TextureLeft); UnloadTexture(Sh2TextureLeft); UnloadTexture(Sh3TextureLeft);
UnloadTexture(Ex1TextureLeft); UnloadTexture(Ex2TextureLeft); UnloadTexture(Ex3TextureLeft);
UnloadMusicStream(music); UnloadSound(GameOverSound); UnloadSound(VictorySound);

return 0;
}