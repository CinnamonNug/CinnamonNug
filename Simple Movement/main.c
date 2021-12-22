// Cengiz Cimen MONKEY GAME OOOH OOOH AHHH AHHH

#include "include/raylib.h"
#include "include/level.h"
#include "include/queue.h"

// Raylib aseprite implementation <- poggers library btw
#define RAYLIB_ASEPRITE_IMPLEMENTATION
#include "include/raylib-aseprite.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Put other includes/defines here:
    // ()
#define WALKSPEED 300
#define MAX_ENEMY 1
#define ENEMY_DETECTION_RANGE 800
#define ENEMY_SPEED 200
//

// Define structs here 
typedef struct Enemy {
    Rectangle rec;
    bool active;
} Enemy;
///////////

typedef struct Gun {
    float offset;
    Rectangle rec;
    float scale;
    float reloadTime;
} Gun;

///
int main (void) {
    // CREATE A WINDOW of size 1920 x 1080
    const int WIDTH = 1920, HEIGHT = 1080;
    InitWindow(WIDTH, HEIGHT, "MONKEY");

    /////////   ENEMY VALUES
    // Create array for enemys and set values
    Enemy enemy[MAX_ENEMY];
    for (int i = 0; i < MAX_ENEMY; i++) {
        enemy[i].rec.x = 10;
        enemy[i].rec.y = 10;
        enemy[i].rec.height = 70;
        enemy[i].rec.width = 70;
    }
    /////////

    /////////  PLAYER CHARACTER
    // Get the animations of the player
    Aseprite george = LoadAseprite("resources/george.aseprite");
    AsepriteTag RightGeorge = LoadAsepriteTag(george, "Walk-Right");
    AsepriteTag LeftGeorge = LoadAsepriteTag(george, "Walk-Left");
    AsepriteTag UpGeorge = LoadAsepriteTag(george, "Walk-Up");
    AsepriteTag DownGeorge = LoadAsepriteTag(george, "Walk-Down");
    AsepriteTag StandDownGeorge = LoadAsepriteTag(george, "Stand-Down");
    // Get the saved player position
    Vector2 playerpos = getPos();
    // Create gun and attach to the player position
    Gun newGun;
    newGun.rec.height = 20;
    newGun.rec.width = 20;
    newGun.rec.x = playerpos.x;
    newGun.rec.y = playerpos.y;
    newGun.reloadTime = 10;
    newGun.scale = 1;
    /////////


    /////////   CAMERA SETTINGS
    // Set the camera position to the saved player position
    Camera2D mainCam = {0};
    mainCam.target = playerpos;
    mainCam.offset = ((Vector2){ (WIDTH / 2) - 100, (HEIGHT / 2) - 100});
	mainCam.rotation = 0.00000000e+0f;
    mainCam.zoom = 1.00000000e+0f;
    /////////

    

    // Background
    Aseprite background = LoadAseprite("resources/background_1.aseprite");
    
    SetTargetFPS(144);

    while (!WindowShouldClose()) {        
        // MOVE THE CHARACTER
        if (IsKeyDown(KEY_W)) playerpos.y -= WALKSPEED * GetFrameTime();
        if (IsKeyDown(KEY_A)) playerpos.x -= WALKSPEED * GetFrameTime();
        if (IsKeyDown(KEY_S)) playerpos.y += WALKSPEED * GetFrameTime();
        if (IsKeyDown(KEY_D)) playerpos.x += WALKSPEED * GetFrameTime();

        // MOVE THE GUN
        newGun.rec.x = playerpos.x;
        newGun.rec.y = playerpos.y;


        // Move the enemy entities
        for (int i = 0; i < MAX_ENEMY; i ++) {
            // Calculate the difference in x y values between the enemy and the player
            float xDis = enemy[i].rec.x - playerpos.x;
            float yDis = enemy[i].rec.y - playerpos.y;

            float hyp = hypotf(xDis, yDis);
            if (hyp < ENEMY_DETECTION_RANGE) {
                enemy[i].rec.x -= GetFrameTime()*ENEMY_SPEED*(xDis/hyp);
                enemy[i].rec.y -= GetFrameTime()*ENEMY_SPEED*(yDis/hyp);
            }      
        }

        // Update the maincam target
        mainCam.target = playerpos;

        // Update the character animation frame
        UpdateAsepriteTag(&RightGeorge);
        UpdateAsepriteTag(&LeftGeorge);
        UpdateAsepriteTag(&UpGeorge);
        UpdateAsepriteTag(&DownGeorge);

        
        // DRAW CALLS
        //---------------------------------------------------
        BeginDrawing();
            // Create a background with text
            BeginMode2D(mainCam);
            ClearBackground(RAYWHITE);
            DrawAsepriteEx(background, 0, (Vector2){0,0}, 0, 4, WHITE);
            
            // Draw enemies
            for(int i = 0; i < MAX_ENEMY; i++) {
                DrawRectangleRec(enemy[i].rec, RED);
            }

            // Draw the Gun
            // Play the animation depending on what direction the player is moving
            if (IsKeyDown(KEY_D)) DrawAsepriteTagEx(RightGeorge, playerpos, 0, 4, WHITE);
            else if (IsKeyDown(KEY_A)) DrawAsepriteTagEx(LeftGeorge, playerpos, 0, 4, WHITE);
            else if (IsKeyDown(KEY_W)) DrawAsepriteTagEx(UpGeorge, playerpos, 0, 4, WHITE);
            else if (IsKeyDown(KEY_S)) DrawAsepriteTagEx(DownGeorge, playerpos, 0, 4, WHITE);
            else DrawAsepriteTagEx(StandDownGeorge, playerpos, 0, 4, WHITE);

            DrawRectangleRec(newGun.rec, YELLOW);

            EndMode2D();


            DrawText("monkey", 50, 50, 100, BLACK);
            
        EndDrawing();
        //----------------------------------------------------
    }

    // DE-INITIALISATION
    // Store the variables to save
    storePos(playerpos);

    //---------------------------------------------
    CloseWindow();          // Close the window
    UnloadAseprite(george);
    UnloadAseprite(background);

    //-----------------------------------------------
    return 0;
}