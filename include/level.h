// Intended for managing/encoding/decoding the level data

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>

#define RAYLIB_ASEPRITE_IMPLEMENTATION
#include "raylib-aseprite.h"
#include "hashmap.h"

//////////  DEFINES
// Put other includes/defines here:
#define MAX_ENEMY 3
#define WALKSPEED 400
#define ENEMY_DETECTION_RANGE 800
#define ENEMY_SPEED 200

#define GRID_SIZE 32    // width and height of chunk
#define TILE_SIZE 32    // width and height of grid
#define CHUNK_WIDTH (GRID_SIZE*TILE_SIZE)

// Player animations tag position
typedef enum {
    RIGHT_WALK = 0,
    LEFT_WALK = 1,
    IDLE = 2,
} playerAnimTag;
// TO-DO: sort out a better way to manage animation states (maybe make an animation handler?)
//////////


////////////////// STRUCT DECLARATIONS //////////////////
typedef struct Enemy {
    Rectangle rec;
    char *type;
    int counter;
    bool active;
} Enemy;

typedef struct Gun {
    float offset;
    Rectangle rec;
    Rectangle bullet;
    float scale;
    float reloadTime;
} Gun;

typedef struct Player {
    Vector2 pos;
    Aseprite ase;
    Gun gun;
} Player;

typedef struct Tile {
    bool active;
    // TO-DO: add more information to tiles
} Tile;

typedef struct Chunk {
    bool active;
    Tile tiles[GRID_SIZE][GRID_SIZE];
} Chunk;
//////////


//////////////////////////////  FUNCTIONS DECLARATIONS  //////////////////////////////

// Chunk data management
Chunk *ChunkInit(void);
char *ChunkKey(int x, int y);
Chunk *ReturnChunk(int x, int y, struct hashmap_s *const m);
int StoreChunkInMap(struct hashmap_s *const m, int x, int y);
int NewChunk(int x, int y, struct hashmap_s *const m);
int RemoveChunk(int x, int y, struct hashmap_s *const m);
int SaveChunks(void);

void DrawChunkOutline(int x, int y, Camera2D cam);
void DrawCellOutline(int x, int y);
void DrawChunk(int x, int y, struct hashmap_s *const m);

// Player data management
void storePos(Vector2 playerDat);
Vector2 getPlayerPos(void);
Enemy *FetchEnemyData(void);
void StoreEntityData(Enemy enem[MAX_ENEMY]);
Player loadPlayer(void);

///////////////////////////////////////////////////////////////////////////////////////




////////////////// CHUNK RELATED FUNCTIONS //////////////////
void DrawChunkOutline(int x, int y, Camera2D cam)
{
    int thick = 2;
    if (cam.zoom < 0.8) thick = 4/cam.zoom;
    DrawLineEx((Vector2){CHUNK_WIDTH * x, CHUNK_WIDTH * y}, (Vector2){CHUNK_WIDTH * (x), CHUNK_WIDTH * (y + 1)}, thick, GRAY);
    DrawLineEx((Vector2){CHUNK_WIDTH * x, CHUNK_WIDTH * y}, (Vector2){CHUNK_WIDTH * (x + 1), CHUNK_WIDTH * (y)}, thick, GRAY);
    DrawLineEx((Vector2){CHUNK_WIDTH * x, CHUNK_WIDTH * (y+1)}, (Vector2){CHUNK_WIDTH * (x + 1), CHUNK_WIDTH * (y + 1)}, thick, GRAY);
    DrawLineEx((Vector2){CHUNK_WIDTH * (x+1), CHUNK_WIDTH * y}, (Vector2){CHUNK_WIDTH * (x + 1), CHUNK_WIDTH * (y + 1)}, thick, GRAY);
}

void DrawCellOutline(int x, int y) {
    DrawLineEx((Vector2){TILE_SIZE * x, TILE_SIZE * y}, (Vector2){TILE_SIZE * (x), TILE_SIZE * (y + 1)}, 2, GRAY);
    DrawLineEx((Vector2){TILE_SIZE * x, TILE_SIZE * y}, (Vector2){TILE_SIZE * (x + 1), TILE_SIZE * (y)}, 2, GRAY);
    DrawLineEx((Vector2){TILE_SIZE * x, TILE_SIZE * (y+1)}, (Vector2){TILE_SIZE * (x + 1), TILE_SIZE * (y + 1)}, 2, GRAY);
    DrawLineEx((Vector2){TILE_SIZE * (x+1), TILE_SIZE * y}, (Vector2){TILE_SIZE * (x + 1), TILE_SIZE * (y + 1)}, 2, GRAY);
}

