#pragma once 

#include "hashmap.h"
#include "../game/scene.h"
//#include "../game/game_config.h"


/// @brief bah résoud le bordel quoi (jsp, j'espère fort)
int solve(GameCore* target);

/// @brief résoud aussi mais récursivement (WOW)
//GameHashmapEntry* solve_recursive(GameHashmap* map, GameHashmapEntry state);


int climbBack(GameHashmap* map, GameHashmapEntry* solution);

