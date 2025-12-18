#include "game_editor.h"
#include "game/core/game_core.h"
#include "game/game_config.h"

GameEditor g_gameEditor = { 0 };

Rabbit* GameEditor_buildUsableArray(Rabbit* rabbits)
{
    Rabbit* l_New = calloc(MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS, sizeof(Rabbit));
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

    for (int x = RABBIT_COUNT + MUSHROOM_COUNT + FOX_COUNT; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
    {
        l_New[x].Type = NO_OBJECT;
        l_New[x].CellX = -1;
        l_New[x].CellY = -1;
        l_New[x].Direction = RABBIT_NORTH;
        l_New[x].RabbitSprite = NULL;
    }

    return l_New;
}
