#include "minicraft.h"

static GameState gs;
static GameState quickSave;
static int quickSaveValid = 0;

void game_init(GameState *gs2) {
    memset(gs2, 0, sizeof(GameState));
    gs2->state = STATE_MENU;
    gs2->airWizardAlive = 1;

    unsigned int seed = 0xDEAD1234;
    for (int i = 0; i < NUM_LEVELS; i++) {
        world_generate(&gs2->levels[i], i, seed);
    }

    int spawnX = 5, spawnY = 5;
    for (int y = WORLD_H/4; y < WORLD_H*3/4; y++) {
        for (int x = WORLD_W/4; x < WORLD_W*3/4; x++) {
            if (world_getTile(&gs2->levels[0], x, y) == TILE_GRASS) {
                spawnX = x; spawnY = y;
                goto found;
            }
        }
    }
    found:
    player_init(&gs2->player, spawnX, spawnY);
    gs2->camX = gs2->player.x;
    gs2->camY = gs2->player.y;
    gs2->dayTime = 0;
    gs2->tickCount = 0;
}

void game_tick(GameState *gs2) {
    gs2->tickCount++;
    gs2->dayTime = (gs2->dayTime + 1) % CYCLE_LENGTH;

    // Debounce timer
    if (gs2->stateTimer > 0) gs2->stateTimer--;
    if (gs2->saveMessageTimer > 0) gs2->saveMessageTimer--;

    // Select = quick save. Start + Select = quick load.
    if (input_pressed(gs2, PAD_SELECT)) {
        if ((gs2->padCurrent & PAD_START) && quickSaveValid) {
            GameState loaded = quickSave;
            memcpy(gs2, &loaded, sizeof(GameState));
            gs2->saveMessageTimer = 120;
        } else {
            quickSave = *gs2;
            quickSave.hasQuickSave = 1;
            quickSaveValid = 1;
            gs2->hasQuickSave = 1;
            gs2->saveMessageTimer = 120;
        }
    }

    if (input_pressed(gs2, PAD_START)) {
        if (gs2->state == STATE_PLAYING) gs2->state = STATE_PAUSE;
        else if (gs2->state == STATE_PAUSE) gs2->state = STATE_PLAYING;
    }

    if (gs2->state == STATE_PLAYING) {
        player_tick(gs2);
        entity_tickAll(gs2);
        // Particulele raman oprite; mobii sunt inapoi.
        // particle_tickAll(gs2);

        // Camera smooth follow
        gs2->camX += (gs2->player.x - gs2->camX) / 4;
        gs2->camY += (gs2->player.y - gs2->camY) / 4;

        // Clamp camera
        int halfW = SCREEN_W / 2;
        int halfH = (SCREEN_H - 40) / 2;
        int worldPxW = WORLD_W * TILE_SIZE;
        int worldPxH = WORLD_H * TILE_SIZE;

        if (worldPxW <= SCREEN_W) {
            gs2->camX = worldPxW / 2;
        } else {
            if (gs2->camX < halfW) gs2->camX = halfW;
            if (gs2->camX > worldPxW - halfW) gs2->camX = worldPxW - halfW;
        }

        if (worldPxH <= (SCREEN_H - 40)) {
            gs2->camY = worldPxH / 2;
        } else {
            if (gs2->camY < halfH) gs2->camY = halfH;
            if (gs2->camY > worldPxH - halfH) gs2->camY = worldPxH - halfH;
        }
    }
}

int main(void) {
    SifInitRpc(0);

    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode            = GS_MODE_NTSC;
    gsGlobal->Width           = SCREEN_W;
    gsGlobal->Height          = SCREEN_H;
    gsGlobal->PSM             = GS_PSM_CT32;
    gsGlobal->PSMZ            = GS_PSMZ_16;
    gsGlobal->ZBuffering      = GS_SETTING_OFF;
    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->PrimAlphaEnable = GS_SETTING_OFF;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
                D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    input_init();
    rng_seed(0xCAFE1234);
    game_init(&gs);

    while(1) {
        input_update(&gs);
        game_tick(&gs);
        game_render(&gs, gsGlobal);
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);
    }

    return 0;
}
