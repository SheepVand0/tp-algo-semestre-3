/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

#define GAME_GRID_SIZE 5

typedef struct Scene Scene;

typedef struct GameCore
{
    int todo;
} GameCore;

typedef enum EObjectType
{
    NO_OBJECT,
    RABBIT,
    OBSTACLE

} EObjectType;

typedef struct Rabbit
{

    SpriteGroup* RabbitSprite;

    int CellX;
    int CellY;

} Rabbit;

typedef struct Obstacle
{

    SpriteGroup* ObstacleSprite;

    int CellX0;
    int CellY0;

    int CellX1;
    int CellY1;

    bool Movable;
} Obstacle;

typedef struct SelectedObject
{
    Rabbit* SelectedRabbit;
    Obstacle* SelectedObstacle;

    EObjectType ObjectType;

} SelectedObject;

Rabbit* Rabbit_create(Scene* scene, int cellX, int cellY);

void Rabbit_destroy(Rabbit* rabbit);

Obstacle* Fox_create(Scene* scene, int cellX0, int cellY0, int cellX1, int cellY1);

void Obstacle_destroy(Obstacle* obstacle);

SelectedObject* SelectedObject_create(void* object, EObjectType type);

void SelectedObject_destroy(SelectedObject* obj);

// TODO : MOVE FUNCTIONS
