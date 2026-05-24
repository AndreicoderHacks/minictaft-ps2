// render.c - Rendering via gsKit filled rectangles (pixel art style)
// All graphics are drawn with colored rectangles - no texture files needed!
#include "minicraft.h"

// ---- Minimal bitmap font (5x7, ASCII 32-127) ----
// Each char is 5 bytes, each byte is a row of 5 bits
static const u8 font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // 32 space
    {0x04,0x04,0x04,0x00,0x04}, // 33 !
    {0x0A,0x0A,0x00,0x00,0x00}, // 34 "
    {0x0A,0x1F,0x0A,0x1F,0x0A}, // 35 #
    {0x0E,0x14,0x0E,0x05,0x0E}, // 36 $
    {0x18,0x19,0x02,0x13,0x03}, // 37 %
    {0x0C,0x12,0x0C,0x15,0x0E}, // 38 &
    {0x04,0x04,0x00,0x00,0x00}, // 39 '
    {0x02,0x04,0x04,0x04,0x02}, // 40 (
    {0x08,0x04,0x04,0x04,0x08}, // 41 )
    {0x00,0x15,0x0E,0x15,0x00}, // 42 *
    {0x00,0x04,0x0E,0x04,0x00}, // 43 +
    {0x00,0x00,0x00,0x04,0x08}, // 44 ,
    {0x00,0x00,0x0E,0x00,0x00}, // 45 -
    {0x00,0x00,0x00,0x00,0x04}, // 46 .
    {0x01,0x02,0x04,0x08,0x10}, // 47 /
    {0x0E,0x13,0x15,0x19,0x0E}, // 48 0
    {0x04,0x0C,0x04,0x04,0x0E}, // 49 1
    {0x0E,0x01,0x0E,0x10,0x1F}, // 50 2
    {0x1F,0x02,0x06,0x01,0x1E}, // 51 3
    {0x12,0x12,0x1F,0x02,0x02}, // 52 4
    {0x1F,0x10,0x1E,0x01,0x1E}, // 53 5
    {0x06,0x08,0x1E,0x11,0x0E}, // 54 6
    {0x1F,0x01,0x02,0x04,0x04}, // 55 7
    {0x0E,0x11,0x0E,0x11,0x0E}, // 56 8
    {0x0E,0x11,0x0F,0x01,0x0E}, // 57 9
    {0x00,0x04,0x00,0x04,0x00}, // 58 :
    {0x00,0x04,0x00,0x04,0x08}, // 59 ;
    {0x02,0x04,0x08,0x04,0x02}, // 60 <
    {0x00,0x1F,0x00,0x1F,0x00}, // 61 =
    {0x08,0x04,0x02,0x04,0x08}, // 62 >
    {0x0E,0x01,0x06,0x00,0x04}, // 63 ?
    {0x0E,0x15,0x17,0x10,0x0E}, // 64 @
    {0x0E,0x11,0x1F,0x11,0x11}, // 65 A
    {0x1E,0x11,0x1E,0x11,0x1E}, // 66 B
    {0x0E,0x11,0x10,0x11,0x0E}, // 67 C
    {0x1C,0x12,0x11,0x12,0x1C}, // 68 D
    {0x1F,0x10,0x1E,0x10,0x1F}, // 69 E
    {0x1F,0x10,0x1E,0x10,0x10}, // 70 F
    {0x0E,0x10,0x17,0x11,0x0F}, // 71 G
    {0x11,0x11,0x1F,0x11,0x11}, // 72 H
    {0x0E,0x04,0x04,0x04,0x0E}, // 73 I
    {0x07,0x02,0x02,0x12,0x0C}, // 74 J
    {0x11,0x12,0x1C,0x12,0x11}, // 75 K
    {0x10,0x10,0x10,0x10,0x1F}, // 76 L
    {0x11,0x1B,0x15,0x11,0x11}, // 77 M
    {0x11,0x19,0x15,0x13,0x11}, // 78 N
    {0x0E,0x11,0x11,0x11,0x0E}, // 79 O
    {0x1E,0x11,0x1E,0x10,0x10}, // 80 P
    {0x0E,0x11,0x11,0x13,0x0F}, // 81 Q
    {0x1E,0x11,0x1E,0x12,0x11}, // 82 R
    {0x0E,0x10,0x0E,0x01,0x0E}, // 83 S
    {0x1F,0x04,0x04,0x04,0x04}, // 84 T
    {0x11,0x11,0x11,0x11,0x0E}, // 85 U
    {0x11,0x11,0x11,0x0A,0x04}, // 86 V
    {0x11,0x11,0x15,0x1B,0x11}, // 87 W
    {0x11,0x0A,0x04,0x0A,0x11}, // 88 X
    {0x11,0x0A,0x04,0x04,0x04}, // 89 Y
    {0x1F,0x02,0x04,0x08,0x1F}, // 90 Z
    // lowercase (simplified)
    {0x00,0x0E,0x11,0x1F,0x11}, // 97 a -> just reuse uppercase for simplicity
    {0x10,0x1E,0x11,0x11,0x1E}, // 98 b
    {0x00,0x0E,0x10,0x10,0x0E}, // 99 c
    {0x01,0x0F,0x11,0x11,0x0F}, // 100 d
    {0x0E,0x11,0x1F,0x10,0x0E}, // 101 e
    {0x06,0x08,0x1E,0x08,0x08}, // 102 f
    {0x0F,0x11,0x0F,0x01,0x0E}, // 103 g
    {0x10,0x1E,0x11,0x11,0x11}, // 104 h
    {0x04,0x00,0x04,0x04,0x04}, // 105 i
    {0x02,0x00,0x02,0x12,0x0C}, // 106 j
    {0x10,0x12,0x1C,0x12,0x11}, // 107 k
    {0x0C,0x04,0x04,0x04,0x0E}, // 108 l
    {0x00,0x1B,0x15,0x15,0x11}, // 109 m
    {0x00,0x1E,0x11,0x11,0x11}, // 110 n
    {0x00,0x0E,0x11,0x11,0x0E}, // 111 o
    {0x00,0x1E,0x11,0x1E,0x10}, // 112 p
    {0x00,0x0F,0x11,0x0F,0x01}, // 113 q
    {0x00,0x16,0x19,0x10,0x10}, // 114 r
    {0x00,0x0E,0x0C,0x02,0x1E}, // 115 s
    {0x08,0x1E,0x08,0x08,0x06}, // 116 t
    {0x00,0x11,0x11,0x11,0x0F}, // 117 u
    {0x00,0x11,0x11,0x0A,0x04}, // 118 v
    {0x00,0x11,0x15,0x15,0x0A}, // 119 w
    {0x00,0x11,0x0A,0x0A,0x11}, // 120 x
    {0x00,0x11,0x0F,0x01,0x0E}, // 121 y
    {0x00,0x1F,0x04,0x08,0x1F}, // 122 z
};

