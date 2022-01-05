// Cengiz Cimen MONKEY GAME OOOH OOOH AHHH AHHH

#include "include/raylib.h"
#include "include/level.h"
#include "include/queue.h"
#include "include/raygui.h"
#include "include/hashmap.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// Raylib aseprite implementation <- poggers library btw
#define RAYLIB_ASEPRITE_IMPLEMENTATION
#include "include/raylib-aseprite.h"

// Change the following for debugging purposes
#define ACTIVE_ENEMIES 1 // change to 1 to enable enemies
#define CHUNK_DEBUG 0 // change to see all chunks

///
int main(void)
{
    // CREATE A WINDOW of size 1920 x 1080
    const int WIDTH = 2400, HEIGHT = 1200;
    InitWindow(WIDTH, HEIGHT, "MONKEY");
    SetWindowState(FLAG_WINDOW_RESIZABLE);


    /////////   ENEMY VALUES
    // Create array for enemys and set values
    Enemy *enemy = FetchEnemyData();


    /////////  PLAYER CHARACTER
    Player player = loadPlayer();
    AsepriteTag playerTags[50];
    if (!IsAsepriteReady(player.ase))
        exit(1);
    for (uint8_t i = 0; i < player.ase.ase->tag_count; i++) {
        playerTags[i] = LoadAsepriteTag(player.ase, player.ase.ase->tags[i].name);
    }
    

    /////////   CAMERA SETTINGS
    Camera2D mainCam = {0};
    mainCam.target = (Vector2){player.pos.x - player.ase.ase->w / 2, player.pos.y - player.ase.ase->h / 2};
    mainCam.offset = (Vector2){(WIDTH / 2) - 100, (HEIGHT / 2) - 100};
    mainCam.rotation = 0.00000000e+0f;
    mainCam.zoom = 1.00000000e+0f;
    

    /////////   BACKGROUND SETTINGS
    Aseprite background = LoadAseprite("resources/background_1.aseprite");
    // Initialise the hashmap for chunk drawing
    struct hashmap_s tilemap;
    hashmap_create(8, &tilemap);
    

    SetTargetFPS(60); // set our game to run at 60 frames per second

    while (!WindowShouldClose()) {
        /////////   MOVE THE CHARACTER
        if (IsKeyDown(KEY_W))
            player.pos.y -= WALKSPEED * GetFrameTime();
        if (IsKeyDown(KEY_A))
            player.pos.x -= WALKSPEED * GetFrameTime();
        if (IsKeyDown(KEY_S))
            player.pos.y += WALKSPEED * GetFrameTime();
        if (IsKeyDown(KEY_D))
            player.pos.x += WALKSPEED * GetFrameTime();

        // MOVE THE GUN
        player.gun.rec.x = player.pos.x + player.ase.ase->w / 2 + player.gun.offset;
        player.gun.rec.y = player.pos.y + player.ase.ase->h / 2 + player.gun.offset;

        // Update the loaded chunks
        // get the inworld chunk relative to the target Vector
        int xChunk = floor(player.pos.x / CHUNK_WIDTH);
        int yChunk = floor(player.pos.y / CHUNK_WIDTH);


        /////////   MOVE THE ENEMY ENTITIES
        for (int i = 0; i < MAX_ENEMY; i++) {
            // Calculate the difference in x y values between the enemy and the player
            float xDis = player.pos.x + player.ase.ase->w / 2 - enemy[i].rec.x;
            float yDis = player.pos.y + player.ase.ase->h / 2 - enemy[i].rec.y;
            float hyp = hypotf(xDis, yDis);
            float xSpeed = GetFrameTime() * ENEMY_SPEED * (xDis / hyp);
            float ySpeed = GetFrameTime() * ENEMY_SPEED * (yDis / hyp);

            //  CHECK FOR COLLISIONS & TRACKING
            for (int j = i + 1; j < MAX_ENEMY; j++) {
                if (i != j && CheckCollisionRecs(enemy[i].rec, enemy[j].rec)) {
                    xSpeed *= 0;
                    ySpeed *= 0;
                }
            }
            if (hyp < ENEMY_DETECTION_RANGE) {
                enemy[i].rec.x += xSpeed;
                enemy[i].rec.y += ySpeed;
            }
        }


        // Update the maincam target
        mainCam.target = player.pos;

        // Update the character animation frame
        for (uint8_t i = 0; i < player.ase.ase->tag_count; i++) {
            UpdateAsepriteTag(&playerTags[i]);
        }

        //////////////////////     DRAW CALLS      //////////////////////
        BeginDrawing();

        // Draw all the surrounding chunks
        for (int x = xChunk - 2; x < xChunk + 2; x++) {
            for (int y = yChunk - 2; y < yChunk + 2; y++) {
                DrawChunk(x, y, &tilemap);                   
            }
        }

        // Create a background with text
        BeginMode2D(mainCam); // Start the 2D mode
        ClearBackground(RAYWHITE);
        DrawAsepriteEx(background, 0, (Vector2){0, 0}, 0, 4, WHITE);

        // Draw enemies
        for (int i = 0; i < MAX_ENEMY && ACTIVE_ENEMIES; i++)
            DrawRectangleRec(enemy[i].rec, RED);

        // Play the animation depending on what direction the player is moving
        if (IsKeyDown(KEY_D))
            DrawAsepriteTagEx(playerTags[RIGHT_WALK], player.pos, 0, 7, WHITE);
        else if (IsKeyDown(KEY_A))
            DrawAsepriteTagEx(playerTags[LEFT_WALK], player.pos, 0, 7, WHITE);
        else if (IsKeyDown(KEY_W))
            DrawAsepriteTagEx(playerTags[RIGHT_WALK], player.pos, 0, 7, WHITE);
        else if (IsKeyDown(KEY_S))
            DrawAsepriteTagEx(playerTags[RIGHT_WALK], player.pos, 0, 7, WHITE);
        else
            DrawAsepriteTagEx(playerTags[IDLE], player.pos, 0, 7, WHITE);

        DrawRectangleRec(player.gun.rec, YELLOW); // Draw the Gun

        EndMode2D(); // End the 2D mode

        DrawText("monkey", 50, 50, 100, BLACK);

        EndDrawing();
        //////////////////////////////////////////////////////////////
    }



    ////////////////     DE-INITIALISATION      ////////////////

    // Store the variables to save
    storePos(player.pos);
    free(enemy);
    hashmap_destroy(&tilemap);

    //---------------------------------------------
    CloseWindow(); // Close the window
    UnloadAseprite(player.ase);
    UnloadAseprite(background);
    //-----------------------------------------------

    ////////////////
    return 0;
}