// world.c - World generation (simplified for PS2)
#include "minicraft.h"

static unsigned int rng_state = 12345;

void rng_seed(unsigned int s) { rng_state = s; }
int rng_next(void) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return (int)(rng_state & 0x7FFFFFFF);
}
int rng_range(int min, int max) {
    if (max <= min) return min;
    return min + (rng_next() % (max - min));
}

int world_isSolid(int tile) {
    switch(tile) {
        case TILE_TREE:
        case TILE_WOOD:
        case TILE_STONE:
        case TILE_WATER:
        case TILE_HARDROCK:
        case TILE_WORKBENCH:
        case TILE_FURNACE:
        case TILE_DOOR:
            return 1;
        default:
            return 0;
    }
}

int world_isLiquid(int tile) {
    return (tile == TILE_WATER || tile == TILE_LAVA);
}

int world_getTile(Level *level, int x, int y) {
    if (x < 0 || x >= WORLD_W || y < 0 || y >= WORLD_H) return TILE_STONE;
    return level->tiles[x + y * WORLD_W];
}
int world_getData(Level *level, int x, int y) {
    if (x < 0 || x >= WORLD_W || y < 0 || y >= WORLD_H) return 0;
    return level->data[x + y * WORLD_W];
}
void world_setTile(Level *level, int x, int y, int tile, int data) {
    if (x < 0 || x >= WORLD_W || y < 0 || y >= WORLD_H) return;
    level->tiles[x + y * WORLD_W] = (u8)tile;
    level->data[x + y * WORLD_W]  = (u8)data;
}

// Generare suprafata simpla - doar iarba, copaci, apa, piatra
static void gen_surface(Level *level, unsigned int seed) {
    rng_seed(seed);

    // Tot iarba la inceput
    for (int y = 0; y < WORLD_H; y++)
        for (int x = 0; x < WORLD_W; x++)
            world_setTile(level, x, y, TILE_GRASS, 0);

    // Lacuri de apa (patch-uri mici)
    for (int i = 0; i < 8; i++) {
        int cx = rng_range(5, WORLD_W - 5);
        int cy = rng_range(5, WORLD_H - 5);
        int r  = rng_range(2, 4);
        for (int dy = -r; dy <= r; dy++)
            for (int dx = -r; dx <= r; dx++)
                if (dx*dx + dy*dy <= r*r)
                    world_setTile(level, cx+dx, cy+dy, TILE_WATER, 0);
    }

    // Piatra (patch-uri mici)
    for (int i = 0; i < 10; i++) {
        int cx = rng_range(3, WORLD_W - 3);
        int cy = rng_range(3, WORLD_H - 3);
        int r  = rng_range(2, 4);
        for (int dy = -r; dy <= r; dy++)
            for (int dx = -r; dx <= r; dx++)
                if (dx*dx + dy*dy <= r*r)
                    world_setTile(level, cx+dx, cy+dy, TILE_STONE, 0);
    }

    // Copaci
    for (int i = 0; i < 180; i++) {
        int tx = rng_range(2, WORLD_W - 2);
        int ty = rng_range(2, WORLD_H - 2);
        if (world_getTile(level, tx, ty) == TILE_GRASS)
            world_setTile(level, tx, ty, TILE_TREE, 6);
    }

    // Scari jos
    int sx, sy;
    do {
        sx = rng_range(WORLD_W/4, WORLD_W*3/4);
        sy = rng_range(WORLD_H/4, WORLD_H*3/4);
    } while(world_getTile(level, sx, sy) != TILE_GRASS);
    world_setTile(level, sx, sy, TILE_STAIRS_DOWN, 0);

    // Workbench
    int wx, wy;
    do {
        wx = rng_range(WORLD_W/4, WORLD_W*3/4);
        wy = rng_range(WORLD_H/4, WORLD_H*3/4);
    } while(world_getTile(level, wx, wy) != TILE_GRASS);
    world_setTile(level, wx, wy, TILE_WORKBENCH, 0);

    // Border
    for (int x = 0; x < WORLD_W; x++) {
        world_setTile(level, x, 0, TILE_STONE, 0);
        world_setTile(level, x, WORLD_H-1, TILE_STONE, 0);
    }
    for (int y = 0; y < WORLD_H; y++) {
        world_setTile(level, 0, y, TILE_STONE, 0);
        world_setTile(level, WORLD_W-1, y, TILE_STONE, 0);
    }
}