// ---- Draw helpers ----
void draw_rect(GSGLOBAL *g, int x, int y, int w, int h, u64 color) {
    gsKit_prim_sprite(g,
        (float)x, (float)y,
        (float)(x+w), (float)(y+h),
        1, color);
}

void draw_char(GSGLOBAL *g, char c, int x, int y, u64 color) {
    int idx = -1;
    if (c >= 32 && c <= 90) {
        idx = c - 32;
    } else if (c >= 97 && c <= 122) {
        idx = (c - 97) + 59;
    }
    if (idx < 0 || idx >= 85) return;
    const u8 *glyph = font5x7[idx];
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            if (glyph[row] & (1 << (4 - col))) {
                draw_rect(g, x + col*2, y + row*2, 2, 2, color);
            }
        }
    }
}
void draw_text(GSGLOBAL *g, const char *str, int x, int y, u64 color) {
    int cx = x;
    while (*str) {
        draw_char(g, *str, cx, y, color);
        cx += 12;
        str++;
    }
}

// ---- Utility helpers ----
float brightness_day(int dayTime) {
    // 0-2400 = day (1.0), 2400-3600 = dusk (1.0->0.3), 3600-4800 = night (0.3)
    if (dayTime < 2400) return 1.0f;
    if (dayTime < 3600) return 1.0f - 0.7f * (dayTime - 2400) / 1200.0f;
    return 0.3f;
}

const char* item_getName(int item) {
    static const char *names[] = {
        "None","Wood","Stone","Iron","Gold","Gem",
        "Wheat","Bread","Apple",
        "Wood Pick","Stone Pick","Iron Pick","Gold Pick","Gem Pick",
        "Wood Sword","Stone Sword","Iron Sword","Gold Sword","Gem Sword",
        "Workbench","Furnace","Lantern","Seeds","Coal"
    };
    if (item < 0 || item >= ITEM_COUNT) return "???";
    return names[item];
}

