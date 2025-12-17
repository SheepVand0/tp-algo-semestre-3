/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/game_title_page.h"
#include "game/ui/game_ui_manager.h"
#include "game/scene.h"
#include "common/ui_style.h"
#include <SDL3/SDL.h>
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_mixer.h"
#include "windows.h"
#include <stdio.h>

static void GameTitlePage_emptyCallback(void* selectable)
{

}

static void GameTitlePage_onClick(void* selectable)
{
    GameTitlePage* self = (GameTitlePage*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    int action = UISelectable_getUserId(selectable);
    assert(action >= 0 && action < GAME_UI_ACTION_COUNT && "Invalid action id");

    self->m_nextAction = action;

    printf("Button clicked: %s\n", UIObject_getObjectName(selectable));
}

GameTitlePage* GameTitlePage_create(Scene* scene, GameUIManager *manager)
{
    UICanvas* canvas = manager->m_canvas;
    AssetManager* assets = Scene_getAssetManager(scene);
    UIRect rect = { 0 };
    TTF_Font* font = NULL;

    GameTitlePage* self = (GameTitlePage*)calloc(1, sizeof(GameTitlePage));
    AssertNew(self);

    self->m_manager = manager;
    self->m_scene = scene;
    self->m_mainPanel = UIObject_create("main-panel");
    self->m_nextAction = GAME_UI_ACTION_NONE;
    self->m_focusManager = UIFocusManager_create();
    self->AnimationTime = 0.f;
    self->lastPos = -1;
    self->SpriteScale = 1.f;
    self->Eating = AudioManager_loadWav(g_gameConfig.Audio, "eating.wav", "eating");

    self->Footstep0 = AudioManager_loadWav(g_gameConfig.Audio, "footstep_0.wav", "footstep0");
    self->Footstep1 = AudioManager_loadWav(g_gameConfig.Audio, "footstep_1.wav", "footstep1");

    self->LowQualityCatSprite = SpriteSheet_getGroupByName(AssetManager_getSpriteSheet(assets, SPRITE_LOW_QUALITY_CAT), "cat");
    UIFocusManager_setCanvas(self->m_focusManager, canvas);

    UIObject_setParent(self->m_mainPanel, canvas);
    rect.anchorMin = Vec2_set(0.5f, 0.0f);
    rect.anchorMax = Vec2_set(0.5f, 1.0f);
    rect.offsetMin = Vec2_set(-75.f, 0.0f);
    rect.offsetMax = Vec2_set(+75.f, 0.0f);
    UIObject_setRect(self->m_mainPanel, rect);
    
    UIGridLayout* layout = UIGridLayout_create("main-layout", 5, 1);
    UIObject_setParent(layout, self->m_mainPanel);
    UIGridLayout_setRowSizes(layout, 25.0f);
    UIGridLayout_setRowSpacings(layout, 5.f);
    UIGridLayout_setRowSpacing(layout, 0, 20.f);

    font = AssetManager_getFont(assets, FONT_NORMAL);
    UILabel* label = UILabel_create("title-label", font);
    UILabel_setTextString(label, "LARRY");
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.orange2);

    UIGridLayout_addObject(layout, label, 0, 0, 1, 1);

    const char* buttonNames[4] = { "start-button", "settings-button", "custom-levels-button", "quit-button" };
    GameUIAction buttonActions[4] = {
        GAME_UI_ACTION_START,
        GAME_UI_ACTION_OPEN_SETTINGS,
        GAME_UI_ACTION_OPEN_EDITOR,
        GAME_UI_ACTION_QUIT
    };
    const char* buttonLabels[4] = { "Start", "Settings", "Custom levels", "Quit" };
    font = AssetManager_getFont(assets, FONT_NORMAL);
    for (int i = 0; i < 4; ++i)
    {
        UIButton* button = UIButton_create(buttonNames[i], font);
        UIButton_setLabelString(button, buttonLabels[i]);
        UIButton_setOnClickCallback(button, GameTitlePage_onClick);
        UISelectable_setUserData(button, self);
        UISelectable_setUserId(button, (int)buttonActions[i]);
        UIStyle_setDefaultButton(button);
        UIGridLayout_addObject(layout, button, i + 1, 0, 1, 1);
        UIFocusManager_addSelectable(self->m_focusManager, button);
        if (i == 0)
        {
            UIFocusManager_setFocused(self->m_focusManager, button);
        }

        if (i == 1) self->SettingsButton = button;
    }

    return self;
}

