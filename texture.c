// texture.c - Atlas texture loading for PS2
#include "minicraft.h"

GSTEXTURE *g_atlas = NULL;
static GSTEXTURE atlas_tex;

void atlas_load(GSGLOBAL *gsGlobal) {
    g_atlas = NULL;

    // Incearca USB
    int ret = gsKit_texture_tga(gsGlobal, &atlas_tex, "mass:/atlas.tga");
    if (ret == 0) {
        g_atlas = &atlas_tex;
        gsKit_texture_upload(gsGlobal, g_atlas);
        return;
    }

    // Fallback memory card
    ret = gsKit_texture_tga(gsGlobal, &atlas_tex, "mc0:/MINICRAFT/atlas.tga");
    if (ret == 0) {
        g_atlas = &atlas_tex;
        gsKit_texture_upload(gsGlobal, g_atlas);
    }
    // Daca nu gaseste nimic, g_atlas ramane NULL si se foloseste fallback cu culori
}

void draw_sprite(GSGLOBAL *g, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
    if (!g_atlas) {
        gsKit_prim_sprite(g, (float)dx, (float)dy,
                          (float)(dx+dw), (float)(dy+dh),
                          1, GS_SETREG_RGBAQ(100,100,100,0x80,0));
        return;
    }

    float u0 = (float)sx / 512.0f;
    float v0 = (float)sy / 512.0f;
    float u1 = (float)(sx+sw) / 512.0f;
    float v1 = (float)(sy+sh) / 512.0f;

    gsKit_prim_sprite_texture(g, g_atlas,
        (float)dx,       (float)dy,       u0, v0,
        (float)(dx+dw),  (float)(dy+dh),  u1, v1,
        1, GS_SETREG_RGBAQ(128,128,128,0x80,0));
}