u64 item_getColor(int item) {
    switch(item) {
        case ITEM_WOOD:         return COL_BROWN;
        case ITEM_STONE:        return COL_GRAY;
        case ITEM_IRON:         return GS_SETREG_RGBAQ(180,180,200,0x80,0);
        case ITEM_GOLD:         return COL_YELLOW;
        case ITEM_GEM:          return COL_CYAN;
        case ITEM_WHEAT:        return GS_SETREG_RGBAQ(220,200,50,0x80,0);
        case ITEM_BREAD:        return GS_SETREG_RGBAQ(200,150,80,0x80,0);
        case ITEM_APPLE:        return COL_RED;
        case ITEM_COAL:         return COL_DARK_GRAY;
        case ITEM_WOOD_SWORD:
        case ITEM_WOOD_TOOL:    return COL_BROWN;
        case ITEM_STONE_SWORD:
        case ITEM_STONE_TOOL:   return COL_GRAY;
        case ITEM_IRON_SWORD:
        case ITEM_IRON_TOOL:    return GS_SETREG_RGBAQ(180,180,220,0x80,0);
        case ITEM_GOLD_SWORD:
        case ITEM_GOLD_TOOL:    return COL_YELLOW;
        case ITEM_GEM_SWORD:
        case ITEM_GEM_TOOL:     return COL_CYAN;
        case ITEM_WORKBENCH:    return COL_BROWN;
        case ITEM_FURNACE:      return COL_GRAY;
        case ITEM_LANTERN:      return COL_YELLOW;
        default:                return COL_WHITE;
    }
}

