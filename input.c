#include "minicraft.h"

static u8 padBuf[256] __attribute__((aligned(64)));

void input_init(void) {
    padInit(0);
    padPortOpen(0, 0, padBuf);
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
            // buttons.btns: bit=0 inseamna apasat, bit=1 inseamna liber
            // Cand nimic nu e apasat, btns = 0xFFFF
            u16 btns = buttons.btns;
            if (btns != 0xFFFF) {
                // Inverseaza doar bitii care sunt 0 (apasati)
                gs->padCurrent = (~btns) & 0xFFFF;
            } else {
                gs->padCurrent = 0;
            }
        }
    }
}

int input_pressed(GameState *gs, u32 button) {
    return (gs->padCurrent & button) && !(gs->padPrev & button);
}

int input_held(GameState *gs, u32 button) {
    return (gs->padCurrent & button) != 0;
}
