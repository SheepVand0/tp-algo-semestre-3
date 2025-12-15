/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

#define GAME_GRID_SIZE 5

typedef struct Scene Scene;

typedef struct Vec2Int Vec2Int;

typedef struct GameCore
{
    int todo;
} GameCore;

typedef enum EObjectType
{
    NO_OBJECT,
    RABBIT,
    FOX,
    MUSHROOM

} EObjectType;

typedef enum ERabbitDirection
{
    RABBIT_NORTH,
    RABBIT_EAST,
    RABBIT_WEST,
    RABBIT_SOUTH
} ERabbitDirection;

typedef struct Rabbit
{
    SpriteGroup* RabbitSprite;

    int CellX;
    int CellY;

    ERabbitDirection Direction;

    bool Movable;

    EObjectType Type;

} Rabbit;

Rabbit* Rabbit_create(Scene* scene, int cellX, int cellY);

void Rabbit_destroy(Rabbit* rabbit);

// TODO : MOVE FUNCTIONS
bool Rabbit_move(Rabbit* rabbit, Scene* scene, int targetX, int targetY);
 
Rabbit* Fox_create(Scene* scene, int cellX0, int cellY0, ERabbitDirection direciton);

Vec2 Fox_getSecondCell(Rabbit* rabbit);

Vec2 Fox_getDirection(Rabbit* rabbit);
