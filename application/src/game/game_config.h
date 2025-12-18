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
    GETTING_LARRIED
} EGameState;

typedef enum EGamblingResult
{
    CANDY,
    OUPI_GOUPI,
    MASTERMIND,
    GAMBLING_COUNT
} EGamblingResult;

typedef struct GameConfig
{
    GameScene nextScene;

    bool showElapsedTime;
    bool inLevel;
    bool isEditing;

    float GamblingAnimTime;

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

    EGamblingResult GamblingResult;
    Vec2 CandyPos;
    Vec2 CandyVel;
    bool CandyWaitingForLeftClick;

    bool CompletedGambling[GAMBLING_COUNT];

    bool IsFullscreen;
} GameConfig;

void GameConfig_init();

extern GameConfig g_gameConfig;