// ---- TILE RENDERER ----
// Each tile is drawn as a pattern of colored rectangles
void render_tile(GSGLOBAL *g, int tile, int px, int py, float brightness) {
    // Apply brightness: scale RGB (simplified - we just use a dark overlay)
    u64 col;
    int x = px, y = py, s = TILE_SIZE;

    switch(tile) {
        case TILE_GRASS:
            col = COL_DARK_GREEN;
            draw_rect(g, x, y, s, s, col);
            // Small detail
            if (((px/s + py/s) & 3) == 0)
                draw_rect(g, x+4, y+3, 2, 3, COL_GREEN);
            break;
        case TILE_DIRT:
            draw_rect(g, x, y, s, s, COL_DIRT_COL);
            break;
        case TILE_SAND:
            draw_rect(g, x, y, s, s, COL_SAND);
            break;
        case TILE_WATER:
            col = GS_SETREG_RGBAQ(
                30 + (int)(10 * brightness),
                80 + (int)(20 * brightness),
                200 + (int)(20 * brightness), 0x80, 0);
            draw_rect(g, x, y, s, s, col);
            // Wave detail
            draw_rect(g, x+2, y+6, 5, 2, COL_LIGHT_BLUE);
            draw_rect(g, x+9, y+11, 5, 2, COL_LIGHT_BLUE);
            break;
        case TILE_LAVA:
            draw_rect(g, x, y, s, s, COL_LAVA_COL);
            draw_rect(g, x+3, y+5, 4, 3, COL_ORANGE);
            draw_rect(g, x+9, y+10, 4, 3, COL_ORANGE);
            break;
        case TILE_STONE:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            draw_rect(g, x, y, s, 1, COL_DARK_GRAY);
            draw_rect(g, x, y, 1, s, COL_DARK_GRAY);
            break;
        case TILE_HARDROCK:
            draw_rect(g, x, y, s, s, COL_DARK_GRAY);
            draw_rect(g, x+2, y+2, s-4, s-4, GS_SETREG_RGBAQ(80,80,80,0x80,0));
            break;
        case TILE_TREE:
            draw_rect(g, x, y, s, s, COL_DARK_GREEN);
            // trunk
            draw_rect(g, x+6, y+8, 4, 8, COL_BROWN);
            // leaves
            draw_rect(g, x+2, y+2, 12, 10, COL_GREEN);
            break;
        case TILE_WOOD:
            draw_rect(g, x, y, s, s, COL_BROWN);
            draw_rect(g, x+2, y+2, s-4, s-4, GS_SETREG_RGBAQ(160,110,50,0x80,0));
            break;
        case TILE_FLOWER:
            draw_rect(g, x, y, s, s, COL_DARK_GREEN);
            // flower
            draw_rect(g, x+6, y+4, 4, 4, GS_SETREG_RGBAQ(255,120,180,0x80,0));
            draw_rect(g, x+7, y+5, 2, 2, COL_YELLOW);
            draw_rect(g, x+7, y+9, 1, 5, COL_GREEN);
            break;
        case TILE_CACTUS:
            draw_rect(g, x, y, s, s, COL_SAND);
            draw_rect(g, x+6, y+2, 4, 12, COL_DARK_GREEN);
            draw_rect(g, x+3, y+6, 3, 3, COL_DARK_GREEN);
            draw_rect(g, x+10, y+8, 3, 3, COL_DARK_GREEN);
            break;
        case TILE_ROCK_ORE:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            draw_rect(g, x+3, y+3, 4, 4, COL_GRAY);
            draw_rect(g, x+9, y+8, 4, 4, COL_GRAY);
            break;
        case TILE_IRON_ORE:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            draw_rect(g, x+3, y+3, 4, 4, GS_SETREG_RGBAQ(200,160,120,0x80,0));
            draw_rect(g, x+9, y+8, 4, 4, GS_SETREG_RGBAQ(200,160,120,0x80,0));
            break;
        case TILE_GOLD_ORE:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            draw_rect(g, x+3, y+3, 4, 4, COL_YELLOW);
            draw_rect(g, x+9, y+8, 4, 4, COL_YELLOW);
            break;
        case TILE_GEM_ORE:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            draw_rect(g, x+3, y+3, 4, 4, COL_CYAN);
            draw_rect(g, x+9, y+8, 4, 4, COL_CYAN);
            break;
        case TILE_STAIRS_DOWN:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            // Arrow down
            draw_rect(g, x+7, y+4, 2, 6, COL_DARK_GRAY);
            draw_rect(g, x+4, y+8, 8, 2, COL_DARK_GRAY);
            draw_rect(g, x+5, y+10, 6, 2, COL_DARK_GRAY);
            draw_rect(g, x+7, y+12, 2, 2, COL_DARK_GRAY);
            break;
        case TILE_STAIRS_UP:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            // Arrow up
            draw_rect(g, x+7, y+4, 2, 2, COL_DARK_GRAY);
            draw_rect(g, x+5, y+6, 6, 2, COL_DARK_GRAY);
            draw_rect(g, x+4, y+8, 8, 2, COL_DARK_GRAY);
            draw_rect(g, x+7, y+10, 2, 4, COL_DARK_GRAY);
            break;
        case TILE_CLOUD:
            draw_rect(g, x, y, s, s, GS_SETREG_RGBAQ(150,180,255,0x80,0));
            draw_rect(g, x+2, y+2, s-4, s-4, COL_WHITE);
            break;
        case TILE_HOLE:
            draw_rect(g, x, y, s, s, COL_BLACK);
            break;
        case TILE_WORKBENCH:
            draw_rect(g, x, y, s, s, COL_BROWN);
            draw_rect(g, x+2, y+2, s-4, 4, GS_SETREG_RGBAQ(160,90,30,0x80,0));
            draw_rect(g, x+2, y+8, s-4, 2, COL_DARK_GRAY);
            break;
        case TILE_FURNACE:
            draw_rect(g, x, y, s, s, COL_STONE_COL);
            draw_rect(g, x+4, y+4, 8, 8, COL_DARK_GRAY);
            draw_rect(g, x+5, y+5, 6, 6, COL_LAVA_COL);
            break;
        case TILE_FARMLAND:
            draw_rect(g, x, y, s, s, GS_SETREG_RGBAQ(100,60,20,0x80,0));
            draw_rect(g, x+4, y, 2, s, GS_SETREG_RGBAQ(80,40,10,0x80,0));
            break;
        case TILE_WHEAT:
            draw_rect(g, x, y, s, s, GS_SETREG_RGBAQ(100,60,20,0x80,0));
            draw_rect(g, x+4, y+2, 2, 10, GS_SETREG_RGBAQ(220,200,50,0x80,0));
            draw_rect(g, x+2, y+2, 3, 3, COL_DARK_GREEN);
            draw_rect(g, x+11, y+4, 3, 3, COL_DARK_GREEN);
            break;
        case TILE_PORTAL:
            draw_rect(g, x, y, s, s, COL_BLACK);
            draw_rect(g, x+2, y+2, s-4, s-4, COL_PURPLE);
            draw_rect(g, x+5, y+5, 6, 6, GS_SETREG_RGBAQ(180,80,255,0x80,0));
            break;
        default:
            draw_rect(g, x, y, s, s, COL_DARK_GRAY);
            break;
    }

    // Night darkening overlay
    if (brightness < 1.0f) {
        int alpha = (int)((1.0f - brightness) * 96);
        if (alpha > 96) alpha = 96;
        u64 night = GS_SETREG_RGBAQ(0, 0, 30, alpha, 0);
        draw_rect(g, x, y, s, s, night);
    }
}

