// particles.c - Simple particle effects
#include "minicraft.h"

void particle_spawn(GameState *gs, int x, int y, int vx, int vy, u32 color, int life) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (gs->particles[i].life <= 0) {
            gs->particles[i].x    = x;
            gs->particles[i].y    = y;
            gs->particles[i].vx   = vx;
            gs->particles[i].vy   = vy;
            gs->particles[i].color = color;
            gs->particles[i].life  = life;
            return;
        }
    }
}

void particle_tickAll(GameState *gs) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &gs->particles[i];
        if (p->life <= 0) continue;
        p->x += p->vx;
        p->y += p->vy;
        p->vy++;      // gravity
        p->life--;
    }
}
