/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/game_ui_manager.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_mixer.h"

typedef struct Scene Scene;

typedef struct GameTitlePage
{
    Scene* m_scene;
    GameUIManager* m_manager;
    UIFocusManager* m_focusManager;

    GameUIAction m_nextAction;
    UIObject* m_mainPanel;

    MIX_Mixer* Mixer;
} GameTitlePage;

GameTitlePage* GameTitlePage_create(Scene* scene, GameUIManager *manager);
void GameTitlePage_destroy(GameTitlePage* self);
void GameTitlePage_update(GameTitlePage* self, UIInput* input);

