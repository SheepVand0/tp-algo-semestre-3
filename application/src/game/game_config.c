/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/game_config.h"
#include "common/assets.h"
#include "common/common.h"

GameConfig g_gameConfig = { 0 };

void GameConfig_init()
{
    memset(&g_gameConfig, 0, sizeof(GameConfig));
    g_gameConfig.nextScene = GAME_SCENE_MAIN;
    g_gameConfig.inLevel = false;
    g_gameConfig.isEditing = false;
    g_gameConfig.gamblingAnimTime = 0;
    g_gameConfig.core = NULL;
    g_gameConfig.assets = NULL;
    g_gameConfig.larryAudio = NULL;
    g_gameConfig.settings = GameSettings_create();
    g_gameConfig.audio = AudioManager_create();
    g_gameConfig.isFullscreen = false;
    g_gameConfig.candyHasTakenOver = false;
    g_gameConfig.inputLockTime = 0.f;
    g_gameConfig.solveObjectIndex = -1;
    g_gameConfig.footstep0 = AudioManager_loadWav(g_gameConfig.audio, "footstep_0.wav", "footstep0");
    g_gameConfig.footstep1 = AudioManager_loadWav(g_gameConfig.audio, "footstep_1.wav", "footstep1");
    g_gameConfig.userHasPulledAMove = false;

    g_gameConfig.larryAudio = AudioManager_loadWav(g_gameConfig.audio, "larry_end_screen.wav", "larry-end-screen");
    g_gameConfig.amongUsAudio = AudioManager_loadWav(g_gameConfig.audio, "among_us_win.wav", "among-us-win");
}
