// player.c - Player movement, combat, inventory
#include "minicraft.h"

void player_init(Entity *p, int x, int y) {
    memset(p, 0, sizeof(Entity));
    p->x = x * TILE_SIZE + TILE_SIZE / 2;
    p->y = y * TILE_SIZE + TILE_SIZE / 2;
    p->health    = 10;
    p->maxHealth = 10;
    p->type      = ENT_PLAYER;
    p->dir       = DIR_DOWN;
    p->alive     = 1;
    p->level     = LEVEL_SURFACE;
}

void player_addItem(GameState *gs, int item, int count) {
    // Try to stack
    for (int i = 0; i < INV_SIZE; i++) {
        if (gs->inventory[i].type == item) {
            gs->inventory[i].count += count;
            return;
        }
    }
    // Find empty slot
    for (int i = 0; i < INV_SIZE; i++) {
        if (gs->inventory[i].type == ITEM_NONE) {
            gs->inventory[i].type  = item;
            gs->inventory[i].count = count;
            return;
        }
    }
}

int player_hasItem(GameState *gs, int item, int count) {
    for (int i = 0; i < INV_SIZE; i++) {
        if (gs->inventory[i].type == item && gs->inventory[i].count >= count)
            return 1;
    }
    return 0;
}

void player_removeItem(GameState *gs, int item, int count) {
    for (int i = 0; i < INV_SIZE; i++) {
        if (gs->inventory[i].type == item) {
            gs->inventory[i].count -= count;
            if (gs->inventory[i].count <= 0) {
                gs->inventory[i].type  = ITEM_NONE;
                gs->inventory[i].count = 0;
            }
            return;
        }
    }
}

// Returns attack damage based on equipped item
static int player_attackDamage(GameState *gs) {
    int item = gs->inventory[gs->selectedSlot].type;
    switch(item) {
        case ITEM_WOOD_SWORD:   return 2;
        case ITEM_STONE_SWORD:  return 3;
        case ITEM_IRON_SWORD:   return 4;
        case ITEM_GOLD_SWORD:   return 5;
        case ITEM_GEM_SWORD:    return 8;
        default:                return 1;
    }
}

// Returns mining power based on equipped item
static int player_miningPower(GameState *gs) {
    int item = gs->inventory[gs->selectedSlot].type;
    switch(item) {
        case ITEM_WOOD_TOOL:    return 1;
        case ITEM_STONE_TOOL:   return 2;
        case ITEM_IRON_TOOL:    return 3;
        case ITEM_GOLD_TOOL:    return 4;
        case ITEM_GEM_TOOL:     return 5;
        default:                return 0;
    }
}

