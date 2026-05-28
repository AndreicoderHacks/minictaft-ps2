// entity.c - Entity spawning, AI, combat
#include "minicraft.h"

Entity* entity_spawn(GameState *gs, int type, int x, int y, int level) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!gs->entities[i].alive) {
            Entity *e = &gs->entities[i];
            memset(e, 0, sizeof(Entity));
            e->type   = type;
            e->x      = x * TILE_SIZE + TILE_SIZE/2;
            e->y      = y * TILE_SIZE + TILE_SIZE/2;
            e->alive  = 1;
            e->level  = level;
            e->dir    = DIR_DOWN;
            switch(type) {
                case ENT_ZOMBIE:
                    e->health = e->maxHealth = 10;
                    break;
                case ENT_SLIME:
                    e->health = e->maxHealth = 6;
                    break;
                case ENT_AIR_WIZARD:
                    e->health = e->maxHealth = 2000;
                    break;
                default:
                    e->health = e->maxHealth = 5;
                    break;
            }
            if (i + 1 > gs->entityCount) gs->entityCount = i + 1;
            return e;
        }
    }
    return NULL;
}

void entity_remove(GameState *gs, int idx) {
    gs->entities[idx].alive = 0;
}

void entity_hurt(GameState *gs, int idx, int damage) {
    Entity *e = &gs->entities[idx];
    if (!e->alive || e->hurtTime > 0) return;
    e->health -= damage;
    e->hurtTime = 20;
    // Particulele raman oprite in build-ul stabil.
    if (e->health <= 0) {
        // Drop loot
        int dropX = e->x / TILE_SIZE;
        int dropY = e->y / TILE_SIZE;
        switch(e->type) {
            case ENT_ZOMBIE:
                // Random drops
                if (rng_range(0,3) == 0) player_addItem(gs, ITEM_BREAD, 1);
                player_addItem(gs, ITEM_COAL, rng_range(1,3));
                break;
            case ENT_SLIME:
                // Slime drop nothing for now
                break;
            case ENT_AIR_WIZARD:
                gs->airWizardAlive = 0;
                gs->state = STATE_WIN;
                break;
        }
        gs->monstersKilled++;
        entity_remove(gs, idx);
    }
}

// ---- ZOMBIE AI ----
void zombie_ai(GameState *gs, int idx) {
    Entity *e = &gs->entities[idx];
    Entity *p = &gs->player;
    Level  *lv = &gs->levels[gs->currentLevel];

    if (e->level != gs->currentLevel) return;
    if (e->hurtTime > 0) { e->hurtTime--; }
    e->anim++;

    // Simple chase: move toward player
    e->ai_timer++;
    if (e->ai_timer < 3) return;
    e->ai_timer = 0;

    int dx = p->x - e->x;
    int dy = p->y - e->y;
    int dist2 = dx*dx + dy*dy;

    // Only chase if within 10 tiles
    if (dist2 > (10 * TILE_SIZE) * (10 * TILE_SIZE)) {
        // Random wander
        if ((rng_next() & 0xF) == 0) {
            int dir = rng_next() & 3;
            switch(dir) {
                case 0: e->vx = 0;  e->vy = -1; e->dir = DIR_UP;    break;
                case 1: e->vx = 0;  e->vy =  1; e->dir = DIR_DOWN;  break;
                case 2: e->vx = -1; e->vy = 0;  e->dir = DIR_LEFT;  break;
                case 3: e->vx =  1; e->vy = 0;  e->dir = DIR_RIGHT; break;
            }
        }
    } else {
        // Chase
        if      (dx > 0) { e->vx =  1; e->dir = DIR_RIGHT; }
        else if (dx < 0) { e->vx = -1; e->dir = DIR_LEFT;  }
        else              e->vx = 0;
        if      (dy > 0) { e->vy =  1; e->dir = DIR_DOWN;  }
        else if (dy < 0) { e->vy = -1; e->dir = DIR_UP;    }
        else              e->vy = 0;
    }

    // Move with simple collision
    int nx = e->x + e->vx;
    int ny = e->y + e->vy;
    int tx1 = (nx - 6) / TILE_SIZE, tx2 = (nx + 6) / TILE_SIZE;
    int ty1 = (ny - 6) / TILE_SIZE, ty2 = (ny + 6) / TILE_SIZE;
    if (!world_isSolid(world_getTile(lv, tx1, e->y/TILE_SIZE)) &&
        !world_isSolid(world_getTile(lv, tx2, e->y/TILE_SIZE)))
        e->x = nx;
    if (!world_isSolid(world_getTile(lv, e->x/TILE_SIZE, ty1)) &&
        !world_isSolid(world_getTile(lv, e->x/TILE_SIZE, ty2)))
        e->y = ny;

    // Attack player if adjacent
    if (dist2 < (TILE_SIZE * 2) * (TILE_SIZE * 2)) {
        if ((gs->tickCount & 31) == 0) {
            player_hurt(gs, 1);
        }
    }
}