// ---- ENTITY RENDERER ----
void render_entity(GSGLOBAL *g, Entity *e, int camX, int camY) {
    int sx = e->x - camX + SCREEN_W/2;
    int sy = e->y - camY + SCREEN_H/2 - 32; // viewport offset for HUD
    int s  = TILE_SIZE - 2;

    // Flash red when hurt
    u64 mainColor;
    int isHurt = (e->hurtTime > 0 && (e->hurtTime & 2));

    switch(e->type) {
        case ENT_ZOMBIE:
            mainColor = isHurt ? COL_RED : GS_SETREG_RGBAQ(40,120,40,0x80,0);
            // Body
            draw_rect(g, sx-5, sy-6, 10, 12, mainColor);
            // Head
            draw_rect(g, sx-4, sy-12, 8, 8, GS_SETREG_RGBAQ(60,160,60,0x80,0));
            // Eyes
            draw_rect(g, sx-2, sy-10, 2, 2, COL_RED);
            draw_rect(g, sx+2, sy-10, 2, 2, COL_RED);
            break;

        case ENT_SLIME:
            mainColor = isHurt ? COL_RED : GS_SETREG_RGBAQ(30,160,30,0x80,0);
            // Blob shape
            draw_rect(g, sx-6, sy-4, 12, 8, mainColor);
            draw_rect(g, sx-4, sy-6, 8, 10, mainColor);
            // Eyes
            draw_rect(g, sx-3, sy-3, 2, 2, COL_WHITE);
            draw_rect(g, sx+1, sy-3, 2, 2, COL_WHITE);
            break;

        case ENT_AIR_WIZARD:
            mainColor = isHurt ? COL_RED : COL_PURPLE;
            // Robe
            draw_rect(g, sx-8, sy-6, 16, 14, mainColor);
            // Head
            draw_rect(g, sx-5, sy-14, 10, 10, GS_SETREG_RGBAQ(200,180,140,0x80,0));
            // Hat
            draw_rect(g, sx-5, sy-18, 10, 6, COL_PURPLE);
            draw_rect(g, sx-3, sy-22, 6, 6, COL_PURPLE);
            // Eyes
            draw_rect(g, sx-3, sy-11, 2, 3, GS_SETREG_RGBAQ(255,100,255,0x80,0));
            draw_rect(g, sx+2, sy-11, 2, 3, GS_SETREG_RGBAQ(255,100,255,0x80,0));
            // Health bar above
            {
                int barW = 32;
                int hp = (e->health * barW) / e->maxHealth;
                draw_rect(g, sx - barW/2, sy-26, barW, 4, COL_DARK_GRAY);
                draw_rect(g, sx - barW/2, sy-26, hp,   4, COL_GREEN);
            }
            break;

        default:
            draw_rect(g, sx-4, sy-4, 8, 8, COL_WHITE);
            break;
    }
}

// ---- HUD ----
void render_hud(GSGLOBAL *g, GameState *gs) {
    Entity *p = &gs->player;
    int y = SCREEN_H - 40;

    // Black bar background
    draw_rect(g, 0, y, SCREEN_W, 40, COL_BLACK);
    draw_rect(g, 0, y, SCREEN_W, 1, COL_DARK_GRAY);

    // Health hearts
    for (int i = 0; i < p->maxHealth; i++) {
        u64 c = (i < p->health) ? COL_RED : COL_DARK_GRAY;
        draw_rect(g, 8 + i * 12, y + 6, 10, 10, c);
    }

    // Inventory slots
    for (int i = 0; i < INV_SIZE; i++) {
        int sx = SCREEN_W/2 - (INV_SIZE*26)/2 + i*26;
        int sy = y + 6;
        // Slot background
        u64 slotCol = (i == gs->selectedSlot) ? COL_YELLOW : COL_DARK_GRAY;
        draw_rect(g, sx, sy, 22, 22, slotCol);
        draw_rect(g, sx+1, sy+1, 20, 20, COL_BLACK);

        if (gs->inventory[i].type != ITEM_NONE) {
            // Item icon (colored square with count)
            draw_rect(g, sx+3, sy+3, 14, 14, item_getColor(gs->inventory[i].type));
            if (gs->inventory[i].count > 1) {
                char buf[4];
                buf[0] = '0' + (gs->inventory[i].count / 10);
                buf[1] = '0' + (gs->inventory[i].count % 10);
                buf[2] = 0;
                draw_text(g, buf, sx+2, sy+12, COL_WHITE);
            }
        }
    }

    // Current item name
    int sel = gs->selectedSlot;
    if (gs->inventory[sel].type != ITEM_NONE) {
        draw_text(g, item_getName(gs->inventory[sel].type), SCREEN_W - 160, y + 14, COL_WHITE);
    }

    // Day/night indicator
    {
        char timeStr[16];
        int day = gs->dayTime < 2400 ? 1 : 0;
        timeStr[0] = day ? 'D' : 'N';
        timeStr[1] = 'A';
        timeStr[2] = day ? 'Y' : 'I';
        timeStr[3] = ' ';
        timeStr[4] = 0;
        draw_text(g, day ? "DAY" : "NIGHT", 8, y - 16, day ? COL_YELLOW : COL_LIGHT_BLUE);
    }

    // Level indicator
    static const char *levelNames[] = { "Surface","Cave 1","Cave 2","Cave 3","Sky" };
    if (gs->currentLevel < NUM_LEVELS)
        draw_text(g, levelNames[gs->currentLevel], SCREEN_W/2 - 24, 8, COL_WHITE);
}

