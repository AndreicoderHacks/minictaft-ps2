// main.c - Entry point, game loop, initialization
#include "minicraft.h"

// ---- INIT ----
void game_init(GameState *gs) {
    memset(gs, 0, sizeof(GameState));
    gs->state = STATE_MENU;
    gs->airWizardAlive = 1;

    // Generate all levels
    unsigned int seed = (unsigned int)clock();
    if (seed == 0) seed = 0xDEAD1234;
    for (int i = 0; i < NUM_LEVELS; i++) {
        world_generate(&gs->levels[i], i, seed);
    }

    // Find a grass tile to spawn player
    int spawnX = WORLD_W/2, spawnY = WORLD_H/2;
    for (int y = WORLD_H/2; y < WORLD_H; y++) {
        for (int x = WORLD_W/2; x < WORLD_W; x++) {
            if (world_getTile(&gs->levels[0], x, y) == TILE_GRASS) {
                spawnX = x;
                spawnY = y;
                goto found_spawn;
            }
        }
    }
    found_spawn:
    player_init(&gs->player, spawnX, spawnY);

    // Starting inventory: just hands
    // (nothing - player must gather everything)

    gs->dayTime = 0;
    gs->tickCount = 0;
}

// ---- TICK ----
void game_tick(GameState *gs) {
    gs->tickCount++;
    gs->padPrev = gs->padCurrent; // handled in input_update but safe here too

    // Day/night cycle: 4800 ticks per full day (~80 seconds at 60fps)
    gs->dayTime = (gs->dayTime + 1) % 4800;

    // Pause
    if (input_pressed(gs, PAD_START)) {
        if (gs->state == STATE_PLAYING) gs->state = STATE_PAUSE;
        else if (gs->state == STATE_PAUSE) gs->state = STATE_PLAYING;
    }

    if (gs->state == STATE_PLAYING) {
        player_tick(gs);
        entity_tickAll(gs);
        particle_tickAll(gs);

        // Camera follow player smoothly
        gs->camX += (gs->player.x - gs->camX) / 4;
        gs->camY += (gs->player.y - gs->camY) / 4;

        // Clamp camera
        int halfW = SCREEN_W / 2;
        int halfH = (SCREEN_H - 40) / 2;
        if (gs->camX < halfW) gs->camX = halfW;
        if (gs->camY < halfH) gs->camY = halfH;
        if (gs->camX > WORLD_W * TILE_SIZE - halfW) gs->camX = WORLD_W * TILE_SIZE - halfW;
        if (gs->camY > WORLD_H * TILE_SIZE - halfH) gs->camY = WORLD_H * TILE_SIZE - halfH;
    } else if (gs->state == STATE_INVENTORY || gs->state == STATE_CRAFTING) {
        // Inventory/crafting handles its own input in render
    }

    // Pause screen
    if (gs->state == STATE_PAUSE) {
        // Draw handled in render, just wait for resume
    }
}

// ---- MAIN ----
int main(void) {
    // Init PS2 subsystems
    SifInitRpc(0);

    // Init gsKit
    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode     = GS_MODE_NTSC;
    gsGlobal->Width    = SCREEN_W;
    gsGlobal->Height   = SCREEN_H;
    gsGlobal->PSM      = GS_PSM_CT32;
    gsGlobal->PSMZ     = GS_PSMZ_16;
    gsGlobal->ZBuffering = GS_SETTING_ON;
    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
                D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

    // Enable alpha blending
    gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);

    // Init input
    input_init();

    // Init game
    GameState gs;
    game_init(&gs);

    // Seed RNG with some value
    rng_seed(0xCAFE1234);

    // ---- GAME LOOP ----
    while(1) {
        input_update(&gs);
        game_tick(&gs);
        game_render(&gs, gsGlobal);
        gsKit_sync_flip(gsGlobal);
        gsKit_queue_exec(gsGlobal);
    }

    return 0;
}