// Attack a tile in facing direction
static void player_mineTile(GameState *gs, int tx, int ty) {
    Level *lv  = &gs->levels[gs->currentLevel];
    int tile   = world_getTile(lv, tx, ty);
    int data   = world_getData(lv, tx, ty);
    int power  = player_miningPower(gs);

    switch(tile) {
        case TILE_TREE:
            data -= (power + 1);
            if (data <= 0) {
                world_setTile(lv, tx, ty, TILE_GRASS, 0);
                player_addItem(gs, ITEM_WOOD, 3 + rng_range(0, 3));
                // Chance to drop apple
                if (rng_range(0, 5) == 0) player_addItem(gs, ITEM_APPLE, 1);
            } else {
                world_setTile(lv, tx, ty, TILE_TREE, data);
                particle_spawn(gs, tx*TILE_SIZE+8, ty*TILE_SIZE+8, rng_range(-2,2), -2, COL_BROWN, 20);
            }
            break;
        case TILE_ROCK_ORE:
            if (power >= 1) {
                data -= power;
                if (data <= 0) {
                    world_setTile(lv, tx, ty, TILE_DIRT, 0);
                    player_addItem(gs, ITEM_STONE, 2 + rng_range(0, 2));
                } else {
                    world_setTile(lv, tx, ty, TILE_ROCK_ORE, data);
                    particle_spawn(gs, tx*TILE_SIZE+8, ty*TILE_SIZE+8, rng_range(-2,2), -2, COL_GRAY, 20);
                }
            }
            break;
        case TILE_IRON_ORE:
            if (power >= 2) {
                data -= power - 1;
                if (data <= 0) {
                    world_setTile(lv, tx, ty, TILE_DIRT, 0);
                    player_addItem(gs, ITEM_IRON, 2 + rng_range(0, 2));
                } else {
                    world_setTile(lv, tx, ty, TILE_IRON_ORE, data);
                }
            }
            break;
        case TILE_GOLD_ORE:
            if (power >= 3) {
                data -= power - 2;
                if (data <= 0) {
                    world_setTile(lv, tx, ty, TILE_DIRT, 0);
                    player_addItem(gs, ITEM_GOLD, 2 + rng_range(0, 2));
                } else {
                    world_setTile(lv, tx, ty, TILE_GOLD_ORE, data);
                }
            }
            break;
        case TILE_GEM_ORE:
            if (power >= 4) {
                data -= power - 3;
                if (data <= 0) {
                    world_setTile(lv, tx, ty, TILE_DIRT, 0);
                    player_addItem(gs, ITEM_GEM, 2 + rng_range(0, 2));
                } else {
                    world_setTile(lv, tx, ty, TILE_GEM_ORE, data);
                }
            }
            break;
        case TILE_WORKBENCH:
            // Opens crafting menu
            gs->state = STATE_CRAFTING;
            break;
        case TILE_FURNACE:
            // Smelt if we have coal+ore
            if (player_hasItem(gs, ITEM_COAL, 1) && player_hasItem(gs, ITEM_IRON, 1)) {
                player_removeItem(gs, ITEM_COAL, 1);
                player_removeItem(gs, ITEM_IRON, 1);
                player_addItem(gs, ITEM_IRON, 1);
            }
            break;
        case TILE_WHEAT:
            if (data >= 5) {
                world_setTile(lv, tx, ty, TILE_FARMLAND, 0);
                player_addItem(gs, ITEM_WHEAT, 2 + rng_range(0, 2));
                player_addItem(gs, ITEM_SEEDS, 1 + rng_range(0, 1));
            }
            break;
        case TILE_FARMLAND:
            // Can plant seeds
            if (player_hasItem(gs, ITEM_SEEDS, 1)) {
                player_removeItem(gs, ITEM_SEEDS, 1);
                world_setTile(lv, tx, ty, TILE_WHEAT, 0);
            }
            break;
        case TILE_STAIRS_DOWN:
            if (gs->currentLevel < NUM_LEVELS - 1) {
                gs->currentLevel++;
                gs->player.level = gs->currentLevel;
                // Spawn player near stairs up on new level
                for (int y = 0; y < WORLD_H && !gs->player.alive; y++) {
                    for (int x = 0; x < WORLD_W; x++) {
                        if (world_getTile(&gs->levels[gs->currentLevel], x, y) == TILE_STAIRS_UP) {
                            gs->player.x = x * TILE_SIZE + 8;
                            gs->player.y = y * TILE_SIZE + 8;
                        }
                    }
                }
                gs->player.alive = 1;
            }
            break;
        case TILE_STAIRS_UP:
            if (gs->currentLevel > 0) {
                gs->currentLevel--;
                gs->player.level = gs->currentLevel;
                gs->player.alive = 1;
            }
            break;
        default:
            break;
    }
}

void player_attack(GameState *gs) {
    Entity *p = &gs->player;
    if (p->attackCooldown > 0) return;
    p->attackCooldown = 10;

    // Facing tile
    int tx = p->x / TILE_SIZE;
    int ty = p->y / TILE_SIZE;
    switch(p->dir) {
        case DIR_UP:    ty--; break;
        case DIR_DOWN:  ty++; break;
        case DIR_LEFT:  tx--; break;
        case DIR_RIGHT: tx++; break;
    }

    // Hit tiles
    player_mineTile(gs, tx, ty);

    // Hit entities
    int ax = tx * TILE_SIZE + TILE_SIZE/2;
    int ay = ty * TILE_SIZE + TILE_SIZE/2;
    int dmg = player_attackDamage(gs);
    for (int i = 0; i < gs->entityCount; i++) {
        Entity *e = &gs->entities[i];
        if (!e->alive || e->level != gs->currentLevel) continue;
        int dx = e->x - ax;
        int dy = e->y - ay;
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;
        if (dx < TILE_SIZE && dy < TILE_SIZE) {
            entity_hurt(gs, i, dmg);
            // Knockback
            int kbx = (e->x > p->x) ? 64 : -64;
            int kby = (e->y > p->y) ? 64 : -64;
            e->vx += kbx;
            e->vy += kby;
        }
    }
}

void player_hurt(GameState *gs, int damage) {
    Entity *p = &gs->player;
    if (p->hurtTime > 0) return;
    p->health -= damage;
    p->hurtTime = 30;
    particle_spawn(gs, p->x, p->y, rng_range(-3,3), -3, COL_RED, 25);
    if (p->health <= 0) {
        p->health = 0;
        gs->state = STATE_DEAD;
    }
}

