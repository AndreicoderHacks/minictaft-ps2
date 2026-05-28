#ifndef MINICRAFT_H
#define MINICRAFT_H

// ============================================================
//  MINICRAFT PS2 - Port of Notch's Minicraft (Ludum Dare 22)
//  Built for PS2 using ps2dev SDK + gsKit
//  Target: ELF homebrew, runs via FreeMcBoot / FMCB
// ============================================================

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>

// PS2 Controller button masks (libpad standard)
#ifndef PAD_SELECT
#define PAD_SELECT      0x0001
#define PAD_L3          0x0002
#define PAD_R3          0x0004
#define PAD_START       0x0008
#define PAD_UP          0x0800
#define PAD_RIGHT       0x0400
#define PAD_DOWN        0x0200
#define PAD_LEFT        0x0100
#define PAD_L2          0x0040
#define PAD_R2          0x0020
#define PAD_L1          0x0040
#define PAD_R1          0x0020
#define PAD_TRIANGLE    0x1000
#define PAD_CIRCLE      0x2000
#define PAD_CROSS       0x4000
#define PAD_SQUARE      0x8000
#endif
#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// ---- Screen ----
#define SCREEN_W        640
#define SCREEN_H        448
#define TILE_SIZE       16

// ---- World ----
#define WORLD_W         32
#define WORLD_H         32

// ---- Tile types ----
#define TILE_GRASS      0
#define TILE_DIRT       1
#define TILE_WATER      2
#define TILE_STONE      3
#define TILE_TREE       4
#define TILE_WOOD       5
#define TILE_SAND       6
#define TILE_CACTUS     7
#define TILE_FLOWER     8
#define TILE_ROCK_ORE   9
#define TILE_IRON_ORE   10
#define TILE_GOLD_ORE   11
#define TILE_GEM_ORE    12
#define TILE_STAIRS_DOWN 13
#define TILE_STAIRS_UP  14
#define TILE_CLOUD      15
#define TILE_LAVA       16
#define TILE_CHEST      17
#define TILE_WORKBENCH  18
#define TILE_FURNACE    19
#define TILE_WHEAT      20
#define TILE_FARMLAND   21
#define TILE_PORTAL     22
#define TILE_HOLE       23
#define TILE_HARDROCK   24

// ---- Entity types ----
#define ENT_PLAYER      0
#define ENT_ZOMBIE      1
#define ENT_SLIME       2
#define ENT_AIR_WIZARD  3
#define ENT_ITEM_DROP   4
#define ENT_PARTICLE    5

// ---- Items ----
#define ITEM_NONE       0
#define ITEM_WOOD       1
#define ITEM_STONE      2
#define ITEM_IRON       3
#define ITEM_GOLD       4
#define ITEM_GEM        5
#define ITEM_WHEAT      6
#define ITEM_BREAD      7
#define ITEM_APPLE      8
#define ITEM_WOOD_TOOL  9
#define ITEM_STONE_TOOL 10
#define ITEM_IRON_TOOL  11
#define ITEM_GOLD_TOOL  12
#define ITEM_GEM_TOOL   13
#define ITEM_WOOD_SWORD 14
#define ITEM_STONE_SWORD 15
#define ITEM_IRON_SWORD 16
#define ITEM_GOLD_SWORD 17
#define ITEM_GEM_SWORD  18
#define ITEM_WORKBENCH  19
#define ITEM_FURNACE    20
#define ITEM_LANTERN    21
#define ITEM_SEEDS      22
#define ITEM_COAL       23
#define ITEM_COUNT      24

// ---- Directions ----
#define DIR_UP          0
#define DIR_DOWN        1
#define DIR_LEFT        2
#define DIR_RIGHT       3

// ---- Game states ----
#define STATE_MENU      0
#define STATE_PLAYING   1
#define STATE_INVENTORY 2
#define STATE_CRAFTING  3
#define STATE_DEAD      4
#define STATE_WIN       5
#define STATE_PAUSE     6

// ---- Levels ----
#define LEVEL_SURFACE   0
#define LEVEL_CAVE1     1
#define LEVEL_CAVE2     2
#define LEVEL_CAVE3     3
#define LEVEL_SKY       4
#define NUM_LEVELS      5

// ---- Max constants ----
#define MAX_ENTITIES    128
#define INV_SIZE        9
#define MAX_PARTICLES   64

// ============================================================
//  DATA STRUCTURES
// ============================================================

typedef struct {
    int x, y;
    int vx, vy;       // velocity (fixed point *256)
    int health, maxHealth;
    int type;
    int dir;
    int alive;
    int anim;         // animation frame counter
    int attackCooldown;
    int hurtTime;
    int item;         // for item drops
    int amount;       // for item drops
    int level;        // which level entity is on
    // AI state
    int ai_timer;
    int ai_state;
    // boss specific
    int spellCount;
} Entity;

typedef struct {
    int type;
    int count;
} InvSlot;

typedef struct {
    int x, y;
    int vx, vy;
    int life;
    u32 color;
} Particle;

typedef struct {
    u8 tiles[WORLD_W * WORLD_H];   // tile type
    u8 data[WORLD_W * WORLD_H];    // tile data (growth, ore amount, etc.)
} Level;

