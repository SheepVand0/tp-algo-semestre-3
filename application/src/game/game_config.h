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
    FISC_GUY,
    GAMBLING_COUNT
} EGamblingResult;

typedef struct GameConfig
{
    GameScene nextScene;

    bool showElapsedTime;
    bool inLevel;
    bool isEditing;

    float gamblingAnimTime;

    GameCore* core;
    GameSettings* settings;
    AssetManager* assets;
    AudioManager* audio;

    SAudio* larryAudio;
    SAudio* amongUsAudio;
    SAudio* footstep0;
    SAudio* footstep1;

    Rabbit* selected;

    EGameState state;

    float remaining;

    float currentAnimationTime;

    EGamblingResult gamblingResult;
    Vec2 candyPos;
    Vec2 candyVel;
    bool candyWaitingForLeftClick;
    bool candyHasTakenOver;

    bool completedGambling[GAMBLING_COUNT];

    bool isFullscreen;

    float inputLockTime;

    int solveObjectIndex;
    Rabbit solveNextSol[2];
    Vec2 fiscTargetPos;
    Vec2 fiscNextMovePos;

    bool userHasPulledAMove;
} GameConfig;

void GameConfig_init();

extern GameConfig g_gameConfig;
