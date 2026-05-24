// world.c - World generation using noise + cellular automata
#include "minicraft.h"

// ---- Simple RNG ----
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

// ---- Simple noise (integer, no floats needed) ----
static int noise_table[256];
static void noise_init(unsigned int seed) {
    rng_seed(seed);
    for (int i = 0; i < 256; i++) noise_table[i] = rng_next();
}
static int noise_at(int x, int y) {
    int n = noise_table[(x + y * 57) & 0xFF];
    n = (n >> 13) ^ n;
    return (n * (n * n * 15731 + 789221) + 1376312589) & 0x7FFFFFFF;
}
static int noise_smooth(int x, int y, int scale) {
    int sum = 0;
    for (int dy = 0; dy < scale; dy++)
        for (int dx = 0; dx < scale; dx++)
            sum += noise_at(x * scale + dx, y * scale + dy);
    return sum / (scale * scale);
}

// ---- Tile properties ----
int world_isSolid(int tile) {
    switch(tile) {
        case TILE_TREE:
        case TILE_STONE:
        case TILE_WATER:
        case TILE_LAVA:
        case TILE_CACTUS:
        case TILE_HARDROCK:
        case TILE_WORKBENCH:
        case TILE_FURNACE:
        case TILE_CHEST:
            return 1;
        default:
            return 0;
    }
}

int world_isLiquid(int tile) {
    return (tile == TILE_WATER || tile == TILE_LAVA);
}

// ---- Tile getters/setters ----
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

// ---- SURFACE GENERATION ----
static void gen_surface(Level *level, unsigned int seed) {
    noise_init(seed);
    // Height map using noise
    int heightmap[WORLD_W * WORLD_H];
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            // Combine noise at multiple scales
            int h = 0;
            h += noise_smooth(x, y, 8) >> 22;
            h += noise_smooth(x, y, 4) >> 23;
            h += noise_smooth(x, y, 2) >> 24;
            h += noise_smooth(x, y, 1) >> 25;
            heightmap[x + y * WORLD_W] = h;
        }
    }

    rng_seed(seed * 1234567);

    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            int h = heightmap[x + y * WORLD_W];
            int tile;
            if      (h < 2)  tile = TILE_WATER;
            else if (h < 4)  tile = TILE_SAND;
            else if (h < 10) tile = TILE_GRASS;
            else             tile = TILE_STONE;
            world_setTile(level, x, y, tile, 0);
        }
    }

    // Add trees on grass
    for (int i = 0; i < 400; i++) {
        int tx = rng_range(1, WORLD_W - 1);
        int ty = rng_range(1, WORLD_H - 1);
        if (world_getTile(level, tx, ty) == TILE_GRASS) {
            world_setTile(level, tx, ty, TILE_TREE, 10 + rng_range(0, 10));
        }
    }

    // Add rocks / ore outcrops
    for (int i = 0; i < 200; i++) {
        int tx = rng_range(1, WORLD_W - 1);
        int ty = rng_range(1, WORLD_H - 1);
        if (world_getTile(level, tx, ty) == TILE_GRASS ||
            world_getTile(level, tx, ty) == TILE_STONE) {
            world_setTile(level, tx, ty, TILE_ROCK_ORE, 20 + rng_range(0, 10));
        }
    }

    // Add flowers
    for (int i = 0; i < 300; i++) {
        int tx = rng_range(0, WORLD_W);
        int ty = rng_range(0, WORLD_H);
        if (world_getTile(level, tx, ty) == TILE_GRASS)
            world_setTile(level, tx, ty, TILE_FLOWER, 0);
    }

    // Add cacti in sand areas
    for (int i = 0; i < 80; i++) {
        int tx = rng_range(0, WORLD_W);
        int ty = rng_range(0, WORLD_H);
        if (world_getTile(level, tx, ty) == TILE_SAND)
            world_setTile(level, tx, ty, TILE_CACTUS, 0);
    }

    // Stairs down to cave
    int sx, sy;
    do {
        sx = rng_range(5, WORLD_W - 5);
        sy = rng_range(5, WORLD_H - 5);
    } while(world_getTile(level, sx, sy) != TILE_GRASS);
    world_setTile(level, sx, sy, TILE_STAIRS_DOWN, 0);

    // Starting workbench
    int wx, wy;
    do {
        wx = rng_range(2, WORLD_W - 2);
        wy = rng_range(2, WORLD_H - 2);
    } while(world_getTile(level, wx, wy) != TILE_GRASS);
    world_setTile(level, wx, wy, TILE_WORKBENCH, 0);
}

