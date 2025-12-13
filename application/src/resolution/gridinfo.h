#pragma once

#include "settings.h"
#include "game/core/game_core.h"

typedef struct GridInfo
{
    int rabbitNumber;
    Vec2Int* rabbitPositions;
    int foxNumber;
    Vec4Int* foxPositions;
}GridInfo;

/// @brief renvoie la position d'un lapin
/// @param le lapin dont on souhaite obtenir la position
Vec2Int GetRabbitPosition(Rabbit* rabbit);

/// @brief renvoie le nombre de lapins sur le plateau
int GetRabbitNumber();

/// @brief renvoie la position d'un renard
/// @param le renard dont on souhaite obtenir la position
Vec4Int GetFoxPosition();

/// @brief renvoie le nombre de renards sur le plateau
int GetFoxNumber();



