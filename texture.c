// texture.c - Atlas texture loading and sprite drawing for PS2
#include "minicraft.h"

GSTEXTURE *g_atlas = NULL;

void atlas_load(GSGLOBAL *gsGlobal) {
    g_atlas = gsKit_texture_tga(gsGlobal, "mass:/atlas.tga");
    if (!g_atlas) {
        // Fallback - incercam de pe mc
        g_atlas = gsKit_texture_tga(gsGlobal, "mc0:/MINICRAFT/atlas.tga");
    }
}

// Deseneaza un sprite din atlas la pozitia (dx,dy) cu dimensiunea (dw,dh)
// sx,sy = pozitia in atlas, sw,sh = dimensiunea in atlas
void draw_sprite(GSGLOBAL *g, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
    if (!g_atlas) {
        // Fallback la dreptunghi gri daca nu e textura
        gsKit_prim_sprite(g, (float)dx, (float)dy, (float)(dx+dw), (float)(dy+dh),
                          1, GS_SETREG_RGBAQ(100,100,100,0x80,0));
        return;
    }

    float u0 = (float)sx / 512.0f;
    float v0 = (float)sy / 512.0f;
    float u1 = (float)(sx + sw) / 512.0f;
    float v1 = (float)(sy + sh) / 512.0f;

    gsKit_prim_sprite_texture(g, g_atlas,
        (float)dx,      (float)dy,
        u0,             v0,
        (float)(dx+dw), (float)(dy+dh),
        u1,             v1,
        1, GS_SETREG_RGBAQ(128,128,128,0x80,0));
}
