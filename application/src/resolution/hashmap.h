#pragma once

#include "settings.h"
#include "../game/game_config.h"


//typedef struct HashEntry HashEntry;
//struct HashEntry
//{
//    HashEntry* next;
//    int* key;
//    GridInfo* grid;
//};
//
//typedef struct HashMap
//{
//    int size;
//    int capacity;
//    HashEntry** entries;
//} HashMap;

typedef struct GameHashmapEntry
{
    //int nbrCoups;
    GameCore prevState;
    GameCore currState;
} GameHashmapEntry;

typedef struct GameHashmap
{
    GameHashmapEntry* m_entries;
    size_t* m_idMap;
    size_t m_capacity;
    size_t m_size;
} GameHashmap;



/// @brief Crée une table de hachage vide.
/// 
/// @param capacity la capacité de la table.
/// @return La table de hachage créée.
GameHashmap* HashMap_New(size_t capacity);

/// @brief Détruit une table de hachage.
/// Les clés sont automatiquement libérées mais les données associées ne le sont pas.
/// L'utilisateur doit donc parcourir le dictionnaire pour les libérer avant d'appeler cette méthode.
/// 
/// @param self la table de hachage.
void HashMap_destroy(GameHashmap* self);

/// @brief Renvoie la taille d'une table de hachage.
/// 
/// @param self la table de hachage.
/// @return La taille de la table de hachage.
int HashMap_GetSize(GameHashmap* self);

/// @brief Renvoie la grid associée à une clé dans la table de hachage
/// ou NULL si la clé n'est pas présente.
/// 
/// @param self la table de hachage.
/// @param key la clé.
/// @return la valeur associée à la clé.
//GridInfo* HashMap_Get(GameHashmap* self, int* key);

/// @brief Ajoute un couple clé/valeur dans une table si la clé n'est pas présente,
/// sinon modifie la valeur associée à une clé.
/// La chaîne de caractères représentant la clé est copiée dans la table.
/// 
/// @param self la table de hachage.
/// @param key la clé.
/// @param value la valeur associée à la clé.
/// @return Si la clé était déjà présente dans la table, renvoie la valeur précédente
/// (pour que l'utilisateur puisse libérer la mémoire), sinon renvoie NULL.
GameHashmap* HashMap_Insert(GameHashmap* map, GameHashmapEntry value);

/// @brief Supprime une clé dans une table de hachage.
/// Cette fonction est sans effet si la clé n'est pas présente.
/// 
/// @param self la table de hachage.
/// @param key la clé.
/// @return Si la clé est présente dans la table, renvoie la valeur associée
/// (pour que l'utilisateur puisse libérer la mémoire), sinon renvoie NULL.
//void* HashMap_Remove(GameHashmap* self, int* key);


/// @brief double la taille de la hashmap
GameHashmap* HashMap_Resize(GameHashmap* map);

/// @brief printgrid
void printGrid(GameCore grid);