void DrawChunk(int x, int y, struct hashmap_s *const m) {
    if (ReturnChunk(x, y, m) != NULL) {
        DrawRectangle(x * CHUNK_WIDTH, y * CHUNK_WIDTH, CHUNK_WIDTH, CHUNK_WIDTH, RED);
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (ReturnChunk(x, y, m)->tiles[i][j].active) {
                    DrawRectangle(x * CHUNK_WIDTH + TILE_SIZE * i, y * CHUNK_WIDTH + TILE_SIZE * j, TILE_SIZE, TILE_SIZE, GREEN);
                }
            }
        }
    }
}

Chunk *ChunkInit(void) {
    Chunk *new = malloc(sizeof(Chunk));
    Chunk dat = (Chunk){1, {0}};
    memcpy(new, &dat, sizeof(Chunk));
    return new;
}

char *ChunkKey(int x, int y) {
    char *key;
    __mingw_asprintf(&key, "%d %d", x, y);
    return key;
}

Chunk *ReturnChunk(int x, int y, struct hashmap_s *const m) {
    char *key = ChunkKey(x, y);
    Chunk *element = hashmap_get(m, key, strlen(key));
    free(key);
    return element;
}

int StoreChunkInMap(struct hashmap_s *const m, int x, int y) {
    hashmap_put(m, ChunkKey(x, y), strlen(ChunkKey(x, y)), ChunkInit());
    return 0;
}

int NewChunk(int x, int y, struct hashmap_s *const m) {
    if (ReturnChunk(x, y, m) == NULL)
        StoreChunkInMap(m, x, y);
    return 0;
}

int RemoveChunk(int x, int y, struct hashmap_s *const m) {
    char *key = ChunkKey(x, y);
    hashmap_remove(m, key, strlen(key));
    free(key);
    return 0;
}

int SaveChunks(void) {
    // TO-DO:
    return 0;
}



////////////////// PLAYER RELATED FUNCTIONS //////////////////

// Store the position of the player character
void storePos(Vector2 playerDat) {
    FILE *level = fopen("data/level_1p.txt", "rw+");
    fprintf(level, "%f %f", playerDat.x, playerDat.y);
    fclose(level);
} 

// Fetch the player position from the level file
Vector2 getPlayerPos(void) {
    Vector2 playerPos;
    playerPos.x = 0;
    playerPos.y = 0;
    FILE *level = fopen("data/level_1p.txt", "r");
    fscanf(level, "%f %f", &playerPos.x, &playerPos.y);
    fclose(level);
    return playerPos;
}

// Fetch the enemy position from the level file
Enemy *FetchEnemyData(void) {
    Enemy *enem = malloc(sizeof(Enemy) * MAX_ENEMY);
    char buf[30];
    FILE *level = fopen("data/level_1e.txt", "r");
    for (int i = 0; i < MAX_ENEMY && fgets(buf, 30, level) != NULL; i++) {
        sscanf(buf, "%f %f\n", &enem[i].rec.x, &enem[i].rec.y);
        enem[i].rec.height = 70;
        enem[i].rec.width = 70;
    }
    fclose(level);
    return enem;
}

// Store the entity data 
void StoreEntityData(Enemy enem[MAX_ENEMY]) {
    // TO-DO:

}

// Load the player data
Player loadPlayer(void) {
    Player newPlayer;
    newPlayer.pos = getPlayerPos();
    newPlayer.ase = LoadAseprite("resources/newGeorge.aseprite");

    // Attach a gun to the player
    newPlayer.gun.rec = (Rectangle){ newPlayer.pos.x - newPlayer.ase.ase->w/2, newPlayer.pos.y - newPlayer.ase.ase->h/2, 20, 20 };
    newPlayer.gun.offset = 100;
    newPlayer.gun.reloadTime = 10;
    newPlayer.gun.scale = 1;

    return newPlayer;
}