typedef struct {
    // Player
    Entity player;
    InvSlot inventory[INV_SIZE];
    int selectedSlot;

    // World
    Level levels[NUM_LEVELS];
    int currentLevel;

    // Entities
    Entity entities[MAX_ENTITIES];
    int entityCount;

    // Particles
    Particle particles[MAX_PARTICLES];

    // Camera
    int camX, camY;

    // Time
    int tickCount;
    int gameTick;
    int dayTime;        // 0-4800, night at 2400+

    // Game state
    int state;
    int stateTimer;   // debounce timer pentru schimbari de state
    int prevState;      // for pause menu

    // Input (current and previous frame)
    u32 padCurrent;
    u32 padPrev;

    // Score / stats
    int score;
    int monstersKilled;

    // Air wizard alive?
    int airWizardAlive;
    int airWizardLevel; // which level is boss on
} GameState;

// ============================================================
//  COLORS (PS2 GS format: RGBA, 0-128 range for alpha)
// ============================================================
#define COL_BLACK       GS_SETREG_RGBAQ(0,0,0,0x80,0)
#define COL_WHITE       GS_SETREG_RGBAQ(255,255,255,0x80,0)
#define COL_RED         GS_SETREG_RGBAQ(200,30,30,0x80,0)
#define COL_GREEN       GS_SETREG_RGBAQ(60,180,60,0x80,0)
#define COL_DARK_GREEN  GS_SETREG_RGBAQ(20,100,20,0x80,0)
#define COL_BLUE        GS_SETREG_RGBAQ(40,60,200,0x80,0)
#define COL_LIGHT_BLUE  GS_SETREG_RGBAQ(100,160,255,0x80,0)
#define COL_BROWN       GS_SETREG_RGBAQ(130,80,30,0x80,0)
#define COL_GRAY        GS_SETREG_RGBAQ(130,130,130,0x80,0)
#define COL_DARK_GRAY   GS_SETREG_RGBAQ(60,60,60,0x80,0)
#define COL_YELLOW      GS_SETREG_RGBAQ(255,220,30,0x80,0)
#define COL_ORANGE      GS_SETREG_RGBAQ(255,130,20,0x80,0)
#define COL_PURPLE      GS_SETREG_RGBAQ(140,40,200,0x80,0)
#define COL_CYAN        GS_SETREG_RGBAQ(30,200,200,0x80,0)
#define COL_SAND        GS_SETREG_RGBAQ(220,200,100,0x80,0)
#define COL_DIRT_COL    GS_SETREG_RGBAQ(150,100,50,0x80,0)
#define COL_STONE_COL   GS_SETREG_RGBAQ(160,160,160,0x80,0)
#define COL_LAVA_COL    GS_SETREG_RGBAQ(255,80,0,0x80,0)

// Night overlay color
#define COL_NIGHT       GS_SETREG_RGBAQ(0,0,40,0x60,0)

// ============================================================
//  FUNCTION DECLARATIONS
// ============================================================

// main.c
void game_init(GameState *gs);
void game_tick(GameState *gs);
void game_render(GameState *gs, GSGLOBAL *gsGlobal);

// world.c
void world_generate(Level *level, int depth, unsigned int seed);
int  world_getTile(Level *level, int x, int y);
void world_setTile(Level *level, int x, int y, int tile, int data);
int  world_getData(Level *level, int x, int y);
int  world_isSolid(int tile);
int  world_isLiquid(int tile);

// player.c
void player_init(Entity *p, int x, int y);
void player_tick(GameState *gs);
void player_attack(GameState *gs);
void player_hurt(GameState *gs, int damage);
void player_addItem(GameState *gs, int item, int count);
int  player_hasItem(GameState *gs, int item, int count);
void player_removeItem(GameState *gs, int item, int count);

// entity.c
Entity* entity_spawn(GameState *gs, int type, int x, int y, int level);
void entity_tickAll(GameState *gs);
void entity_remove(GameState *gs, int idx);
void entity_hurt(GameState *gs, int idx, int damage);
void zombie_ai(GameState *gs, int idx);
void slime_ai(GameState *gs, int idx);
void airwizard_ai(GameState *gs, int idx);

// render.c
void render_tile(GSGLOBAL *g, int tile, int x, int y, int brightness);
void render_entity(GSGLOBAL *g, Entity *e, int camX, int camY);
void render_hud(GSGLOBAL *g, GameState *gs);
void render_inventory(GSGLOBAL *g, GameState *gs);
void render_menu(GSGLOBAL *g, GameState *gs);
void render_minimap(GSGLOBAL *g, GameState *gs);
void draw_rect(GSGLOBAL *g, int x, int y, int w, int h, u64 color);
void draw_text(GSGLOBAL *g, const char *str, int x, int y, u64 color);
void draw_char(GSGLOBAL *g, char c, int x, int y, u64 color);

// input.c
void input_init(void);
void input_update(GameState *gs);
int  input_pressed(GameState *gs, u32 button);
int  input_held(GameState *gs, u32 button);

// crafting.c
int  crafting_canCraft(GameState *gs, int recipe);
void crafting_doCraft(GameState *gs, int recipe);
void crafting_getResult(int recipe, int *item, int *count);
const char* crafting_getName(int recipe);
int  crafting_getRecipeCount(void);

// particles.c
void particle_spawn(GameState *gs, int x, int y, int vx, int vy, u32 color, int life);
void particle_tickAll(GameState *gs);

// utils.c
int  rng_next(void);
void rng_seed(unsigned int s);
int  rng_range(int min, int max);
int brightness_day(int dayTime);
const char* item_getName(int item);
u64  item_getColor(int item);

#endif // MINICRAFT_H