// ---- SLIME AI ----
void slime_ai(GameState *gs, int idx) {
    Entity *e = &gs->entities[idx];
    Entity *p = &gs->player;
    Level  *lv = &gs->levels[gs->currentLevel];

    if (e->level != gs->currentLevel) return;
    if (e->hurtTime > 0) { e->hurtTime--; }
    e->anim++;
    e->ai_timer++;

    // Hop every 20 ticks
    if (e->ai_timer >= 20) {
        e->ai_timer = 0;
        int dx = p->x - e->x;
        int dy = p->y - e->y;
        int dist2 = dx*dx + dy*dy;
        if (dist2 < (8 * TILE_SIZE) * (8 * TILE_SIZE)) {
            // Jump toward player
            e->vx = (dx > 0) ? 3 : -3;
            e->vy = (dy > 0) ? 3 : -3;
        } else {
            e->vx = rng_range(-2, 2);
            e->vy = rng_range(-2, 2);
        }
    }

    // Apply velocity with friction
    int nx = e->x + e->vx;
    int ny = e->y + e->vy;
    int tx1 = (nx - 6) / TILE_SIZE, tx2 = (nx + 6) / TILE_SIZE;
    int ty1 = (ny - 6) / TILE_SIZE, ty2 = (ny + 6) / TILE_SIZE;
    if (!world_isSolid(world_getTile(lv, tx1, e->y/TILE_SIZE)) &&
        !world_isSolid(world_getTile(lv, tx2, e->y/TILE_SIZE)))
        e->x = nx;
    else
        e->vx = -e->vx / 2;

    if (!world_isSolid(world_getTile(lv, e->x/TILE_SIZE, ty1)) &&
        !world_isSolid(world_getTile(lv, e->x/TILE_SIZE, ty2)))
        e->y = ny;
    else
        e->vy = -e->vy / 2;

    // Friction
    e->vx = e->vx * 3 / 4;
    e->vy = e->vy * 3 / 4;

    // Damage player on contact
    int ddx = p->x - e->x, ddy = p->y - e->y;
    if (ddx*ddx + ddy*ddy < (TILE_SIZE + 4) * (TILE_SIZE + 4)) {
        if ((gs->tickCount & 31) == 0) player_hurt(gs, 1);
    }
}

// ---- AIR WIZARD AI ----
void airwizard_ai(GameState *gs, int idx) {
    Entity *e = &gs->entities[idx];
    Entity *p = &gs->player;

    if (e->level != gs->currentLevel) return;
    if (e->hurtTime > 0) { e->hurtTime--; }
    e->anim++;
    e->ai_timer++;

    // Phase 1: charge at player
    int dx = p->x - e->x;
    int dy = p->y - e->y;
    int dist2 = dx*dx + dy*dy;

    // Faster when lower health
    int speed = 2 + (e->maxHealth - e->health) / 200;

    // Normalize and move
    if (dist2 > 100) {
        // integer sqrt approximation
        int dist = 1;
        while (dist * dist < dist2) dist++;
        e->x += (dx * speed) / dist;
        e->y += (dy * speed) / dist;
        e->dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;
    }

    // Phase 2: spawn spark (as new zombie, visually different)
    if (e->ai_timer > 60 && e->spellCount < 5 + (e->maxHealth - e->health)/300) {
        e->ai_timer = 0;
        // Spawn 2-4 sparks (represented as slimes on sky)
        int sparks = 2 + rng_range(0, 2);
        for (int s = 0; s < sparks; s++) {
            Entity *spark = entity_spawn(gs, ENT_SLIME,
                e->x / TILE_SIZE + rng_range(-3,3),
                e->y / TILE_SIZE + rng_range(-3,3),
                gs->currentLevel);
            if (spark) {
                spark->health = spark->maxHealth = 3;
            }
        }
        e->spellCount++;
    }

    // Reset spell count periodically
    if (e->ai_timer % 120 == 0) e->spellCount = 0;

    // Contact damage
    if (dist2 < (TILE_SIZE + 8) * (TILE_SIZE + 8)) {
        if ((gs->tickCount & 15) == 0) player_hurt(gs, 3);
    }

    // Clamp to world
    if (e->x < TILE_SIZE)                  e->x = TILE_SIZE;
    if (e->y < TILE_SIZE)                  e->y = TILE_SIZE;
    if (e->x > (WORLD_W-1)*TILE_SIZE)     e->x = (WORLD_W-1)*TILE_SIZE;
    if (e->y > (WORLD_H-1)*TILE_SIZE)     e->y = (WORLD_H-1)*TILE_SIZE;
}

