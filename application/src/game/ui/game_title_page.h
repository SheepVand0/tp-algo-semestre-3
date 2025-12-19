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
#include "game/audio/audio_manager.h"

typedef struct Scene Scene;

typedef struct GameTitlePage
{
    Scene* m_scene;
    GameUIManager* m_manager;
    UIFocusManager* m_focusManager;

    GameUIAction m_nextAction;
    UIObject* m_mainPanel;

    SpriteGroup* m_lowQualityCatSprite;
    float m_animationTime;

    UIButton* m_settingsButton;
    UIButton* m_playButton;

    UIList* m_rabbitList;
    UIList* m_foxList;
    UIList* m_mushroomList;
    UIList* m_moveCountList;
    UILabel* m_generatorWarning;

    int m_lastPos;
    float m_spriteScale;
    float m_lastFixedTime;

    SAudio* m_footstep0;
    SAudio* m_footstep1;
    SAudio* m_eating;
} GameTitlePage;

GameTitlePage* GameTitlePage_create(Scene* scene, GameUIManager *manager);
void GameTitlePage_destroy(GameTitlePage* self);
void GameTitlePage_update(GameTitlePage* self, UIInput* input);

void GameTitlePage_render(GameTitlePage* self);
