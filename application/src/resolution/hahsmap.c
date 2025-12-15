#include "hashmap.h"

typedef unsigned int uint32;

// Fonction de hachage des clés de la table de hachage
static uint32 hash(GameHashmapEntry grid, uint32 capacity)
{
    return NULL;
}

GameHashmap* HashMap_New(int capacity)
{
    GameHashmap* map = (GameHashmap*)calloc(1, sizeof(GameHashmap));
    AssertNew(map);

    map->m_size = 0;
    map->m_capacity = capacity;
    map->m_entries = (GameHashmapEntry*)calloc(capacity, sizeof(GameHashmapEntry));
    map->m_idMap = (size_t*)calloc(capacity, sizeof(size_t));
    AssertNew(map->m_entries && map->m_idMap);

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


void HashMap_Insert(GameHashmap* map, GameHashmapEntry value)
{
    assert(map);

    uint32 idx = hash(value, map->m_capacity);

    if (map->m_size * 2 > map->m_capacity)
    {
        // TODO redimentionner
        map = HashMap_Resize(map);
    }

    // TODO vérifier qu elle est pas déjà là cette saloperie

    map->m_entries[map->m_size] = value;

    while (map->m_idMap[idx] >= 0)
    {
        idx++ % map->m_capacity;
    }
    map->m_idMap[idx] = map->m_size;

    map->m_size++;
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
    GameHashmap* NewMap = HashMap_New(map->m_capacity * 2);
    for (int i = 0; i < map->m_size; i++)
    {
        HashMap_Insert(NewMap, map->m_entries[i]);
    }

    HashMap_destroy(map);

    return NewMap;
}
