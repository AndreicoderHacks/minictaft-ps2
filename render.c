// render.c - Rendering via gsKit filled rectangles
#include "minicraft.h"

// ---- Minimal bitmap font 5x5 ----
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
    {0x00,0x0E,0x11,0x1F,0x11}, // 91 = a
    {0x10,0x1E,0x11,0x11,0x1E}, // 92 = b
    {0x00,0x0E,0x10,0x10,0x0E}, // 93 = c
    {0x01,0x0F,0x11,0x11,0x0F}, // 94 = d
    {0x0E,0x11,0x1F,0x10,0x0E}, // 95 = e
    {0x06,0x08,0x1E,0x08,0x08}, // 96 = f
    {0x0F,0x11,0x0F,0x01,0x0E}, // 97 = g
    {0x10,0x1E,0x11,0x11,0x11}, // 98 = h
    {0x04,0x00,0x04,0x04,0x04}, // 99 = i
    {0x02,0x00,0x02,0x12,0x0C}, // 100 = j
    {0x10,0x12,0x1C,0x12,0x11}, // 101 = k
    {0x0C,0x04,0x04,0x04,0x0E}, // 102 = l
    {0x00,0x1B,0x15,0x15,0x11}, // 103 = m
    {0x00,0x1E,0x11,0x11,0x11}, // 104 = n
    {0x00,0x0E,0x11,0x11,0x0E}, // 105 = o
    {0x00,0x1E,0x11,0x1E,0x10}, // 106 = p
    {0x00,0x0F,0x11,0x0F,0x01}, // 107 = q
    {0x00,0x16,0x19,0x10,0x10}, // 108 = r
    {0x00,0x0E,0x0C,0x02,0x1E}, // 109 = s
    {0x08,0x1E,0x08,0x08,0x06}, // 110 = t
    {0x00,0x11,0x11,0x11,0x0F}, // 111 = u
    {0x00,0x11,0x11,0x0A,0x04}, // 112 = v
    {0x00,0x11,0x15,0x15,0x0A}, // 113 = w
    {0x00,0x11,0x0A,0x0A,0x11}, // 114 = x
    {0x00,0x11,0x0F,0x01,0x0E}, // 115 = y
    {0x00,0x1F,0x04,0x08,0x1F}, // 116 = z
};
#define FONT_COUNT 117

// ---- Draw helpers ----
void draw_rect(GSGLOBAL *g, int x, int y, int w, int h, u64 color) {
    if (x >= SCREEN_W || y >= SCREEN_H) return;
    if (x + w < 0 || y + h < 0) return;
    if (w <= 0 || h <= 0) return;
    gsKit_prim_sprite(g,
        (float)x, (float)y,
        (float)(x + w), (float)(y + h),
        1, color);
}

