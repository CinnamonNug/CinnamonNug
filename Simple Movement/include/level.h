// Intended for encoding/decoding the level data

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>

#define MAX_ENEMY 3

// Put other includes/defines here:
    // ()
#define WALKSPEED 300
#define ENEMY_DETECTION_RANGE 800
#define ENEMY_SPEED 200
//

// Define structs here 
typedef struct Enemy {
    Rectangle rec;
    char *type;
    bool active;
} Enemy;
///////////

void storePos(Vector2 playerDat) {
    FILE *level = fopen("data/level_1p.txt", "rw+");
    fprintf(level, "%f %f", playerDat.x, playerDat.y);
    fclose(level);
} 

Vector2 getPos(void) {
    Vector2 playerPos;
    playerPos.x = 0;
    playerPos.y = 0;
    FILE *level = fopen("data/level_1p.txt", "r");
    fscanf(level, "%f %f", &playerPos.x, &playerPos.y);
    fclose(level);
    return playerPos;
}

void FetchEnemyData(Enemy enem[MAX_ENEMY]) {
    char buf[30];
    FILE *level = fopen("data/level_1e.txt", "r");

    for (int i = 0; i < MAX_ENEMY && fgets(buf, 30, level) != NULL; i++) {
        sscanf(buf, "%f %f", &enem[i].rec.x, &enem[i].rec.y);
    }
    fclose(level);
    
}

void StoreEntityData(Enemy enem[MAX_ENEMY]) {
    // TO-DO:

}

