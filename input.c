#include "minicraft.h"

static u8 padBuf[256] __attribute__((aligned(64)));

void input_init(void) {
    padInit(0);
    padPortOpen(0, 0, padBuf);
    // Asteapta 120 frame-uri ca pad-ul sa se initializeze
    for (int i = 0; i < 120; i++) {
        padGetState(0, 0);
    }
}

void input_update(GameState *gs) {
    gs->padPrev = gs->padCurrent;
    gs->padCurrent = 0;

    struct padButtonStatus buttons;
    int state = padGetState(0, 0);
    if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
        if (padRead(0, 0, &buttons) != 0) {
            gs->padCurrent = ~((u32)buttons.btns) & 0xFFFF;
        }
    }
}

int input_pressed(GameState *gs, u32 button) {
    return (gs->padCurrent & button) && !(gs->padPrev & button);
}
