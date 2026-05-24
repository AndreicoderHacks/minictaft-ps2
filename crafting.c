// crafting.c - Recipes and crafting logic
#include "minicraft.h"

typedef struct {
    const char *name;
    int result;
    int resultCount;
    int ing1, amt1;
    int ing2, amt2;
    int ing3, amt3;
} Recipe;

// Recipes: name, result item, count, up to 3 ingredients
static const Recipe recipes[] = {
    // Basic tools
    { "Wood Pickaxe",   ITEM_WOOD_TOOL,   1,  ITEM_WOOD, 5,  ITEM_NONE, 0, ITEM_NONE, 0 },
    { "Stone Pickaxe",  ITEM_STONE_TOOL,  1,  ITEM_STONE, 5, ITEM_WOOD, 2, ITEM_NONE, 0 },
    { "Iron Pickaxe",   ITEM_IRON_TOOL,   1,  ITEM_IRON, 5,  ITEM_WOOD, 2, ITEM_NONE, 0 },
    { "Gold Pickaxe",   ITEM_GOLD_TOOL,   1,  ITEM_GOLD, 5,  ITEM_WOOD, 2, ITEM_NONE, 0 },
    { "Gem Pickaxe",    ITEM_GEM_TOOL,    1,  ITEM_GEM, 5,   ITEM_WOOD, 2, ITEM_NONE, 0 },
    // Swords
    { "Wood Sword",     ITEM_WOOD_SWORD,  1,  ITEM_WOOD, 5,  ITEM_NONE, 0, ITEM_NONE, 0 },
    { "Stone Sword",    ITEM_STONE_SWORD, 1,  ITEM_STONE, 5, ITEM_WOOD, 2, ITEM_NONE, 0 },
    { "Iron Sword",     ITEM_IRON_SWORD,  1,  ITEM_IRON, 5,  ITEM_WOOD, 2, ITEM_NONE, 0 },
    { "Gold Sword",     ITEM_GOLD_SWORD,  1,  ITEM_GOLD, 5,  ITEM_WOOD, 2, ITEM_NONE, 0 },
    { "Gem Sword",      ITEM_GEM_SWORD,   1,  ITEM_GEM, 5,   ITEM_WOOD, 2, ITEM_NONE, 0 },
    // Structures
    { "Workbench",      ITEM_WORKBENCH,   1,  ITEM_WOOD, 20, ITEM_NONE, 0, ITEM_NONE, 0 },
    { "Furnace",        ITEM_FURNACE,     1,  ITEM_STONE, 20,ITEM_NONE, 0, ITEM_NONE, 0 },
    { "Lantern",        ITEM_LANTERN,     1,  ITEM_IRON, 5,  ITEM_COAL, 2, ITEM_NONE, 0 },
    // Food
    { "Bread",          ITEM_BREAD,       1,  ITEM_WHEAT, 3, ITEM_NONE, 0, ITEM_NONE, 0 },
};

#define RECIPE_COUNT (sizeof(recipes) / sizeof(recipes[0]))

int crafting_getRecipeCount(void) { return (int)RECIPE_COUNT; }

const char* crafting_getName(int recipe) {
    if (recipe < 0 || recipe >= (int)RECIPE_COUNT) return "???";
    return recipes[recipe].name;
}

void crafting_getResult(int recipe, int *item, int *count) {
    if (recipe < 0 || recipe >= (int)RECIPE_COUNT) { *item = 0; *count = 0; return; }
    *item  = recipes[recipe].result;
    *count = recipes[recipe].resultCount;
}

int crafting_canCraft(GameState *gs, int recipe) {
    if (recipe < 0 || recipe >= (int)RECIPE_COUNT) return 0;
    const Recipe *r = &recipes[recipe];
    if (r->ing1 != ITEM_NONE && !player_hasItem(gs, r->ing1, r->amt1)) return 0;
    if (r->ing2 != ITEM_NONE && !player_hasItem(gs, r->ing2, r->amt2)) return 0;
    if (r->ing3 != ITEM_NONE && !player_hasItem(gs, r->ing3, r->amt3)) return 0;
    return 1;
}

void crafting_doCraft(GameState *gs, int recipe) {
    if (!crafting_canCraft(gs, recipe)) return;
    const Recipe *r = &recipes[recipe];
    if (r->ing1 != ITEM_NONE) player_removeItem(gs, r->ing1, r->amt1);
    if (r->ing2 != ITEM_NONE) player_removeItem(gs, r->ing2, r->amt2);
    if (r->ing3 != ITEM_NONE) player_removeItem(gs, r->ing3, r->amt3);
    player_addItem(gs, r->result, r->resultCount);
}
