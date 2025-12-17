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
    g_gameConfig.GamblingAnimTime = 0;
    g_gameConfig.Core = NULL;
    g_gameConfig.Assets = NULL;
    g_gameConfig.LarryAudio = NULL;
    g_gameConfig.Settings = GameSettings_create();
    g_gameConfig.Audio = AudioManager_create();

    g_gameConfig.LarryAudio = AudioManager_loadWav(g_gameConfig.Audio, "larry_end_screen.wav", "larry-end-screen");
    g_gameConfig.AmongUsAudio = AudioManager_loadWav(g_gameConfig.Audio, "among_us_win.wav", "among-us-win");
}
