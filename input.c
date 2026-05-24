// input.c - PS2 controller input via libpad
#include "minicraft.h"

static u8 padBuf[2][256] __attribute__((aligned(64)));
static struct padButtonStatus buttons;

void input_init(void) {
    padInit(0);
    padPortOpen(0, 0, padBuf[0]);
    padPortOpen(1, 0, padBuf[1]);
    // Nu astepta - lasa game loop sa se ocupe
}

void input_update(GameState *gs) {
    gs->padPrev = gs->padCurrent;

    int state = padGetState(0, 0);
    if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
        if (padRead(0, 0, &buttons) != 0) {
            gs->padCurrent = ~buttons.btns;
        }
    }
    // Daca pad nu e gata, pastreaza butoanele vechi
    // (nu reseta la 0 ca sa nu blocheze meniul)
}

int input_pressed(GameState *gs, u32 button) {
    return (gs->padCurrent & button) && !(gs->padPrev & button);
}

int input_held(GameState *gs, u32 button) {
    return (gs->padCurrent & button) != 0;
}