// ---- INVENTORY SCREEN ----
void render_inventory(GSGLOBAL *g, GameState *gs) {
    // Dark overlay
    for (int y2 = 0; y2 < SCREEN_H; y2 += 8)
        draw_rect(g, 0, y2, SCREEN_W, 4, GS_SETREG_RGBAQ(0,0,0,96,0));

    int bx = SCREEN_W/2 - 120;
    int by = SCREEN_H/2 - 80;
    draw_rect(g, bx, by, 240, 160, COL_DARK_GRAY);
    draw_rect(g, bx+2, by+2, 236, 156, COL_BLACK);
    draw_text(g, "INVENTORY", bx+60, by+8, COL_YELLOW);

    for (int i = 0; i < INV_SIZE; i++) {
        int sx = bx + 10 + (i % 3) * 75;
        int sy = by + 30 + (i / 3) * 40;
        u64 slotCol = (i == gs->selectedSlot) ? COL_YELLOW : COL_DARK_GRAY;
        draw_rect(g, sx, sy, 65, 30, slotCol);
        draw_rect(g, sx+1, sy+1, 63, 28, COL_BLACK);
        if (gs->inventory[i].type != ITEM_NONE) {
            draw_rect(g, sx+3, sy+3, 14, 14, item_getColor(gs->inventory[i].type));
            draw_text(g, item_getName(gs->inventory[i].type), sx+20, sy+8, COL_WHITE);
            if (gs->inventory[i].count > 1) {
                char buf[8] = { '0'+(gs->inventory[i].count/10), '0'+(gs->inventory[i].count%10), 0 };
                draw_text(g, buf, sx+3, sy+17, COL_YELLOW);
            }
        }
    }
    draw_text(g, "Triangle=Close  Cross=Use", bx+10, by+142, COL_GRAY);
}

// ---- CRAFTING SCREEN ----
static int craftingSelected = 0;
void render_crafting(GSGLOBAL *g, GameState *gs) {
    int bx = SCREEN_W/2 - 140;
    int by = 40;
    draw_rect(g, bx, by, 280, 360, COL_DARK_GRAY);
    draw_rect(g, bx+2, by+2, 276, 356, COL_BLACK);
    draw_text(g, "CRAFTING", bx+90, by+8, COL_YELLOW);

    int count = crafting_getRecipeCount();
    for (int i = 0; i < count && i < 12; i++) {
        int sy = by + 28 + i * 26;
        int canCraft = crafting_canCraft(gs, i);
        u64 col = (i == craftingSelected) ? COL_YELLOW :
                  (canCraft ? COL_WHITE : COL_DARK_GRAY);
        draw_rect(g, bx+4, sy, 272, 24, (i == craftingSelected) ? GS_SETREG_RGBAQ(40,40,0,0x80,0) : COL_BLACK);
        draw_text(g, crafting_getName(i), bx+8, sy+7, col);
        if (canCraft)
            draw_text(g, "OK", bx+240, sy+7, COL_GREEN);
    }
    draw_text(g, "Up/Down=Select  Cross=Craft  Tri=Close", bx+4, by+340, COL_GRAY);

    // Handle input for crafting
    if (input_pressed(gs, PAD_UP))    craftingSelected = (craftingSelected - 1 + count) % count;
    if (input_pressed(gs, PAD_DOWN))  craftingSelected = (craftingSelected + 1) % count;
    if (input_pressed(gs, PAD_CROSS)) crafting_doCraft(gs, craftingSelected);
    if (input_pressed(gs, PAD_TRIANGLE)) gs->state = STATE_PLAYING;
}

