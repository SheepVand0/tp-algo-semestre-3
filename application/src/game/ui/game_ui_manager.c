/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/game_ui_manager.h"
#include "game/ui/game_title_page.h"
#include "game/ui/game_settings_page.h"
#include "game/scene.h"
#include "game/game_config.h"
#include "game/ui/editor_ui_manager.h"

static void GameUIManager_closePages(GameUIManager* self);

static void GameUIManager_onLeaveClicked(void* selectable)
{
    g_gameConfig.inLevel = false;
    g_gameConfig.Core->State = NONE;

    //GameCore_destroyGame(g_gameConfig.Core);

    GameUIManager* l_Manager = (GameUIManager*)UISelectable_getUserData(selectable);
    assert(l_Manager);

    l_Manager->m_nextAction = GAME_UI_ACTION_OPEN_TITLE;
}

GameUIManager* GameUIManager_create(Scene* scene)
{
    GameUIManager* self = (GameUIManager*)calloc(1, sizeof(GameUIManager));
    AssertNew(self);

    self->m_scene = scene;

    UICanvas* canvas = UICanvas_create("canvas");
    self->m_canvas = canvas;
    self->m_titlePage = GameTitlePage_create(scene, self);
    self->m_settingsPage = NULL;
    self->FocusManager = UIFocusManager_create();

    AssetManager* assets = Scene_getAssetManager(scene);

    self->m_timeTextLayout = UIGridLayout_create("time-text-layout", 4, 5);
    UIObject_setParent(self->m_timeTextLayout, canvas);
    UIGridLayout_setRowSizes(self->m_timeTextLayout, 25.0f);
    UIGridLayout_setRowSpacings(self->m_timeTextLayout, 5.f);
    UIGridLayout_setRowSpacing(self->m_timeTextLayout, 0, 20.f);
    UIGridLayout_setAnchor(self->m_timeTextLayout, Vec2_anchor_north);
    UIGridLayout_setPadding(self->m_timeTextLayout, Vec2_set(6.f, 0));

    self->m_timeText = UILabel_create("time-text", AssetManager_getFont(assets, FONT_NORMAL));
    UILabel_setTextString(self->m_timeText, "time_text");
    UILabel_setAnchor(self->m_timeText, Vec2_anchor_north);
    UILabel_setColor(self->m_timeText, g_colors.orange4);

    self->LeaveGameButton = UIButton_create("leave-button", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->LeaveGameButton, "Leave");
    UISelectable_setUserData(self->LeaveGameButton, self);
    UISelectable_setUserId(self->LeaveGameButton, 0);
    UIButton_setOnClickCallback(self->LeaveGameButton, GameUIManager_onLeaveClicked);

    UIFocusManager_addSelectable(self->FocusManager, self->LeaveGameButton);

    self->m_lostTextLayout = UIGridLayout_create("lost-text-layout", 2, 1);
    UIObject_setParent(self->m_lostTextLayout, canvas);
    UIGridLayout_setRowSizes(self->m_lostTextLayout, 25.0f);
    UIGridLayout_setRowSpacings(self->m_lostTextLayout, 5.f);
    UIGridLayout_setRowSpacing(self->m_lostTextLayout, 0, 20.f);
    UIGridLayout_setAnchor(self->m_lostTextLayout, Vec2_anchor_center);

    self->m_lostText = UILabel_create("lose-text-layout", AssetManager_getFont(assets, FONT_EXTRA_BOLD));
    UILabel_setTextString(self->m_lostText, "YOU LOST");
    UILabel_setAnchor(self->m_lostText, Vec2_anchor_center);
    UILabel_setColor(self->m_lostText, g_colors.red7);
    
    UIObject_setEnabled(self->m_lostText, false);

    UIGridLayout_addObject(self->m_timeTextLayout, self->m_timeText, 0, 2, 1, 1);
    UIGridLayout_addObject(self->m_timeTextLayout, self->LeaveGameButton, 2, 0, 1, 1);
    UIGridLayout_addObject(self->m_lostTextLayout, self->m_lostText, 0, 0, 1, 1);

    return self;
}

