#include "hashmap.h"

typedef unsigned int uint32;

// Fonction de hachage des clés de la table de hachage
static uint32 hash(char* str, uint32 capacity)
{
    return NULL;
}

HashMap* HashMap_New(int capacity)
{
    HashMap* map = (HashMap*)calloc(1, sizeof(HashMap));
    AssertNew(map);

    map->capacity = capacity;
    map->entries = (HashEntry**)calloc(capacity, sizeof(HashEntry*));
    AssertNew(map->entries);

    return map;
}

void HashMap_destroy(HashMap* map)
{
    if (!map) return;

    int capacity = map->capacity;
    HashEntry** entries = map->entries;
    for (int h = 0; h < capacity; h++)
    {
        HashEntry* entry = entries[h];
        while (entry != NULL)
        {
            HashEntry* next = entry->next;

            free(entry->key);
            free(entry);

            entry = next;
        }
    }
    free(map);
}

int HashMap_GetSize(HashMap* map)
{
    return map->size;
}

GridInfo* HashMap_Get(HashMap* map, int* key)
{
    assert(map && key);

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

void* HashMap_Insert(HashMap* map, char* key, void* value)
{
    assert(map && key);

    uint32 idx = hash(key, map->capacity);

    HashEntry* curr = map->entries[idx];
    HashEntry* prev = NULL;

    while (curr)
    {
        if (strcmp(curr->key, key) == 0)
        {
            void* old = curr->grid;
            curr->grid = value;
            return old;
        }
        prev = curr;
        curr = curr->next;
    }

    HashEntry* newOcc = calloc(1, sizeof(HashEntry));
    newOcc->key = strdup(key);
    newOcc->grid = value;
    newOcc->next = NULL;

    if (prev)
    {
        prev->next = newOcc;
    }
    else
    {
        map->entries[idx] = newOcc;
    }

    map->size++;

    return NULL;
}

void* HashMap_Remove(HashMap* map, char* key)
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
