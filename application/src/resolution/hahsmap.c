#include "hashmap.h"

typedef unsigned int uint32;

// Fonction de hachage des clés de la table de hachage
static uint32 hash(GameHashmapEntry grid, uint32 capacity)
{
    return 1;
}

GameHashmap* HashMap_New(size_t capacity)
{
    GameHashmap* map = (GameHashmap*)calloc(1, sizeof(GameHashmap));
    AssertNew(map);

    map->m_size = 0;
    map->m_capacity = capacity;
    map->m_entries = (GameHashmapEntry*)calloc(capacity, sizeof(GameHashmapEntry));
    map->m_idMap = (size_t*)calloc(capacity, sizeof(size_t));
    AssertNew(map->m_entries);
    AssertNew(map->m_idMap);

    for (int i = 0; i < capacity; i++)
    {
        map->m_idMap[i] = -1;
    }

    return map;
}

void HashMap_destroy(GameHashmap* map)
{
    if (!map) return;

    free(map->m_entries);
    free(map->m_idMap);
    free(map);
}

int HashMap_GetSize(GameHashmap* map)
{
    return map->m_size;
}


/*  // pas sur sûr ça soit utile cette merde 
GameHashmapEntry HashMap_Get(GameHashmap map)
{
    assert(map);

    uint32 idx = hash(key, map->capacity);

    HashEntry* tmp = map->entries[idx];

    while (tmp)
    {
        if (!strcmp(tmp->key, key))
        {
            return tmp->grid;
        }
        else tmp = tmp->next;
    }

    return NULL;
}
*/


GameHashmap* HashMap_Insert(GameHashmap* map, GameHashmapEntry value)
{
    assert(map);

    if (map->m_size * 2 > map->m_capacity)
    {
        // TODO redimentionner

        map = HashMap_Resize(map);
    }

    uint32 idx = (uint32)hash(value, map->m_capacity);

    // TODO vérifier qu elle est pas déjà là cette saloperie

    map->m_entries[map->m_size] = value;

    while ((int)map->m_idMap[idx] >= 0)
    {
        idx = (idx + 1) % map->m_capacity;
    }

    map->m_idMap[idx] = map->m_size;

    map->m_size++;

    return map;
}


/*   STANDBY je suis même pas sûr d'avoir besoin de remove
void* HashMap_Remove(GameHashmap* map,)
{
    assert(map && key);

    uint32 idx = hash(key, map->capacity);

    HashEntry* curr = map->entries[idx];
    HashEntry* prev = NULL;

    while (curr)
    {
        if (strcmp(key, curr->key) == 0)
        {
            if (prev)
            {
                prev->next = curr->next;
            }
            else
            {
                map->entries[idx] = curr->next;
            }

            void* res = curr->grid;

            free(curr->key);
            free(curr);

            map->size--;

            return res;
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
    return NULL;
}
*/ 


GameHashmap* HashMap_Resize(GameHashmap* map)
{
    GameHashmap* NewMap = HashMap_New(map->m_capacity*2);
    for (int i = 0; i < map->m_size; i++)
    {
        HashMap_Insert(NewMap, map->m_entries[i]);
    }

    HashMap_destroy(map);

    return NewMap;
}


static bool Compare(GameCore grid1, GameCore grid2);
static bool Compare(GameCore grid1, GameCore grid2)
{
    for (int i = 0; i < (g_gameConfig.Settings->RabbitCount); i++)
    {
        bool tmp = false;
        for (int j = 0; j < g_gameConfig.Settings->RabbitCount; j++)
        {
            if (grid1.Rabbits[i].CellX == grid2.Rabbits[j].CellX &&
                grid1.Rabbits[i].CellY == grid2.Rabbits[j].CellY) tmp = true;
        }
        if (!tmp) return false;
    }

    for (int i = g_gameConfig.Settings->RabbitCount; i < g_gameConfig.Settings->FoxCount; i++)
    {
        bool tmp = false;
        for (int j = g_gameConfig.Settings->RabbitCount; j < g_gameConfig.Settings->FoxCount; j++)
        {
            if (grid1.Rabbits[i].CellX == grid2.Rabbits[j].CellX &&
                grid1.Rabbits[i].CellY == grid2.Rabbits[j].CellY) tmp = true;
        }
        if (!tmp) return false;
    }

    return true;
}


bool alreadyIn(GameHashmap* map, GameHashmapEntry grid)
{
    uint32 idx = hash(grid,map->m_capacity);

    while (map->m_idMap[idx] >= 0)
    {
        if (Compare(map->m_entries[idx].currState, grid.currState)) return true;
        idx = (idx + 1) % map->m_capacity;
    }

    return false;
}


void printGrid(GameCore grid)
{
    char tab[25];
    for (int i = 0; i < 25; i++)
    {
        tab[i] = '/';
    }

    tab[0] = 'O';
    tab[4] = 'O';
    tab[12] = 'O';
    tab[20] = 'O';
    tab[24] = 'O';

    for (int i = 0; i < (g_gameConfig.Settings->RabbitCount); i++)
    {
        int x = grid.Rabbits[i].CellX;
        int y = grid.Rabbits[i].CellY;
        tab[x + 5 * y] = 'L';
    }
    for (int i = g_gameConfig.Settings->RabbitCount; i < (g_gameConfig.Settings->FoxCount + g_gameConfig.Settings->RabbitCount); i++)
    {
        int x = grid.Rabbits[i].CellX;
        int y = grid.Rabbits[i].CellY;
        tab[x + 5 * y] = 'F';
        switch (grid.Rabbits[i].Direction)
        {
        case(RABBIT_NORTH):
            y -= 1;
            break;
        case(RABBIT_EAST):
            x += 1;
            break;
        case(RABBIT_SOUTH):
            y += 1;
            break;
        case(RABBIT_WEST):
            x -= 1;
            break;
        default:
            break;
        }
        tab[x + 5 * y] = 'F';

    }
    for (int i = g_gameConfig.Settings->RabbitCount + g_gameConfig.Settings->FoxCount;
        i < (g_gameConfig.Settings->MushroomCount + g_gameConfig.Settings->RabbitCount + g_gameConfig.Settings->FoxCount); i++)
    {
        int x = grid.Rabbits[i].CellX;
        int y = grid.Rabbits[i].CellY;
        tab[x + 5 * y] = 'M';
    }

    for (int i = 0; i < 25; i++)
    {
        if ((i%5)==0)
        {
            printf("\n");
        }
        printf("%c",tab[i]);
    }
    printf("\n");
}