void GameUIManager_destroy(GameUIManager* self)
{
    if (!self) return;

    GameUIManager_closePages(self);
    UIObject_destroy(self->m_canvas);

    free(self);
}

static void GameUIManager_closePages(GameUIManager* self)
{
    GameTitlePage_destroy(self->m_titlePage);
    GameSettingsPage_destroy(self->m_settingsPage);
    EditorUI_destroy(self->m_editorPage);

    self->m_titlePage = NULL;
    self->m_settingsPage = NULL;
    self->m_editorPage = NULL;
}

void GameUIManager_update(GameUIManager* self, UIInput* input)
{
    UIObject_setEnabled(self->m_timeTextLayout, g_gameConfig.Core->State == PLAYING);
    UIObject_setEnabled(self->m_lostText, g_gameConfig.Core->State == GETTING_LARRIED || g_gameConfig.Core->State == WINNING);

    if (g_gameConfig.Core && g_gameConfig.inLevel)
    {
        UILabel_setTextString(self->m_timeText, GameUIManager_formatTime(g_gameConfig.Core->Remaining));

        UILabel_setColor(self->m_timeText, GameUIManager_getColorByTime(g_gameConfig.Core->Remaining));
    }

    Scene* scene = self->m_scene;
    UIObject_update(self->m_canvas);
    if (self->m_titlePage)
    {
        GameTitlePage_update(self->m_titlePage, input);
    }
    if (self->m_settingsPage)
    {
        GameSettingsPage_update(self->m_settingsPage, input);
    }
    if (self->m_editorPage)
    {
        EditorUI_update(self->m_editorPage, input);
    }

    UIFocusManager_update(self->FocusManager, input);

    if (self->m_nextAction != GAME_UI_ACTION_NONE)
    {
        switch (self->m_nextAction)
        {
        case GAME_UI_ACTION_OPEN_SETTINGS:
            GameUIManager_closePages(self);
            self->m_settingsPage = GameSettingsPage_create(scene, self);
            break;

        case GAME_UI_ACTION_OPEN_TITLE:
            GameUIManager_closePages(self);
            GameCore_destroyGame(g_gameConfig.Core);
            printf("opening at least");
            self->m_titlePage = GameTitlePage_create(scene, self);
            break;

        case GAME_UI_ACTION_START:
            GameUIManager_closePages(self);
            g_gameConfig.inLevel = true;
            g_gameConfig.isEditing = false;
            GameGraphics_setEnabled(scene->m_gameGraphics, true);
            break;
        case GAME_UI_ACTION_OPEN_EDITOR:
            GameUIManager_closePages(self);
            self->m_editorPage = EditorUI_create(scene, self);
            break;
        case GAME_UI_ACTION_START_EDITOR:
            g_gameConfig.inLevel = true;
            g_gameConfig.isEditing = true;
            GameGraphics_setEnabled(scene->m_gameGraphics, true);
            break;
        }
        self->m_nextAction = GAME_UI_ACTION_NONE;
    }

    UIObject_update(self->m_canvas);
}

char* GameUIManager_formatTime(float time)
{
    char* l_Res = "thaila";

    int itime = (int)time;

    int minutes = itime / 60;
    int seconds = itime - (minutes * 60);

    int unite = minutes - (minutes / 10 * 10);

    int i = 0;
    l_Res[0] = '0' + minutes / 10;
    l_Res[1] = '0' + unite;
    l_Res[2] = ':';

    unite = seconds - (seconds / 10 * 10);

    l_Res[3] = '0' + seconds / 10;
    l_Res[4] = '0' + unite;
    l_Res[5] = '\0';

    return l_Res;
}

SDL_Color GameUIManager_getColorByTime(float time)
{
    if (time > 20.f) return g_colors.white;

    SDL_Color l_Color = { 0 };

    l_Color.r = 255;
    l_Color.g = (time / 20.f) * 255;
    l_Color.b = (time / 20.f) * 255;
    l_Color.a = 255;

    return l_Color;
}

void GameUIManager_render(GameUIManager* self)
{
    UIObject_render(self->m_canvas);
}
