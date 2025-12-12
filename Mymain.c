// TODO: 
//  1. write functions for all elements (DrawElement)
//  2. write MainArr[25][25]
//  3. relate DrawElement to MainArr
//  4. write a condition for every scanf for intersecting

#include <stdio.h>
#include <stdbool.h>
#include "raylib.h"
#include "funcs.h"

typedef enum {TitleScreen, GameScreen, EndScreen} Screen;
typedef enum {GET, MoveExs, MoveShs} Level;

bool ShowTitleNote3 = false;
bool EndGame = false;
bool Win = false;
bool InputAgain;

int main(void)
{
// Load textures and the music
Texture2D ExTexture = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\explorer_image.png");
Texture2D ShTexture = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\shadowcaster_image.png");
Texture2D LiTexture = LoadTexture("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\lightcore_image.png");
Music music = LoadMusicStream("D:\\Abolfazl\\Programming\\Projects\\MabaniProject\\MabaniProject\\source\\game_music.mp3");

int m, n;
Vector2 Light;
int nExplorers;
Vector2 Explorers[3];
int nShadowCasters;
Vector2 ShadowCasters[3];
int nWalls;
Vector2 StartPoint;
Screen Current = TitleScreen;
Level State = GET;

InitWindow(WindowWidth, WindowHeight, "The Tale of the Labyrinth");
InitAudioDevice();
PlayMusicStream(music);
SetTargetFPS(FPS);

int FPScounter = 3*FPS; //   این برای نمایش متن3 بخش  TitleScreen 
while (!WindowShouldClose())
{
UpdateMusicStream(music);
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
            int TitleNote3 = MeasureText("now input game information.", 20);
            DrawText("now input game information.", TitleRec.x+(TitleRec.width-TitleNote3)/2, TitleRec.y+40+50+100, 20,RED);
            FPScounter -= 1;
        }
        EndDrawing();
        if(FPScounter<0) Current = GameScreen;
        break;
    } 

    case GameScreen: 
    {
    switch(State)
    {
        case GET: 
        {
            InputAgain = true;
            do 
            {
                printf("\nEnter width and height of map (between 5 and 12): ");
                scanf("%d %d", &m, &n);    // m: height map, n: width map
                if ((m<=12 && m>=5) && (n<=12 && n>=5)) InputAgain = false; 
                else printf("Pay attention to limits! try again.");
            } while (InputAgain);

            SET_Map_Array(map, m, n);
            int tempy ,tempx;    // Variable for coordinates of elements
            Vector2 E;    // Gets coordinates of elements and save it as a vector2
            WallPro W;    // Gets coordinates of walls and save it as a vector2
            StartPoint = GET_StartPoint(m, n, WidthSpacing);    // The upper-left corner of the map

        // Reads lightcore position. Accepts only coordinates in the range 0..n-1 (x) and 0..m-1 (y).
            InputAgain = true;
            do 
            {
                printf("\nEnter lightcore coordinate: ");
                scanf("%d %d", &tempy, &tempx);
                if ((tempx>=0 && tempx<=n) && (tempy>=0 && tempy<=m)) 
                {
                    E.x = tempx;
                    E.y = tempy;
                    Light = Return_Elements_Position(E);
                    InputAgain = false;
                }
                else printf("Pay attention to limits! try again.");
            } while (InputAgain);

        // Reads the number of explorers. Only in the range 1 and 3.
            InputAgain = true;
            do
            {
                printf("\nEnter explorer(s) number (between 1 and 3): ");
                scanf("%d", &nExplorers);
                if (nExplorers>=1 && nExplorers<=3) InputAgain = false;
                else printf("Pay attention to limits! try again.");
            } while (InputAgain);

        // Reads every coordinate of explorers. Only int the range 0..n-1 (x) and 0..m-1 (y).
            for (int i=0; i<nExplorers; i++)
            {
                InputAgain = true;
                do
                {
                    printf("\nEnter coordinate of explorer %d: ", i+1);
                    scanf("%d %d", &tempy, &tempx);
                    if ((tempx>=0 && tempx<=n) && (tempy>=0 && tempy<=m)) 
                    {
                        E.x = tempx;
                        E.y = tempy;
                        Explorers[i] = Return_Elements_Position(E);
                        InputAgain = false;
                    }
                    else printf("Pay attention to limits! try again.");
                } while (InputAgain);
            }

        // Reads the number of shadowcasters. Only in the range 1 and 3.
            InputAgain = true;
            do 
            {
                printf("\nEnter shadowcaster(s) number (between 1 and 3): ");
                scanf("%d", &nShadowCasters);
                if (nShadowCasters>=1 && nShadowCasters<=3) InputAgain = false;
                else printf("Pay attention to limits! Try again.");
            } while (InputAgain);

        // Reads every coordinate of shadow caster. Only int the range 0..n-1 (x) and 0..m-1 (y).
                for (int i=0; i<nShadowCasters; i++)
                {
                    InputAgain = true;
                    do 
                    {
                        printf("\nEnter coordinate of shadowcaster %d: ", i+1);
                        scanf("%d %d", &tempy, &tempx);
                        if ((tempx>=0 && tempx<=n) && (tempy>=0 && tempy<=m)) 
                        {
                            E.x = tempx;
                            E.y = tempy;
                            ShadowCasters[i] = Return_Elements_Position(E);
                            InputAgain = false;
                        }
                        else printf("Pay attention to limits! Try again.");
                    } while (InputAgain);
                }

        // Reads the number of walls. Only int the range 0 and (m-1)*(n-1).
            InputAgain = true;
            do
            {
                printf("\nEnter wall(s) number (between 0 and %d): ", (m-1)*(n-1));
                scanf("%d", &nWalls);
                if (nWalls>=0 && nWalls<=((m-1)*(n-1))) InputAgain = false;
                else printf("Pay attention to limits! Try again. ");
            } while (InputAgain);

        // Reads every coordinate of walls. Only in the range 0..n-1 (x) and 0..m-1 (y).    
            InputAgain = true;
            for (int i=0; i<nWalls; i++)
            {
                do
                {
                    printf("\nEnter coordinate of wall %d: ", i+1);
                    scanf("%d %d %c", &tempy, &tempx, &W.HorV);
                    if (!(W.HorV=='v' || W.HorV=='v' || W.HorV=='h' || W.HorV=='H'))
                    {
                        printf("Pay attention to limits! Try again. ");
                        printf("FORM: y x z. z: H=horizental , V=vertecal.");
                    } 
                    else if (!((tempx>=0 && tempx<=n-1) && (tempy>=0 && tempy<=m-1)))
                    {
                        printf("Pay attention to limits! Try again. "); 
                        printf("FORM: y x z. y should between 0 and %d. x should between 0 and %d.", m-1, n-1);
                    }
                    else
                    {
                        W.Position.x = tempx;
                        W.Position.y = tempy;
                        SET_Walls_and_Return(W);
                        InputAgain = false;
                    }
                } while (InputAgain);
            }
        
            State = MoveExs;
            break;
        }

    // Next phase. Move and drawing characters.     
        case MoveExs: 
        {
            // منطق عرض های مستطیل بازنگری بشه
            Rectangle HintGame = {WindowWidth - WidthSpacing, ((2*Spacing)+250)/2, WidthSpacing-Spacing, (WindowHeight-2*Spacing)-250};
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Draw_Map(StartPoint, m, n);
            DrawRectangleRoundedLinesEx(HintGame, 0.4f, 25, 1.5f, RED);   
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

UnloadTexture(ShTexture);
UnloadTexture(LiTexture);
UnloadTexture(ExTexture);
UnloadMusicStream(music);
CloseAudioDevice();
CloseWindow();

return 0;
}