/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_core.h"

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

typedef struct GameConfig
{
    GameScene nextScene;

    bool showElapsedTime;
    bool inLevel;
    bool isEditing;

    GameCore* Core;
    GameSettings* Settings;
    AssetManager* Assets;
    AudioManager* Audio;

    SAudio* LarryAudio;
    SAudio* AmongUsAudio;

    Rabbit* Selected;

    EGameState State;

    float Remaining;

    float CurrentAnimationTime;
} GameConfig;

void GameConfig_init();

extern GameConfig g_gameConfig;
