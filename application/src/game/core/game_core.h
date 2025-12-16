/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/game_settings.h"
#include "game/input.h"
#include "../audio/audio_manager.h"

#define GAME_GRID_SIZE 5
#define MAX_RABBITS 10
#define MAX_FOXES 5
#define MAX_MUSHROOMS 10

#define RABBIT_COUNT gameCore->Settings->RabbitCount
#define FOX_COUNT gameCore->Settings->FoxCount
#define MUSHROOM_COUNT gameCore->Settings->MushroomCount

typedef struct Scene Scene;

typedef struct Vec2Int Vec2Int;

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
    RABBIT_SOUTH,
    RABBIT_WEST
} ERabbitDirection;

typedef enum EGameState
{
    NONE,
    PLAYING,
    EDITING,
    WINNING,
    GAMBLING,
    OUPI_GOUPI,
    MASTERMIND,
    GETTING_LARRIED
} EGameState;

typedef struct Rabbit
{
    SpriteGroup* RabbitSprite;

    int CellX;
    int CellY;

    ERabbitDirection Direction;

    bool Movable;

    EObjectType Type;

} Rabbit;

typedef struct GameCore
{
    Rabbit Rabbits[MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS];
    GameSettings* Settings;
    Rabbit* Selected;
    AssetManager* Assets;

    EGameState State;

    SAudio* LarryMusic;

    float Remaining;

    float CurrentAnimationTime;
} GameCore;

GameCore* GameCore_create(AssetManager* assetManager, AudioManager* audio);

void GameCore_update(GameCore* gameCore, Scene* scene, int selectedX, int selectedY);

void GameCore_destroyGame(GameCore* gameCore);

void GameCore_initNextGame(GameCore* gameCore);

EObjectType GameCore_getObjTypeAtLocation(GameCore* gameCore, int cellx, int y);

Rabbit* GameCore_getRabbit(GameCore* gameCore, int index);

Rabbit* GameCore_getMushroom(GameCore* gameCore, int index);

Rabbit* GameCore_getFox(GameCore* gameCore, int index);

Rabbit* Rabbit_create(GameCore* scene, int cellX, int cellY);

void Rabbit_destroy(Rabbit* rabbit);


// TODO : MOVE FUNCTIONS
bool Rabbit_canMove(Rabbit* rabbit, GameCore* gameCore, int targetX, int targetY);

bool Rabbit_move(Rabbit* rabbit, GameCore* gameCore, int targetX, int targetY);
 
Rabbit* Fox_create(GameCore* scene, int cellX0, int cellY0, ERabbitDirection direciton);

Vec2 Fox_getSecondCell(Rabbit* rabbit);

Vec2 Fox_getDirection(Rabbit* rabbit);

void Rabbit_getAnchorAngAngleFromDirection(ERabbitDirection direction, Vec2* anchor, float* angle);

Rabbit* Mushroom_create(GameCore* scene, int cellX, int cellY); 