// ---- SPAWN RANDOM MOBS ----
static void try_spawn_mob(GameState *gs) {
    if (gs->currentLevel == LEVEL_SURFACE && gs->dayTime < DAY_LENGTH) return;

    Level *lv = &gs->levels[gs->currentLevel];
    int px = gs->player.x / TILE_SIZE;
    int py = gs->player.y / TILE_SIZE;

    // Count existing on this level
    int count = 0;
    for (int i = 0; i < gs->entityCount; i++) {
        if (gs->entities[i].alive && gs->entities[i].level == gs->currentLevel)
            count++;
    }
    int maxMobs = 4 + gs->currentLevel * 2;
    if (count >= maxMobs) return;

    // Spawn far from player
    int tx, ty;
    for (int attempt = 0; attempt < 10; attempt++) {
        tx = rng_range(0, WORLD_W);
        ty = rng_range(0, WORLD_H);
        int ddx = tx - px, ddy = ty - py;
        if (ddx*ddx + ddy*ddy < 10*10) continue;
        int tile = world_getTile(lv, tx, ty);
        if (tile == TILE_GRASS || tile == TILE_DIRT ||
            tile == TILE_STONE || tile == TILE_CLOUD) {
            // Night = more zombies
            int r = rng_range(0, 100);
            int type;
            if (gs->currentLevel == 0) {
                type = (r < 75) ? ENT_ZOMBIE : ENT_SLIME;
            } else {
                type = (r < 70) ? ENT_ZOMBIE : ENT_SLIME;
            }
            entity_spawn(gs, type, tx, ty, gs->currentLevel);
            return;
        }
    }
}

// ---- TICK ALL ----
void entity_tickAll(GameState *gs) {
    // Pe suprafata mobii exista doar noaptea. Ziua ii curatam pentru FPS si gameplay mai fair.
    if (gs->currentLevel == LEVEL_SURFACE && gs->dayTime < DAY_LENGTH) {
        for (int i = 0; i < gs->entityCount; i++) {
            if (gs->entities[i].alive && gs->entities[i].level == LEVEL_SURFACE)
                gs->entities[i].alive = 0;
        }
    }

    // Spawn mobs occasionally
    if ((gs->tickCount & 255) == 0) try_spawn_mob(gs);

    // Spawn boss if not alive and player is on sky
    if (gs->currentLevel == LEVEL_SKY && gs->airWizardAlive) {
        int bossPresent = 0;
        for (int i = 0; i < gs->entityCount; i++) {
            if (gs->entities[i].alive && gs->entities[i].type == ENT_AIR_WIZARD)
                bossPresent = 1;
        }
        if (!bossPresent) {
            entity_spawn(gs, ENT_AIR_WIZARD, WORLD_W/2, WORLD_H/2, LEVEL_SKY);
        }
    }

    for (int i = 0; i < gs->entityCount; i++) {
        Entity *e = &gs->entities[i];
        if (!e->alive) continue;
        switch(e->type) {
            case ENT_ZOMBIE:     zombie_ai(gs, i);     break;
            case ENT_SLIME:      slime_ai(gs, i);      break;
            case ENT_AIR_WIZARD: airwizard_ai(gs, i);  break;
        }
    }
}