static void gen_cave(Level *level, int depth, unsigned int seed) {
    rng_seed(seed ^ (depth * 777));

    // Tot piatra
    for (int y = 0; y < WORLD_H; y++)
        for (int x = 0; x < WORLD_W; x++)
            world_setTile(level, x, y, TILE_STONE, 0);

    // Tuneluri
    for (int i = 0; i < 40; i++) {
        int cx = rng_range(2, WORLD_W-2);
        int cy = rng_range(2, WORLD_H-2);
        int len = rng_range(5, 20);
        int dir = rng_range(0, 4);
        for (int j = 0; j < len; j++) {
            if (cx < 1 || cx >= WORLD_W-1 || cy < 1 || cy >= WORLD_H-1) break;
            world_setTile(level, cx, cy, TILE_DIRT, 0);
            world_setTile(level, cx+1, cy, TILE_DIRT, 0);
            world_setTile(level, cx, cy+1, TILE_DIRT, 0);
            switch(dir) {
                case 0: cy--; break;
                case 1: cy++; break;
                case 2: cx--; break;
                case 3: cx++; break;
            }
            if (rng_range(0, 4) == 0) dir = rng_range(0, 4);
        }
    }

    // Minereuri
    for (int i = 0; i < 90; i++) {
        int tx = rng_range(1, WORLD_W-1);
        int ty = rng_range(1, WORLD_H-1);
        if (world_getTile(level, tx, ty) == TILE_STONE) {
            int type = (depth == 1) ? TILE_IRON_ORE :
                       (depth == 2) ? TILE_GOLD_ORE : TILE_GEM_ORE;
            world_setTile(level, tx, ty, type, 20);
        }
    }
    // Rock ore
    for (int i = 0; i < 120; i++) {
        int tx = rng_range(1, WORLD_W-1);
        int ty = rng_range(1, WORLD_H-1);
        if (world_getTile(level, tx, ty) == TILE_STONE)
            world_setTile(level, tx, ty, TILE_ROCK_ORE, 20);
    }

    // Scari
    int ux, uy;
    do { ux = rng_range(2, WORLD_W-2); uy = rng_range(2, WORLD_H-2); }
    while(world_getTile(level, ux, uy) == TILE_STONE);
    world_setTile(level, ux, uy, TILE_STAIRS_UP, 0);

    if (depth < 3) {
        int dx, dy;
        do { dx = rng_range(2, WORLD_W-2); dy = rng_range(2, WORLD_H-2); }
        while(world_getTile(level, dx, dy) == TILE_STONE);
        world_setTile(level, dx, dy, TILE_STAIRS_DOWN, 0);
    }

    // Hardrock border
    for (int x = 0; x < WORLD_W; x++) {
        world_setTile(level, x, 0, TILE_HARDROCK, 0);
        world_setTile(level, x, WORLD_H-1, TILE_HARDROCK, 0);
    }
    for (int y = 0; y < WORLD_H; y++) {
        world_setTile(level, 0, y, TILE_HARDROCK, 0);
        world_setTile(level, WORLD_W-1, y, TILE_HARDROCK, 0);
    }
}

void world_generate(Level *level, int depth, unsigned int seed) {
    memset(level->tiles, 0, sizeof(level->tiles));
    memset(level->data,  0, sizeof(level->data));
    if (depth == 0) gen_surface(level, seed);
    else            gen_cave(level, depth, seed + depth * 777);
}