void player_tick(GameState *gs) {
    Entity *p = &gs->player;
    Level  *lv = &gs->levels[gs->currentLevel];

    if (p->attackCooldown > 0) p->attackCooldown--;
    if (p->hurtTime > 0)       p->hurtTime--;
    p->anim++;

   // --- Movement ---
    int speed = 2;
    int mx = 0, my = 0;

    u32 pad = gs->padCurrent;
    if (pad & PAD_UP)    { my = -speed; p->dir = DIR_UP;    }
    if (pad & PAD_DOWN)  { my =  speed; p->dir = DIR_DOWN;  }
    if (pad & PAD_LEFT)  { mx = -speed; p->dir = DIR_LEFT;  }
    if (pad & PAD_RIGHT) { mx =  speed; p->dir = DIR_RIGHT; }

    // Miscare fara coliziuni
    p->x += mx;
    p->y += my;

    // Lava damage
    int ptx = p->x / TILE_SIZE;
    int pty = p->y / TILE_SIZE;
    if (world_getTile(lv, ptx, pty) == TILE_LAVA) {
        if ((gs->tickCount & 15) == 0) player_hurt(gs, 2);
    }

    // Grow wheat
    if ((gs->tickCount & 63) == 0) {
        for (int wy = 0; wy < WORLD_H; wy++) {
            for (int wx2 = 0; wx2 < WORLD_W; wx2++) {
                if (world_getTile(lv, wx2, wy) == TILE_WHEAT) {
                    int d = world_getData(lv, wx2, wy);
                    if (d < 5) world_setTile(lv, wx2, wy, TILE_WHEAT, d + 1);
                }
            }
        }
    }

    // Inventory cycling
    if (input_pressed(gs, PAD_R1)) gs->selectedSlot = (gs->selectedSlot + 1) % INV_SIZE;
    if (input_pressed(gs, PAD_L1)) gs->selectedSlot = (gs->selectedSlot + INV_SIZE - 1) % INV_SIZE;

    // Attack / interact
    if (input_pressed(gs, PAD_CROSS)) player_attack(gs);

    // Use item (eat food, place items)
    if (input_pressed(gs, PAD_SQUARE)) {
        int item = gs->inventory[gs->selectedSlot].type;
        if (item == ITEM_APPLE && p->health < p->maxHealth) {
            player_removeItem(gs, ITEM_APPLE, 1);
            p->health += 2;
            if (p->health > p->maxHealth) p->health = p->maxHealth;
        } else if (item == ITEM_BREAD && p->health < p->maxHealth) {
            player_removeItem(gs, ITEM_BREAD, 1);
            p->health += 4;
            if (p->health > p->maxHealth) p->health = p->maxHealth;
        } else if (item == ITEM_WORKBENCH) {
            // Place workbench
            int fx = ptx, fy = pty;
            switch(p->dir) {
                case DIR_UP:    fy--; break;
                case DIR_DOWN:  fy++; break;
                case DIR_LEFT:  fx--; break;
                case DIR_RIGHT: fx++; break;
            }
            if (!world_isSolid(world_getTile(lv, fx, fy))) {
                world_setTile(lv, fx, fy, TILE_WORKBENCH, 0);
                player_removeItem(gs, ITEM_WORKBENCH, 1);
            }
        } else if (item == ITEM_FURNACE) {
            int fx = ptx, fy = pty;
            switch(p->dir) {
                case DIR_UP:    fy--; break;
                case DIR_DOWN:  fy++; break;
                case DIR_LEFT:  fx--; break;
                case DIR_RIGHT: fx++; break;
            }
            if (!world_isSolid(world_getTile(lv, fx, fy))) {
                world_setTile(lv, fx, fy, TILE_FURNACE, 0);
                player_removeItem(gs, ITEM_FURNACE, 1);
            }
        }
    }

    // Open inventory
    if (input_pressed(gs, PAD_TRIANGLE)) {
        gs->state = STATE_INVENTORY;
    }

    // Clamp to world
    if (p->x < TILE_SIZE)                      p->x = TILE_SIZE;
    if (p->y < TILE_SIZE)                      p->y = TILE_SIZE;
    if (p->x > (WORLD_W - 1) * TILE_SIZE)     p->x = (WORLD_W - 1) * TILE_SIZE;
    if (p->y > (WORLD_H - 1) * TILE_SIZE)     p->y = (WORLD_H - 1) * TILE_SIZE;
}
