#pragma once

#include "settings.h"
#include "game/core/game_core.h"


extern GameCore g_gameGenerationResult;

typedef struct GameHashmap GameHashmap;
typedef struct GameHashmapEntry GameHashmapEntry;

/// <summary>
/// Naive generation
/// </summary>
/// <param name="rabbCount"></param>
/// <param name="foxCount"></param>
/// <param name="mushroomCount"></param>
/// <param name="moveCount"></param>
/// <returns></returns>
GameCore* Generate(int rabbCount, int foxCount, int mushroomCount, int moveCount);

/// <summary>
/// Generate clean but cannot change fox count & mushromm count
/// </summary>
/// <param name="rabbCount"></param>
/// <param name="moveCount"></param>
/// <returns></returns>
GameCore* GenerateClean(int rabbCount, int moveCount);

void Generator_printCurrent(GameCore core);

void Generator_copyGame(GameCore* target, GameCore* original);

/// <summary>
/// Stolen from leo
/// </summary>
/// <param name="map"></param>
/// <param name="grid"></param>
/// <returns></returns>
GameHashmapEntry* Generator_searchEntryAdressInHashmap(GameHashmap* map, GameHashmapEntry* grid);

/// <summary>
/// Stolen from leo too, but with one more if
/// </summary>
/// <param name="hashmap"></param>
/// <param name="pattern"></param>
/// <returns></returns>
int Generator_countMoves(GameHashmap* hashmap, GameHashmapEntry* pattern);