void draw_char(GSGLOBAL *g, char c, int x, int y, u64 color) {
    int idx = -1;
    if (c >= 32 && c <= 90)       idx = c - 32;
    else if (c >= 97 && c <= 122) idx = (c - 97) + 59;
    if (idx < 0 || idx >= FONT_COUNT) return;
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

// ---- Utility ----
int brightness_day(int dayTime) {
    // Returns 0-100 (integer, no float)
    if (dayTime < 2400) return 100;
    if (dayTime < 3600) return 100 - (dayTime - 2400) * 70 / 1200;
    return 30;
}

const char* item_getName(int item) {
    static const char *names[] = {
        "None","Wood","Stone","Iron","Gold","Gem",
        "Wheat","Bread","Apple",
        "Wood Pick","Stone Pick","Iron Pick","Gold Pick","Gem Pick",
        "Wood Sword","Stone Sword","Iron Sword","Gold Sword","Gem Sword",
        "Workbench","Furnace","Lantern","Seeds","Coal","Bed"
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
        case ITEM_BED:          return GS_SETREG_RGBAQ(180,40,60,0x80,0);
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
        default:                return COL_WHITE;
    }
}

static void draw_item_icon(GSGLOBAL *g, int item, int x, int y) {
    u64 c = item_getColor(item);
    switch(item) {
        case ITEM_WOOD:
            draw_rect(g, x+2, y+2, 12, 12, c);
            draw_rect(g, x+2, y+6, 12, 2, GS_SETREG_RGBAQ(90,55,25,0x80,0));
            draw_rect(g, x+7, y+2, 2, 12, GS_SETREG_RGBAQ(170,105,45,0x80,0));
            break;
        case ITEM_STONE:
            draw_rect(g, x+2, y+4, 12, 9, c);
            draw_rect(g, x+4, y+3, 8, 2, GS_SETREG_RGBAQ(190,190,190,0x80,0));
            break;
        case ITEM_APPLE:
            draw_rect(g, x+4, y+5, 8, 8, COL_RED);
            draw_rect(g, x+7, y+3, 2, 3, COL_BROWN);
            draw_rect(g, x+9, y+4, 3, 2, COL_GREEN);
            break;
        case ITEM_BREAD:
            draw_rect(g, x+3, y+6, 10, 6, c);
            draw_rect(g, x+5, y+4, 6, 3, GS_SETREG_RGBAQ(230,180,95,0x80,0));
            break;
        case ITEM_WOOD_TOOL:
        case ITEM_STONE_TOOL:
        case ITEM_IRON_TOOL:
        case ITEM_GOLD_TOOL:
        case ITEM_GEM_TOOL:
            draw_rect(g, x+3, y+4, 10, 3, c);
            draw_rect(g, x+7, y+6, 3, 8, COL_BROWN);
            break;
        case ITEM_WOOD_SWORD:
        case ITEM_STONE_SWORD:
        case ITEM_IRON_SWORD:
        case ITEM_GOLD_SWORD:
        case ITEM_GEM_SWORD:
            draw_rect(g, x+7, y+2, 3, 10, c);
            draw_rect(g, x+4, y+10, 9, 2, COL_BROWN);
            draw_rect(g, x+7, y+12, 3, 3, COL_BROWN);
            break;
        case ITEM_WORKBENCH:
            draw_rect(g, x+2, y+2, 12, 12, COL_BROWN);
            draw_rect(g, x+2, y+7, 12, 2, GS_SETREG_RGBAQ(90,55,25,0x80,0));
            draw_rect(g, x+7, y+2, 2, 12, GS_SETREG_RGBAQ(90,55,25,0x80,0));
            break;
        case ITEM_FURNACE:
            draw_rect(g, x+2, y+2, 12, 12, COL_STONE_COL);
            draw_rect(g, x+4, y+5, 8, 5, COL_BLACK);
            break;
        case ITEM_BED:
            draw_rect(g, x+2, y+5, 12, 7, GS_SETREG_RGBAQ(170,40,55,0x80,0));
            draw_rect(g, x+2, y+3, 5, 4, COL_WHITE);
            draw_rect(g, x+2, y+12, 12, 2, COL_BROWN);
            break;
        default:
            draw_rect(g, x+3, y+3, 10, 10, c);
            break;
    }
}

static void draw_count(GSGLOBAL *g, int count, int x, int y) {
    if (count <= 1) return;
    char buf[4];
    if (count > 99) count = 99;
    sprintf(buf, "%d", count);
    draw_text(g, buf, x, y, COL_WHITE);
}

// ---- TILE RENDERER ----
void render_tile(GSGLOBAL *g, int tile, int px, int py, int bright) {
    int x = px, y = py, s = TILE_SIZE;
    u64 col;

    switch(tile) {
        case TILE_GRASS:     col = (bright < 70) ? GS_SETREG_RGBAQ(8,45,18,0x80,0) : COL_DARK_GREEN; break;
        case TILE_DIRT:      col = (bright < 70) ? GS_SETREG_RGBAQ(70,45,30,0x80,0) : COL_DIRT_COL;   break;
        case TILE_SAND:      col = (bright < 70) ? GS_SETREG_RGBAQ(105,95,65,0x80,0) : COL_SAND;      break;
        case TILE_WATER:     col = (bright < 70) ? GS_SETREG_RGBAQ(15,30,95,0x80,0) : COL_BLUE;       break;
        case TILE_LAVA:      col = COL_LAVA_COL;    break;
        case TILE_STONE:     col = (bright < 70) ? GS_SETREG_RGBAQ(75,75,85,0x80,0) : COL_STONE_COL;  break;
        case TILE_HARDROCK:  col = COL_DARK_GRAY;   break;
        case TILE_TREE:      col = (bright < 70) ? GS_SETREG_RGBAQ(15,70,20,0x80,0) : COL_GREEN;      break;
        case TILE_WOOD:      col = COL_BROWN;        break;
        case TILE_ROCK_ORE:  col = COL_GRAY;         break;
        case TILE_IRON_ORE:  col = GS_SETREG_RGBAQ(200,160,120,0x80,0); break;
        case TILE_GOLD_ORE:  col = COL_YELLOW;       break;
        case TILE_GEM_ORE:   col = COL_CYAN;         break;
        case TILE_STAIRS_DOWN: col = GS_SETREG_RGBAQ(180,140,80,0x80,0); break;
        case TILE_STAIRS_UP:   col = GS_SETREG_RGBAQ(220,200,120,0x80,0); break;
        case TILE_HOLE:      col = COL_BLACK;        break;
        case TILE_WORKBENCH: col = COL_BROWN;        break;
        case TILE_FURNACE:   col = COL_STONE_COL;    break;
        case TILE_FARMLAND:  col = GS_SETREG_RGBAQ(100,60,20,0x80,0); break;
        case TILE_WHEAT:     col = COL_YELLOW;       break;
        case TILE_PORTAL:    col = COL_PURPLE;       break;
        case TILE_CLOUD:     col = COL_WHITE;        break;
        default:             col = COL_DARK_GRAY;    break;
    }

    draw_rect(g, x, y, s, s, col);

    // Detalii pixel-art ieftine. Sunt intentionat putine, ca sa ramana stabil pe PS2.
    int h = ((x >> 4) * 37 + (y >> 4) * 17) & 7;
    switch(tile) {
        case TILE_GRASS:
            if (h == 0) {
                draw_rect(g, x+3, y+4, 2, 2, GS_SETREG_RGBAQ(70,150,45,0x80,0));
                draw_rect(g, x+10, y+11, 2, 2, GS_SETREG_RGBAQ(35,120,30,0x80,0));
            } else if (h == 3) {
                draw_rect(g, x+6, y+8, 2, 2, GS_SETREG_RGBAQ(80,160,55,0x80,0));
            }
            break;
        case TILE_DIRT:
            if (h < 3) draw_rect(g, x+4+h, y+5, 4, 2, GS_SETREG_RGBAQ(110,70,35,0x80,0));
            break;
        case TILE_SAND:
            if (h < 3) draw_rect(g, x+3+h*3, y+4+h, 2, 2, GS_SETREG_RGBAQ(245,225,130,0x80,0));
            break;
        case TILE_WATER:
            draw_rect(g, x, y, s, 2, GS_SETREG_RGBAQ(70,100,230,0x80,0));
            if (h < 4) draw_rect(g, x+3+h*2, y+7, 5, 1, GS_SETREG_RGBAQ(90,140,255,0x80,0));
            break;
        case TILE_STONE:
        case TILE_HARDROCK:
            draw_rect(g, x, y, s, 2, GS_SETREG_RGBAQ(190,190,190,0x80,0));
            if (h < 3) draw_rect(g, x+3+h*2, y+9, 5, 2, GS_SETREG_RGBAQ(100,100,100,0x80,0));
            break;
        case TILE_TREE:
            draw_rect(g, x+6, y+8, 4, 8, COL_BROWN);
            draw_rect(g, x+2, y+2, 12, 10, GS_SETREG_RGBAQ(35,145,35,0x80,0));
            draw_rect(g, x+5, y, 8, 8, GS_SETREG_RGBAQ(55,175,55,0x80,0));
            break;
        case TILE_WOOD:
            draw_rect(g, x, y+3, s, 2, GS_SETREG_RGBAQ(95,55,25,0x80,0));
            draw_rect(g, x, y+10, s, 2, GS_SETREG_RGBAQ(95,55,25,0x80,0));
            draw_rect(g, x+6, y, 2, s, GS_SETREG_RGBAQ(170,105,45,0x80,0));
            break;
        case TILE_WORKBENCH:
            draw_rect(g, x+2, y+2, 12, 12, GS_SETREG_RGBAQ(170,105,45,0x80,0));
            draw_rect(g, x+2, y+7, 12, 2, GS_SETREG_RGBAQ(90,55,25,0x80,0));
            draw_rect(g, x+7, y+2, 2, 12, GS_SETREG_RGBAQ(90,55,25,0x80,0));
            break;
        case TILE_FURNACE:
            draw_rect(g, x+3, y+3, 10, 10, COL_DARK_GRAY);
            draw_rect(g, x+5, y+6, 6, 4, COL_BLACK);
            break;
        case TILE_STAIRS_DOWN:
        case TILE_STAIRS_UP:
            draw_rect(g, x+3, y+4, 10, 2, COL_BROWN);
            draw_rect(g, x+5, y+8, 8, 2, COL_BROWN);
            draw_rect(g, x+7, y+12, 6, 2, COL_BROWN);
            break;
        default:
            break;
    }
}

// ---- ENTITY RENDERER ----
void render_entity(GSGLOBAL *g, Entity *e, int camX, int camY) {
    int sx = e->x - camX + SCREEN_W/2;
    int sy = e->y - camY + (SCREEN_H-40)/2;

    // Skip if offscreen
    if (sx < -20 || sx > SCREEN_W+20) return;
    if (sy < -20 || sy > SCREEN_H+20) return;

    u64 mainColor;
    int isHurt = (e->hurtTime > 0 && (e->hurtTime & 2));

    draw_rect(g, sx-6, sy+5, 12, 3, GS_SETREG_RGBAQ(0,0,0,0x50,0));

    switch(e->type) {
        case ENT_ZOMBIE:
            mainColor = isHurt ? COL_RED : GS_SETREG_RGBAQ(40,120,40,0x80,0);
            draw_rect(g, sx-5, sy-6, 10, 12, mainColor);
            draw_rect(g, sx-4, sy-12, 8, 8, GS_SETREG_RGBAQ(60,160,60,0x80,0));
            draw_rect(g, sx-2, sy-10, 2, 2, COL_RED);
            draw_rect(g, sx+2, sy-10, 2, 2, COL_RED);
            break;
        case ENT_SLIME:
            mainColor = isHurt ? COL_RED : GS_SETREG_RGBAQ(30,160,30,0x80,0);
            draw_rect(g, sx-6, sy-4, 12, 8, mainColor);
            draw_rect(g, sx-4, sy-6, 8, 10, mainColor);
            break;
        case ENT_AIR_WIZARD:
            mainColor = isHurt ? COL_RED : COL_PURPLE;
            draw_rect(g, sx-8, sy-6, 16, 14, mainColor);
            draw_rect(g, sx-5, sy-14, 10, 10, GS_SETREG_RGBAQ(200,180,140,0x80,0));
            // Health bar
            {
                int barW = 32;
                int hp = (e->health * barW) / e->maxHealth;
                if (hp < 0) hp = 0;
                draw_rect(g, sx-barW/2, sy-26, barW, 4, COL_DARK_GRAY);
                draw_rect(g, sx-barW/2, sy-26, hp, 4, COL_GREEN);
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

    draw_rect(g, 0, y, SCREEN_W, 40, COL_BLACK);
    draw_rect(g, 0, y, SCREEN_W, 1, COL_DARK_GRAY);

    // Health
    for (int i = 0; i < p->maxHealth; i++) {
        u64 c = (i < p->health) ? COL_RED : COL_DARK_GRAY;
        draw_rect(g, 8 + i*12, y+6, 10, 10, c);
    }

    // Inventory slots
    for (int i = 0; i < INV_SIZE; i++) {
        int sx = SCREEN_W/2 - (INV_SIZE*26)/2 + i*26;
        int sy = y + 6;
        u64 slotCol = (i == gs->selectedSlot) ? COL_YELLOW : COL_DARK_GRAY;
        draw_rect(g, sx, sy, 22, 22, slotCol);
        draw_rect(g, sx+1, sy+1, 20, 20, COL_BLACK);
        if (gs->inventory[i].type != ITEM_NONE) {
            draw_item_icon(g, gs->inventory[i].type, sx+3, sy+3);
            draw_count(g, gs->inventory[i].count, sx+12, sy+14);
        }
    }

    // Level name
    static const char *levelNames[] = {"Surface","Cave 1","Cave 2","Cave 3","Sky"};
    if (gs->currentLevel < NUM_LEVELS)
        draw_text(g, levelNames[gs->currentLevel], SCREEN_W/2-30, 8, COL_WHITE);

    // Day/night
    draw_text(g, (gs->dayTime < 2400) ? "DAY" : "NIGHT",
              8, y-16, (gs->dayTime < 2400) ? COL_YELLOW : COL_LIGHT_BLUE);
}

// ---- INVENTORY ----
void render_inventory(GSGLOBAL *g, GameState *gs) {
    // Background simplu
    draw_rect(g, 100, 100, 440, 250, COL_DARK_GRAY);
    draw_rect(g, 102, 102, 436, 246, COL_BLACK);
    draw_text(g, "INVENTORY", 270, 110, COL_YELLOW);

    // Doar text, fara multe dreptunghiuri
    for (int i = 0; i < INV_SIZE; i++) {
        int sy2 = 130 + i * 24;
        if (gs->inventory[i].type != ITEM_NONE) {
            if (i == gs->selectedSlot)
                draw_rect(g, 110, sy2, 420, 20, GS_SETREG_RGBAQ(0,80,0,0x80,0));
            draw_item_icon(g, gs->inventory[i].type, 120, sy2+2);
            draw_text(g, item_getName(gs->inventory[i].type), 145, sy2+4, COL_WHITE);
            draw_count(g, gs->inventory[i].count, 470, sy2+4);
        } else {
            if (i == gs->selectedSlot)
                draw_rect(g, 110, sy2, 420, 20, GS_SETREG_RGBAQ(40,40,0,0x80,0));
        }
    }
    draw_text(g, "Tri=Close", 270, 330, COL_GRAY);

    if (input_pressed(gs, PAD_TRIANGLE) && gs->stateTimer == 0) {
        gs->state = STATE_PLAYING;
        gs->stateTimer = 20;
    }
}

// ---- CRAFTING ----
static int craftingSelected = 0;
void render_crafting(GSGLOBAL *g, GameState *gs) {
    int bx = SCREEN_W/2 - 140;
    int by = 40;
    draw_rect(g, bx, by, 280, 360, COL_DARK_GRAY);
    draw_rect(g, bx+2, by+2, 276, 356, COL_BLACK);
    draw_text(g, "CRAFTING", bx+90, by+8, COL_YELLOW);

    int count = crafting_getRecipeCount();
    for (int i = 0; i < count && i < 15; i++) {
        int sy2 = by + 28 + i*20;
        int canCraft = crafting_canCraft(gs, i);
        u64 col = (i == craftingSelected) ? COL_YELLOW :
                  (canCraft ? COL_WHITE : COL_DARK_GRAY);
        if (i == craftingSelected)
            draw_rect(g, bx+4, sy2, 272, 18, GS_SETREG_RGBAQ(40,40,0,0x80,0));
        draw_text(g, crafting_getName(i), bx+8, sy2+4, col);
    }
    draw_text(g, "X=Craft  Tri=Close", bx+60, by+340, COL_GRAY);

    if (input_pressed(gs, PAD_UP))
        craftingSelected = (craftingSelected - 1 + count) % count;
    if (input_pressed(gs, PAD_DOWN))
        craftingSelected = (craftingSelected + 1) % count;
    if (input_pressed(gs, PAD_CROSS))
        crafting_doCraft(gs, craftingSelected);
    if (input_pressed(gs, PAD_TRIANGLE))
        gs->state = STATE_PLAYING;
}

// ---- MENU ----
static int menuSelected = 0;
void render_menu(GSGLOBAL *g, GameState *gs) {
    draw_rect(g, 0, 0, SCREEN_W, SCREEN_H, COL_BLACK);

    // Blink counter
    static int counter = 0;
    counter++;
    if ((counter / 30) % 2)
        draw_rect(g, 8, 8, 20, 20, COL_WHITE);
    else
        draw_rect(g, 8, 8, 20, 20, COL_RED);

    draw_text(g, "MINICRAFT", SCREEN_W/2 - 54, 100, COL_YELLOW);
    draw_text(g, "PS2 Edition", SCREEN_W/2 - 66, 120, COL_WHITE);

    static const char *opts[] = { "Start Game", "Quit" };
    for (int i = 0; i < 2; i++) {
        if (i == menuSelected)
            draw_rect(g, SCREEN_W/2-80, 200+i*40, 160, 28, GS_SETREG_RGBAQ(0,100,0,0x80,0));
        else
            draw_rect(g, SCREEN_W/2-80, 200+i*40, 160, 28, COL_DARK_GRAY);
        draw_text(g, opts[i], SCREEN_W/2-48, 207+i*40,
            (i == menuSelected) ? COL_YELLOW : COL_WHITE);
    }
    draw_text(g, "Cross=Select", SCREEN_W/2-72, 300, COL_GRAY);

    if (input_pressed(gs, PAD_UP))
        menuSelected = (menuSelected + 1) % 2;
    if (input_pressed(gs, PAD_DOWN))
        menuSelected = (menuSelected + 1) % 2;
    if (input_pressed(gs, PAD_CROSS)) {
        if (menuSelected == 0) gs->state = STATE_PLAYING;
        // Quit - nu facem nimic, doar ignoram
    }
}

// ---- DEAD / WIN ----
static void render_dead(GSGLOBAL *g, GameState *gs) {
    draw_rect(g, 0, 0, SCREEN_W, SCREEN_H, GS_SETREG_RGBAQ(40,0,0,0x80,0));
    draw_text(g, "YOU DIED", SCREEN_W/2-50, 180, COL_RED);
    draw_text(g, "Cross=Restart", SCREEN_W/2-78, 240, COL_YELLOW);
    if (input_pressed(gs, PAD_CROSS)) game_init(gs);
}

static void render_win(GSGLOBAL *g, GameState *gs) {
    draw_rect(g, 0, 0, SCREEN_W, SCREEN_H, GS_SETREG_RGBAQ(0,0,40,0x80,0));
    draw_text(g, "YOU WIN!", SCREEN_W/2-48, 160, COL_YELLOW);
    draw_text(g, "Cross=Play Again", SCREEN_W/2-96, 240, COL_YELLOW);
    if (input_pressed(gs, PAD_CROSS)) game_init(gs);
}

// ---- MINIMAP ----
void render_minimap(GSGLOBAL *g, GameState *gs) {
    Level *lv = &gs->levels[gs->currentLevel];
    int mx = SCREEN_W - 68, my = 8;
    draw_rect(g, mx-2, my-2, 68, 68, COL_DARK_GRAY);
    draw_rect(g, mx, my, 64, 64, COL_BLACK);

    // Desenam doar la fiecare 4 tile-uri ca sa reducem draw calls
    for (int ty = 0; ty < WORLD_H; ty += 4) {
        for (int tx = 0; tx < WORLD_W; tx += 4) {
            int tile = world_getTile(lv, tx, ty);
            u64 c;
            switch(tile) {
                case TILE_WATER:  c = COL_BLUE;      break;
                case TILE_LAVA:   c = COL_LAVA_COL;  break;
                case TILE_SAND:   c = COL_SAND;       break;
                case TILE_STONE:  c = COL_GRAY;       break;
                case TILE_TREE:   c = COL_DARK_GREEN; break;
                case TILE_HOLE:   c = COL_BLACK;      break;
                default:          c = COL_DARK_GREEN; break;
            }
            draw_rect(g, mx + tx, my + ty, 4, 4, c);
        }
    }
    // Player dot
    int pdx = gs->player.x / TILE_SIZE;
    int pdy = gs->player.y / TILE_SIZE;
    if (pdx >= 0 && pdx < 64 && pdy >= 0 && pdy < 64)
        draw_rect(g, mx+pdx, my+pdy, 3, 3, COL_WHITE);
}

// ---- PAUSE ----
static void render_pause(GSGLOBAL *g, GameState *gs) {
    draw_rect(g, SCREEN_W/2-80, SCREEN_H/2-30, 160, 60, COL_DARK_GRAY);
    draw_rect(g, SCREEN_W/2-78, SCREEN_H/2-28, 156, 56, COL_BLACK);
    draw_text(g, "PAUSED", SCREEN_W/2-36, SCREEN_H/2-18, COL_YELLOW);
    draw_text(g, "Start=Resume", SCREEN_W/2-72, SCREEN_H/2+4, COL_WHITE);
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

    // World
    Level *lv = &gs->levels[gs->currentLevel];
    int bright = brightness_day(gs->dayTime);

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
            int px2 = tx * TILE_SIZE - gs->camX + SCREEN_W/2;
            int py2 = ty * TILE_SIZE - gs->camY + (SCREEN_H-40)/2;
            render_tile(g, tile, px2, py2, bright);
        }
    }

    // Cursor pe blocul din fata playerului: vezi exact ce minezi/plasezi.
    {
        int tx = gs->player.x / TILE_SIZE;
        int ty = gs->player.y / TILE_SIZE;
        switch(gs->player.dir) {
            case DIR_UP:    ty--; break;
            case DIR_DOWN:  ty++; break;
            case DIR_LEFT:  tx--; break;
            case DIR_RIGHT: tx++; break;
        }
        if (tx >= 0 && tx < WORLD_W && ty >= 0 && ty < WORLD_H) {
            int cx = tx * TILE_SIZE - gs->camX + SCREEN_W/2;
            int cy = ty * TILE_SIZE - gs->camY + (SCREEN_H-40)/2;
            u64 cc = (gs->tickCount & 16) ? COL_YELLOW : COL_WHITE;
            draw_rect(g, cx, cy, TILE_SIZE, 1, cc);
            draw_rect(g, cx, cy+TILE_SIZE-1, TILE_SIZE, 1, cc);
            draw_rect(g, cx, cy, 1, TILE_SIZE, cc);
            draw_rect(g, cx+TILE_SIZE-1, cy, 1, TILE_SIZE, cc);
        }
    }

    // Entities
    for (int i = 0; i < gs->entityCount; i++) {
        Entity *e = &gs->entities[i];
        if (!e->alive || e->level != gs->currentLevel) continue;
        render_entity(g, e, gs->camX, gs->camY);
    }

    // Player
    {
        Entity *p = &gs->player;
        int sx2 = p->x - gs->camX + SCREEN_W/2;
        int sy2 = p->y - gs->camY + (SCREEN_H-40)/2;
        u64 shirtCol = (p->hurtTime > 0 && (p->hurtTime & 2)) ? COL_RED : GS_SETREG_RGBAQ(70,120,220,0x80,0);
        u64 skinCol = GS_SETREG_RGBAQ(220,180,140,0x80,0);
        draw_rect(g, sx2-7, sy2+8, 14, 3, GS_SETREG_RGBAQ(0,0,0,0x50,0));
        draw_rect(g, sx2-5, sy2-5, 10, 11, shirtCol);
        draw_rect(g, sx2-4, sy2+5, 3, 5, GS_SETREG_RGBAQ(45,65,130,0x80,0));
        draw_rect(g, sx2+1, sy2+5, 3, 5, GS_SETREG_RGBAQ(45,65,130,0x80,0));
        draw_rect(g, sx2-5, sy2-14, 10, 9, skinCol);
        draw_rect(g, sx2-5, sy2-15, 10, 3, GS_SETREG_RGBAQ(70,45,20,0x80,0));
        draw_rect(g, sx2-2, sy2-11, 2, 2, COL_BLACK);
        draw_rect(g, sx2+2, sy2-11, 2, 2, COL_BLACK);
        if (p->dir == DIR_LEFT)  draw_rect(g, sx2-6, sy2-1, 2, 4, skinCol);
        if (p->dir == DIR_RIGHT) draw_rect(g, sx2+4, sy2-1, 2, 4, skinCol);
        if (p->dir == DIR_UP)    draw_rect(g, sx2-3, sy2-15, 6, 2, GS_SETREG_RGBAQ(45,30,15,0x80,0));
    }

    // HUD
    render_hud(g, gs);

    // Minimap oprit temporar pentru stabilitate pe PS2 real.
    // render_minimap(g, gs);

    // Overlays
    if (gs->state == STATE_INVENTORY) render_inventory(g, gs);
    if (gs->state == STATE_CRAFTING)  render_crafting(g, gs);
    if (gs->state == STATE_PAUSE)     render_pause(g, gs);
}
