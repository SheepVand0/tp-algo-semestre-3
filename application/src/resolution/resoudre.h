#pragma once 

#include "hashmap.h"
#include "../game/scene.h"
#include "../game/game_config.h"


/// @brief bah résoud le bordel quoi (jsp, j'espère fort)
void resoudre();

/// @brief résoud aussi mais récursivement (WOW)
GameHashmapEntry* resoudre_Rec(GameHashmap* map, GameHashmapEntry state);


int remonter(GameHashmap* map, GameHashmapEntry* solution);