void GameTitlePage_destroy(GameTitlePage* self)
{
    if (!self) return;

    UIObject_destroy(self->m_mainPanel);
    UIFocusManager_destroy(self->m_focusManager);

    free(self);
}


void GameTitlePage_update(GameTitlePage* self, UIInput* input)
{
    UIFocusManager_update(self->m_focusManager, input);

    switch (self->m_nextAction)
    {
    case GAME_UI_ACTION_START:
        GameCore_initNextGame(g_gameConfig.Core);
        self->m_manager->m_nextAction = GAME_UI_ACTION_START;
        break;
    case GAME_UI_ACTION_OPEN_SETTINGS:
        self->m_manager->m_nextAction = GAME_UI_ACTION_OPEN_SETTINGS;
        break;
    case GAME_UI_ACTION_QUIT:
        Scene_quit(self->m_scene);
        break;
    case GAME_UI_ACTION_OPEN_EDITOR:
        self->m_manager->m_nextAction = GAME_UI_ACTION_OPEN_EDITOR;
        break;
    default:
        break;
    }
    self->m_nextAction = GAME_UI_ACTION_NONE;
}

#define ANIM_PRE_DURATION 5
#define ANIM_DISTANCE_X 480
#define ANIM_POS_Y = 370
#define ANIM_DURATION_IN_SEC 8
#define ANIM_EAT_SUPP_TIME 4

void GameTitlePage_render(GameTitlePage* self)
{
    self->AnimationTime += Timer_getDelta(g_time);

    if (self->AnimationTime >= ANIM_PRE_DURATION)
    {
        float fixedAnimTime = self->AnimationTime - ANIM_PRE_DURATION;
        int pos = 0;

        if (fixedAnimTime < ANIM_DURATION_IN_SEC)
        {
            pos = (fixedAnimTime) * (ANIM_DISTANCE_X / ANIM_DURATION_IN_SEC);
            pos -= pos % 40;
        }
        else if (fixedAnimTime >= ANIM_DURATION_IN_SEC && fixedAnimTime <= ANIM_DURATION_IN_SEC + ANIM_EAT_SUPP_TIME)
        {
            pos = ANIM_DISTANCE_X;
            if (fixedAnimTime >= ANIM_DURATION_IN_SEC + (ANIM_EAT_SUPP_TIME - 1) && self->lastFixedTime <= ANIM_DURATION_IN_SEC + (ANIM_EAT_SUPP_TIME - 1))
            {
                UIObject_setEnabled(self->SettingsButton, false);
                UIButton_setOnClickCallback(self->SettingsButton, GameTitlePage_emptyCallback);
                AudioManager_play(g_gameConfig.Audio, self->Eating);
            }
        }
        else
        {
            float animTime = fixedAnimTime - ANIM_DURATION_IN_SEC - ANIM_EAT_SUPP_TIME;
            pos = ANIM_DISTANCE_X - ((animTime) * (ANIM_DISTANCE_X / ANIM_DURATION_IN_SEC));
            pos -= pos % 40;

            if (animTime >= ANIM_DURATION_IN_SEC) return;
        }

        if (pos != self->lastPos)
        {
            
            self->SpriteScale *= -1;
            AudioManager_play(g_gameConfig.Audio, self->SpriteScale == 1 ? self->Footstep0 : self->Footstep1);
        }

        SDL_FRect l_CatRect = { 0 };
        l_CatRect.x = pos;
        l_CatRect.y = 370;
        l_CatRect.w = self->SpriteScale * 40.f;
        l_CatRect.h = 42.f;

        SpriteGroup_render(self->LowQualityCatSprite, 0, &l_CatRect, Vec2_anchor_center, 1.f);

        self->lastPos = pos;
        self->lastFixedTime = fixedAnimTime;
    }
}