// ---- MENU ----
static int menuSelected = 0;
void render_menu(GSGLOBAL *g, GameState *gs) {
    draw_rect(g, 0, 0, SCREEN_W, SCREEN_H, COL_BLACK);

    // Counter vizual - clipeste ca sa stim ca jocul ruleaza
    static int counter = 0;
    counter++;
    int blink = (counter / 30) % 2;
    if (blink) draw_rect(g, 8, 8, 20, 20, COL_WHITE);
    else        draw_rect(g, 8, 8, 20, 20, COL_RED);

    // Title
    draw_text(g, "MINICRAFT", SCREEN_W/2 - 60, 100, COL_YELLOW);
    draw_text(g, "PS2 Edition", SCREEN_W/2 - 60, 120, COL_WHITE);
    draw_text(g, "Port by PS2 Homebrew", SCREEN_W/2 - 100, 140, COL_GRAY);

    static const char *opts[] = { "Start Game", "Quit" };
    for (int i = 0; i < 2; i++) {
        u64 col = (i == menuSelected) ? COL_YELLOW : COL_WHITE;
        draw_rect(g, SCREEN_W/2 - 80, 220 + i*40, 160, 30,
            (i == menuSelected) ? GS_SETREG_RGBAQ(40,40,0,0x80,0) : COL_DARK_GRAY);
        draw_text(g, opts[i], SCREEN_W/2 - 48, 228 + i*40, col);
    }
    draw_text(g, "Cross=Select", SCREEN_W/2-60, 320, COL_GRAY);

    if (input_pressed(gs, PAD_UP))   menuSelected = (menuSelected + 1) % 2;
    if (input_pressed(gs, PAD_DOWN)) menuSelected = (menuSelected + 1) % 2;
    if (input_pressed(gs, PAD_CROSS)) {
        if (menuSelected == 0) gs->state = STATE_PLAYING;
        else                   SleepThread(); // quit
    }
}

// ---- DEAD / WIN ----
static void render_dead(GSGLOBAL *g, GameState *gs) {
    draw_rect(g, 0, 0, SCREEN_W, SCREEN_H, GS_SETREG_RGBAQ(40,0,0,0x80,0));
    draw_text(g, "YOU DIED", SCREEN_W/2-50, 180, COL_RED);
    char score[32];
    score[0]='K'; score[1]='i'; score[2]='l'; score[3]='l'; score[4]='s'; score[5]=':';
    score[6]=' '; score[7]='0'+gs->monstersKilled/10; score[8]='0'+gs->monstersKilled%10; score[9]=0;
    draw_text(g, score, SCREEN_W/2-40, 220, COL_WHITE);
    draw_text(g, "Cross=Restart", SCREEN_W/2-70, 270, COL_YELLOW);
    if (input_pressed(gs, PAD_CROSS)) game_init(gs);
}

static void render_win(GSGLOBAL *g, GameState *gs) {
    draw_rect(g, 0, 0, SCREEN_W, SCREEN_H, GS_SETREG_RGBAQ(0,0,40,0x80,0));
    draw_text(g, "YOU WIN!", SCREEN_W/2-48, 160, COL_YELLOW);
    draw_text(g, "The Air Wizard is defeated!", SCREEN_W/2-130, 200, COL_WHITE);
    draw_text(g, "You are finally alone.", SCREEN_W/2-100, 230, COL_CYAN);
    draw_text(g, "Cross=Play Again", SCREEN_W/2-80, 290, COL_YELLOW);
    if (input_pressed(gs, PAD_CROSS)) game_init(gs);
}

