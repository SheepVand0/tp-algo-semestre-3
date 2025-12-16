#include "game_editor.h"
#include "game/core/game_core.h"
#include "game/game_config.h"

GameEditor g_gameEditor = { 0 };

Rabbit* GameEditor_buildUsableArray(Rabbit* rabbits)
{
    Rabbit* l_New = calloc(RABBIT_COUNT + FOX_COUNT + MUSHROOM_COUNT, sizeof(Rabbit));
    assert(l_New);

    for (int x = 0; x < RABBIT_COUNT; x++)
    {
        l_New[x] = rabbits[x];
    }

    for (int x = 0; x < FOX_COUNT; x++)
    {
        l_New[x + RABBIT_COUNT] = rabbits[MAX_RABBITS + x];
    }

    for (int x = 0; x < MUSHROOM_COUNT; x++)
    {
        l_New[x + RABBIT_COUNT + FOX_COUNT] = rabbits[MAX_RABBITS + MAX_FOXES + x];
    }

    return l_New;
}
