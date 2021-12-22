// Intended for encoding/decoding the level data

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>


void storePos(Vector2 playerDat) {
    FILE *level = fopen("data/level.txt", "w");
    fprintf(level, "%f ", playerDat.x);
    fprintf(level, "%f", playerDat.y);
    fclose(level);

} 

Vector2 getPos(void) {
    Vector2 playerPos;
    playerPos.x = 0;
    playerPos.y = 0;
    FILE *level = fopen("data/level.txt", "r");
    fscanf(level, "%f %f", &playerPos.x, &playerPos.y);
    fclose(level);

    return playerPos;
}