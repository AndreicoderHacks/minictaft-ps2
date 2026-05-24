#include "minicraft.h"

void game_init(GameState *gs);
void game_tick(GameState *gs);
void game_render(GameState *gs, GSGLOBAL *gsGlobal);

static GameState gs;

int main(void) {
    SifInitRpc(0);

    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode           = GS_MODE_NTSC;
    gsGlobal->Width          = SCREEN_W;
    gsGlobal->Height         = SCREEN_H;
    gsGlobal->PSM            = GS_PSM_CT32;
    gsGlobal->PSMZ           = GS_PSMZ_16;
    gsGlobal->ZBuffering     = GS_SETTING_OFF;
    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
                D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

    input_init();

    rng_seed(0xCAFE1234);
    game_init(&gs);

    while(1) {
        input_update(&gs);
        game_tick(&gs);
        game_render(&gs, gsGlobal);
        gsKit_sync_flip(gsGlobal);
        gsKit_queue_exec(gsGlobal);
    }

    return 0;
}
