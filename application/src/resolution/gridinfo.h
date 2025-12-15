#pragma once

#include "settings.h"
#include "game/core/game_core.h"

typedef struct GridInfo
{
    int nbrCoups;      // nombre de coups depuis le début
    int Rnbr;           // nombre de lapins
    Vec2Int* Rpos;     // positions des lapinou
    int Fnbr;           // nombre de renards
    Vec2Int* Fpos;     // positions des renardo
}GridInfo;

/// @brief renvoie la position d'un lapin
/// @param le lapin dont on souhaite obtenir la position
Vec2Int GetRabbitPosition(Rabbit* rabbit);

/// @brief renvoie le nombre de lapins sur le plateau
int GetRabbitNumber();

/// @brief renvoie la position d'un renard
/// @param le renard dont on souhaite obtenir la position
Vec2Int GetFoxPosition();

/// @brief renvoie le nombre de renards sur le plateau
int GetFoxNumber();


/// @brief true si les grilles sont identiques
/// @param grid1, grid2 les grilles a comparer 
bool isSameGrid(GridInfo* grid1, GridInfo* grid2);
