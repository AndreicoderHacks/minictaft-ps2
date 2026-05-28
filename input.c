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
    gs->analogX = 0;
    gs->analogY = 0;

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

            // Analog stanga: valori in jur de 128 sunt centru.
            // Adaugam aceleasi bituri ca D-pad-ul, ca restul codului sa ramana simplu.
            int lx = (int)buttons.ljoy_h - 128;
            int ly = (int)buttons.ljoy_v - 128;
            if (lx < -40) { gs->padCurrent |= PAD_LEFT;  gs->analogX = -1; }
            if (lx >  40) { gs->padCurrent |= PAD_RIGHT; gs->analogX =  1; }
            if (ly < -40) { gs->padCurrent |= PAD_UP;    gs->analogY = -1; }
            if (ly >  40) { gs->padCurrent |= PAD_DOWN;  gs->analogY =  1; }
        }
    }
}

int input_pressed(GameState *gs, u32 button) {
    return (gs->padCurrent & button) && !(gs->padPrev & button);
}

int input_held(GameState *gs, u32 button) {
    return (gs->padCurrent & button) != 0;
}