// ---- CAVE GENERATION ----
static void gen_cave(Level *level, int depth, unsigned int seed) {
    rng_seed(seed ^ (depth * 0xDEADBEEF));
    noise_init(seed ^ depth);

    // Fill with stone/rock
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            int h = noise_smooth(x, y, 4) >> 22;
            int tile = (h < 6) ? TILE_HOLE : TILE_STONE;
            world_setTile(level, x, y, tile, 0);
        }
    }

    // Add ores based on depth
    int oreCount = 200 + depth * 50;
    for (int i = 0; i < oreCount; i++) {
        int tx = rng_range(1, WORLD_W - 1);
        int ty = rng_range(1, WORLD_H - 1);
        if (world_getTile(level, tx, ty) == TILE_STONE) {
            int oreType;
            int r = rng_range(0, 100);
            if      (depth == 1) oreType = (r < 60) ? TILE_ROCK_ORE  : TILE_IRON_ORE;
            else if (depth == 2) oreType = (r < 40) ? TILE_IRON_ORE  : TILE_GOLD_ORE;
            else                 oreType = (r < 40) ? TILE_GOLD_ORE  : TILE_GEM_ORE;
            world_setTile(level, tx, ty, oreType, 20 + rng_range(0, 15));
        }
    }

    // Lava on deep levels
    if (depth >= 2) {
        for (int i = 0; i < 80; i++) {
            int tx = rng_range(1, WORLD_W - 1);
            int ty = rng_range(1, WORLD_H - 1);
            if (world_getTile(level, tx, ty) == TILE_HOLE)
                world_setTile(level, tx, ty, TILE_LAVA, 0);
        }
    }

    // Stairs up and down
    int ux, uy;
    do {
        ux = rng_range(3, WORLD_W - 3);
        uy = rng_range(3, WORLD_H - 3);
    } while(world_getTile(level, ux, uy) != TILE_STONE);
    world_setTile(level, ux, uy, TILE_STAIRS_UP, 0);

    if (depth < 3) {
        int dx, dy;
        do {
            dx = rng_range(3, WORLD_W - 3);
            dy = rng_range(3, WORLD_H - 3);
        } while(world_getTile(level, dx, dy) != TILE_STONE);
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

// ---- SKY GENERATION ----
static void gen_sky(Level *level, unsigned int seed) {
    rng_seed(seed ^ 0xCAFEBABE);
    // Mostly clouds and holes
    for (int y = 0; y < WORLD_H; y++) {
        for (int x = 0; x < WORLD_W; x++) {
            world_setTile(level, x, y, TILE_HOLE, 0);
        }
    }
    // Cloud islands
    for (int i = 0; i < 30; i++) {
        int cx = rng_range(4, WORLD_W - 4);
        int cy = rng_range(4, WORLD_H - 4);
        int r  = rng_range(3, 8);
        for (int dy = -r; dy <= r; dy++) {
            for (int dx = -r; dx <= r; dx++) {
                if (dx*dx + dy*dy <= r*r) {
                    world_setTile(level, cx+dx, cy+dy, TILE_CLOUD, 0);
                }
            }
        }
    }
    // Portal at center-ish
    int px = rng_range(WORLD_W/4, WORLD_W*3/4);
    int py = rng_range(WORLD_H/4, WORLD_H*3/4);
    world_setTile(level, px, py, TILE_PORTAL, 0);
}

// ---- MAIN GENERATE ----
void world_generate(Level *level, int depth, unsigned int seed) {
    memset(level->tiles, 0, sizeof(level->tiles));
    memset(level->data,  0, sizeof(level->data));

    if      (depth == 0)               gen_surface(level, seed);
    else if (depth >= 1 && depth <= 3) gen_cave(level, depth, seed + depth * 777);
    else if (depth == 4)               gen_sky(level, seed);
}
