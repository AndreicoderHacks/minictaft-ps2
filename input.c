#include "minicraft.h"

static u8 padBuf[256] __attribute__((aligned(64)));
static struct padButtonStatus buttons;

void input_init(void) {
    padInit(0);
    padPortOpen(0, 0, padBuf);
}

void input_update(GameState *gs) {
    gs->padPrev = gs->padCurrent;
    gs->padCurrent = 0;

    int state = padGetState(0, 0);
    if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
        padRead(0, 0, &buttons);
        gs->padCurrent = ~((u32)buttons.btns);
        gs->padCurrent = ~raw;
    }
}

int input_pressed(GameState *gs, u32 button) {
    return (gs->padCurrent & button) && !(gs->padPrev & button);
}

int input_held(GameState *gs, u32 button) {
    return (gs->padCurrent & button) != 0;
}
