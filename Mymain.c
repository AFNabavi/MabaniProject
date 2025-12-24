// TODO: 
//  1. fix shadow caster direction between two explorer (wall)
//  2. dfs/bfs algorithm
//  3. random (distance between explorer and lightcore >= 2, shdaowcaster and lightcore >= 2, explorer and shadowcaster >= 2)
//  4. more conditions (2 home distance)

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
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
bool EndGame = false;   // Flag for game end, transitions to EndScreen.
bool Win = false;   // Flag for player win, affects EndScreen.
bool InputAgain;    // Flag for re-input in validation loops.

int main(void)
{
InitWindow(WindowWidth, WindowHeight, "The Tale of the Labyrinth");
InitAudioDevice();
srand(time(NULL));    // randomize choices

// Load textures
Sh1TextureRight = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\shadowcaster1_right_image.png");
Sh2TextureRight = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\shadowcaster2_right_image.png");
Sh3TextureRight = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\shadowcaster3_right_image.png");
Ex1TextureRight = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\explorer1_right_image.png");
Ex2TextureRight = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\explorer2_right_image.png");
Ex3TextureRight = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\explorer3_right_image.png");
Sh1TextureLeft = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\shadowcaster1_left_image.png");
Sh2TextureLeft = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\shadowcaster2_left_image.png");
Sh3TextureLeft = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\shadowcaster3_left_image.png");
Ex1TextureLeft = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\explorer1_left_image.png");
Ex2TextureLeft = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\explorer2_left_image.png");
Ex3TextureLeft = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\explorer3_left_image.png");
LiTexture = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\light_core_image.png");
Music music = LoadMusicStream("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\main_music.ogg");

Screen Current = TitleScreen;   // Current screen, starts at TitleScreen.
Level State = GET;   // Current level, starts at GET for inputs.

PlayMusicStream(music);
SetTargetFPS(FPS);

int m, n, nWalls;
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
            DrawText("now input game details.", TitleRec.x+(TitleRec.width-TitleNote3)/2, TitleRec.y+40+50+100, 20,RED);
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
        case GET: 
        {
        // Reads height and width of the map of game
            InputAgain = true;
            do 
            {
                printf("\nEnter width and height of map (between 5 and 12): ");
                scanf("%d %d", &m, &n);    // m: height map, n: width map
                if ((m<=12 && m>=5) && (n<=12 && n>=5)) InputAgain = false; 
                else printf("Pay attention to limits! try again.");
            } while (InputAgain);
            SET_Map_Array(map, m, n);
            
            Vector2 E;    // Gets coordinates of elements and save it as a vector2
            WallPro W;    // Gets coordinates of walls and save it as a vector2
            StartPoint = GET_StartPoint(m, n, WidthSpacing);    // The upper-left corner of the map

            int numberExNow = 0, numberShNow = 0;
        // Reads the number of explorers. Only in the range 1 and 3.
            InputAgain = true;
            do
            {
                printf("\nEnter explorer(s) number (between 1 and 3): ");
                scanf("%d", &nExplorers);
                if (nExplorers>=1 && nExplorers<=3) InputAgain = false;
                else printf("Pay attention to limits! try again.");
            } while (InputAgain);

        // Reads the number of shadowcasters. Only in the range 1 and 3.
            InputAgain = true;
            do 
            {
                printf("\nEnter shadowcaster(s) number (between 1 and 3): ");
                scanf("%d", &nShadowCasters);
                if (nShadowCasters>=1 && nShadowCasters<=3) InputAgain = false;
                else printf("Pay attention to limits! Try again.");
            } while (InputAgain);

        // Reads lightcore position. Accepts only coordinates in the range 0..n-1 (x) and 0..m-1 (y).
            E.x = rand()%n; E.y = rand()%m;
            Lightcore = Return_Elements_Position(E);
            printf("\nLightcore Pos: %.1f %.1f\n\n", E.x, E.y);

        // Reads every coordinate of explorers. Only in the range 0..n-1 (x) and 0..m-1 (y).
            for (int i=0; i<nExplorers; i++)
            {
                InputAgain = true;
                do
                {
                    E.x = rand()%n; E.y = rand()%m;
                    printf("Explorer %d Pos: %.1f %.1f\n", i, E.x, E.y);
                    if (Distance_Check(E, Lightcore, Explorers, numberExNow, ShadowCasters, numberShNow)) 
                    {
                        InputAgain = false;
                        Explorers[i] = Return_Elements_Position(E);
                        numberExNow ++;
                    }
                } while (InputAgain);
            }
            printf("\n");

        // Reads every coordinate of shadowcaster. Only int the range 0..n-1 (x) and 0..m-1 (y).
            for (int i=0; i<nShadowCasters; i++)
            {
                InputAgain = true;
                do
                {
                    E.x = rand()%n; E.y = rand()%m;
                    printf("Shadowcaster %d Pos: %.1f %.1f\n", i, E.x, E.y);
                    if (Distance_Check(E, Lightcore, Explorers, numberExNow, ShadowCasters, numberShNow)) 
                    {
                        InputAgain = false;
                        ShadowCasters[i] = Return_Elements_Position(E);
                        numberShNow ++;
                    }
                } while (InputAgain);
            }

// printf("\n%.1f %.1f\n%.1f %.1f, %.1f %.1f, %.1f %.1f\n%.1f %.1f, %.1f %.1f, %.1f %.1f\n", Lightcore.x, Lightcore.y, Explorers[0].x, Explorers[0].y, Explorers[1].x, Explorers[1].y, Explorers[2].x, Explorers[2].y, ShadowCasters[0].x, ShadowCasters[0].y, ShadowCasters[1].x, ShadowCasters[1].y, ShadowCasters[2].x, ShadowCasters[2].y);
            
            // // Reads the number of walls. Only int the range 0 and (m-1)*(n-1).
            // InputAgain = true;
            // do
            // {
            //     printf("\nEnter wall(s) number (between 0 and %d): ", (m-1)*(n-1));
            //     scanf("%d", &nWalls);
            //     if (nWalls>=0 && nWalls<=((m-1)*(n-1))) InputAgain = false;
            //     else printf("Pay attention to limits! Try again. ");
            // } while (InputAgain);

            // // Reads every coordinate of walls. Only in the range 0..n-1 (x) and 0..m-1 (y).    
            // if (nWalls>0) printf("\nFORM: y x z. y should between 0 and %d, x should between 0 and %d, z: H=horizental , V=vertecal.", m-1, n-1);
            // for (int i=0; i<nWalls; i++)
            // {
            //     InputAgain = true;
            //     do
            //     {
            //         printf("\nEnter coordinate of wall %d: ", i+1);
            //         scanf("%d %d %c", &tempy, &tempx, &W.HorV);
            //         W.Position.x = tempx;
            //         W.Position.y = tempy;
            //         if (!(W.HorV=='v' || W.HorV=='V' || W.HorV=='h' || W.HorV=='H'))
            //         {
            //             printf("Pay attention to limits! Try again. ");
            //         } 
            //         else if (!((tempx>=0 && tempx<n) && (tempy>=0 && tempy<m)))
            //         {
            //             printf("Pay attention to limits! Try again. "); 
            //         }
            //         else
            //         {
            //             int Ch = Check_Walls(W);
            //             if (Ch == 0) printf("Pay attention to limits! Try again. ");
            //             else 
            //             {
            //                 SET_Walls(W);
            //                 InputAgain = false;
            //             }
            //         }
            //     } while (InputAgain);
            // }

            State = MoveExs;
            break;
        }

        // Next phase. Move and drawing characters.     
        case MoveExs: 
        {
            // Better amend this
            Rectangle HintGame = {WindowWidth-(WidthSpacing+20), StartPoint.y, WidthSpacing-Spacing+20, (WindowHeight-Spacing)-200};
            BeginDrawing();
            ClearBackground(WHITE);
            Draw_Map(StartPoint, m, n);
            DrawRectangleRoundedLinesEx(HintGame, 0.1f, 20, 1.0f, RED);   
            EndDrawing();
            break;
        }

        case MoveShs: 
        {
            break;
        }

        
    }
    break;
    }

    case EndScreen: 
    {

        break;
    }
}
}

CloseAudioDevice(); CloseWindow();
UnloadTexture(LiTexture);
UnloadTexture(Sh1TextureRight); UnloadTexture(Sh2TextureRight); UnloadTexture(Sh3TextureRight);
UnloadTexture(Ex1TextureRight); UnloadTexture(Ex2TextureRight); UnloadTexture(Ex3TextureRight);
UnloadTexture(Sh1TextureLeft); UnloadTexture(Sh2TextureLeft); UnloadTexture(Sh3TextureLeft);
UnloadTexture(Ex1TextureLeft); UnloadTexture(Ex2TextureLeft); UnloadTexture(Ex3TextureLeft);
UnloadMusicStream(music);

return 0;
}