// ---- MINIMAP ----
void render_minimap(GSGLOBAL *g, GameState *gs) {
    Level *lv = &gs->levels[gs->currentLevel];
    int mx = SCREEN_W - 68, my = 8;
    draw_rect(g, mx-2, my-2, 68, 68, COL_DARK_GRAY);
    for (int ty = 0; ty < 32; ty++) {
        for (int tx = 0; tx < 32; tx++) {
            int tile = world_getTile(lv, tx*2, ty*2);
            u64 c;
            switch(tile) {
                case TILE_WATER:    c = COL_BLUE;       break;
                case TILE_LAVA:     c = COL_LAVA_COL;   break;
                case TILE_SAND:     c = COL_SAND;        break;
                case TILE_STONE:    c = COL_GRAY;        break;
                case TILE_TREE:     c = COL_DARK_GREEN;  break;
                case TILE_CLOUD:    c = COL_WHITE;       break;
                case TILE_HOLE:     c = COL_BLACK;       break;
                default:            c = COL_DARK_GREEN;  break;
            }
            draw_rect(g, mx + tx*2, my + ty*2, 2, 2, c);
        }
    }
    // Player dot
    int px = (gs->player.x / TILE_SIZE) * 2 / 4;
    int py = (gs->player.y / TILE_SIZE) * 2 / 4;
    draw_rect(g, mx + px, my + py, 3, 3, COL_WHITE);
}

// ---- MAIN RENDER ----
void game_render(GameState *gs, GSGLOBAL *g) {
    gsKit_clear(g, COL_BLACK);

    if (gs->state == STATE_MENU) {
        render_menu(g, gs);
        return;
    }
    if (gs->state == STATE_DEAD) {
        render_dead(g, gs);
        return;
    }
    if (gs->state == STATE_WIN) {
        render_win(g, gs);
        return;
    }

    // ---- World render ----
    Level *lv = &gs->levels[gs->currentLevel];
    float bright = brightness_day(gs->dayTime);

    // Visible tiles
    int startX = (gs->camX - SCREEN_W/2) / TILE_SIZE - 1;
    int startY = (gs->camY - (SCREEN_H-40)/2) / TILE_SIZE - 1;
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    int endX = startX + SCREEN_W / TILE_SIZE + 2;
    int endY = startY + (SCREEN_H-40) / TILE_SIZE + 2;
    if (endX > WORLD_W) endX = WORLD_W;
    if (endY > WORLD_H) endY = WORLD_H;

    for (int ty = startY; ty < endY; ty++) {
        for (int tx = startX; tx < endX; tx++) {
            int tile = world_getTile(lv, tx, ty);
            int px = tx * TILE_SIZE - gs->camX + SCREEN_W/2;
            int py = ty * TILE_SIZE - gs->camY + (SCREEN_H-40)/2;
            render_tile(g, tile, px, py, bright);
        }
    }

    // Particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *pt = &gs->particles[i];
        if (pt->life <= 0) continue;
        int px = pt->x - gs->camX + SCREEN_W/2;
        int py = pt->y - gs->camY + (SCREEN_H-40)/2;
        draw_rect(g, px, py, 3, 3, pt->color);
    }

    // Entities on current level
    for (int i = 0; i < gs->entityCount; i++) {
        Entity *e = &gs->entities[i];
        if (!e->alive || e->level != gs->currentLevel) continue;
        render_entity(g, e, gs->camX, gs->camY);
    }

    // Player
    {
        Entity *p = &gs->player;
        int sx = SCREEN_W/2;
        int sy = (SCREEN_H-40)/2;
        u64 bodyCol = (p->hurtTime > 0 && (p->hurtTime & 2)) ? COL_RED : COL_WHITE;
        // Body
        draw_rect(g, sx-4, sy-6, 8, 12, bodyCol);
        // Head
        draw_rect(g, sx-4, sy-14, 8, 10, GS_SETREG_RGBAQ(220,180,140,0x80,0));
        // Eyes
        draw_rect(g, sx-2, sy-12, 2, 2, COL_BLACK);
        draw_rect(g, sx+1, sy-12, 2, 2, COL_BLACK);
        // Held item indicator
        int heldItem = gs->inventory[gs->selectedSlot].type;
        if (heldItem != ITEM_NONE) {
            u64 ic = item_getColor(heldItem);
            switch(p->dir) {
                case DIR_UP:    draw_rect(g, sx-1, sy-20, 4, 8, ic); break;
                case DIR_DOWN:  draw_rect(g, sx+4, sy+4, 4, 8, ic);  break;
                case DIR_LEFT:  draw_rect(g, sx-12, sy-2, 8, 4, ic); break;
                case DIR_RIGHT: draw_rect(g, sx+4, sy-2, 8, 4, ic);  break;
            }
        }
    }

    // HUD
    render_hud(g, gs);

    // Minimap
    render_minimap(g, gs);

    // Inventory / crafting overlays
    if (gs->state == STATE_INVENTORY) render_inventory(g, gs);
    if (gs->state == STATE_CRAFTING)  render_crafting(g, gs);
}
