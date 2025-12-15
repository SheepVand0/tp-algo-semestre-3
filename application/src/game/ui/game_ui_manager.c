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

static void GameUIManager_closePages(GameUIManager* self);

GameUIManager* GameUIManager_create(Scene* scene)
{
    GameUIManager* self = (GameUIManager*)calloc(1, sizeof(GameUIManager));
    AssertNew(self);

    self->m_scene = scene;

    UICanvas* canvas = UICanvas_create("canvas");
    self->m_canvas = canvas;
    self->m_titlePage = GameTitlePage_create(scene, self);
    self->m_settingsPage = NULL;

    AssetManager* assets = Scene_getAssetManager(scene);

    self->m_timeTextLayout = UIGridLayout_create("time-text-layout", 2, 1);
    UIObject_setParent(self->m_timeTextLayout, canvas);
    UIGridLayout_setRowSizes(self->m_timeTextLayout, 25.0f);
    UIGridLayout_setRowSpacings(self->m_timeTextLayout, 5.f);
    UIGridLayout_setRowSpacing(self->m_timeTextLayout, 0, 20.f);
    UIGridLayout_setAnchor(self->m_timeTextLayout, Vec2_anchor_north);

    self->m_timeText = UILabel_create("time-text", AssetManager_getFont(assets, FONT_NORMAL));
    UILabel_setTextString(self->m_timeText, "bite");
    UILabel_setAnchor(self->m_timeText, Vec2_anchor_north);
    UILabel_setColor(self->m_timeText, g_colors.orange4);

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

    UIGridLayout_addObject(self->m_timeTextLayout, self->m_timeText, 0, 0, 1, 1);
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

    self->m_titlePage = NULL;
    self->m_settingsPage = NULL;
}

void GameUIManager_update(GameUIManager* self, UIInput* input)
{
    UIObject_setEnabled(self->m_timeTextLayout, self->m_scene->Core->State == PLAYING);
    UIObject_setEnabled(self->m_lostText, self->m_scene->Core->State == GETTING_LARRIED);
    //UIObject_setEnabled(self->m_timeText, self->m_scene->Core->State == PLAYING);

    if (self->m_scene->Core)
    {
        printf("%s\n", GameUIManager_formatTime(self->m_scene->Core->Remaining));
        UILabel_setTextString(self->m_timeText, GameUIManager_formatTime(self->m_scene->Core->Remaining));
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
            self->m_titlePage = GameTitlePage_create(scene, self);
            break;

        case GAME_UI_ACTION_START:
            GameUIManager_closePages(self);
            g_gameConfig.inLevel = true;
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

void GameUIManager_render(GameUIManager* self)
{
    UIObject_render(self->m_canvas);
}
