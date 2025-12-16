#include "editor_ui_manager.h"
#include "game/scene.h"
#include "game/ui/game_title_page.h"
#include "common/ui_style.h"
#include "game/core/game_core.h"

static void EditorUI_levelButtonClick(void* selectable)
{

}

static void EditorUI_pageButtonClick(void* selectable)
{

}

static void EditorUI_newLevelButtonClick(void* selectable)
{

}

EditorUI* EditorUI_create(Scene* scene, GameTitlePage* titlePage)
{
    EditorUI* l_UI = malloc(sizeof(EditorUI));
    assert(l_UI);

    l_UI->Scene = scene;
    l_UI->TitlePage = titlePage;

    l_UI->MainCanvas = UICanvas_create("editor-canvas");

    l_UI->LevelListLayout = UIGridLayout_create("level-list-layout", LEVEL_LIST_MAX_BUTTON_COUNT + 2, 1);
    UIGridLayout_setAnchor(l_UI->LevelListLayout, Vec2_anchor_center);
    UIGridLayout_setColumnSizes(l_UI->LevelListLayout, 25.f);
    UIGridLayout_setRowSizes(l_UI->LevelListLayout, 5.f);

    EditorUI* self = l_UI;

    self->ExistingLevelsButtons = calloc(LEVEL_LIST_MAX_BUTTON_COUNT, sizeof(UIButton*));
    assert(self->ExistingLevelsButtons);

    AssetManager* assets = Scene_getAssetManager(scene);

    for (int x = 0; x < LEVEL_LIST_MAX_BUTTON_COUNT; x++)
    {
        char* l_Name = calloc(50, sizeof(char));
        sprintf(l_Name, "existing-level-button-%d", x);

        UIButton* l_Button = UIButton_create(l_Name, AssetManager_getFont(assets, FONT_NORMAL));
        UIButton_setLabelString(l_Button, l_Name);
        UISelectable_setUserData(l_Button, self);
        UISelectable_setUserId(l_Button, x);
        UIStyle_setDefaultButton(l_Button);
        UIButton_setOnClickCallback(l_Button, EditorUI_levelButtonClick);

        UIGridLayout_addObject(self->LevelListLayout, l_Button, x, 0, 1, 1);
    }

    self->PageButtonsLayout = UIGridLayout_create("page-buttons-layout", 1, 2);
    UIGridLayout_setAnchor(self->PageButtonsLayout, Vec2_anchor_center);
    UIGridLayout_setColumnSizes(self->PageButtonsLayout, 25.f);
    UIGridLayout_setRowSizes(self->PageButtonsLayout, 5.f);

    UIGridLayout_addObject(self->LevelListLayout, self->PageButtonsLayout, LEVEL_LIST_MAX_BUTTON_COUNT, 0, 1, 1);

    self->LevelListNextPage = UIButton_create("level-list-next-page", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->LevelListNextPage, "next");
    UISelectable_setUserData(self->LevelListNextPage, self);
    UISelectable_setUserId(self->LevelListNextPage, PAGE_NEXT);
    UIButton_setOnClickCallback(self->LevelListNextPage, EditorUI_pageButtonClick);

    UIGridLayout_addObject(self->PageButtonsLayout, self->LevelListNextPage, 0, 0, 1, 1);

    self->LevelListPreviousPage = UIButton_create("level-list-next-page", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->LevelListPreviousPage, "back");
    UISelectable_setUserData(self->LevelListPreviousPage, self);
    UISelectable_setUserId(self->LevelListPreviousPage, PAGE_PREVIOUS);
    UIButton_setOnClickCallback(self->LevelListPreviousPage, EditorUI_pageButtonClick);


    UIGridLayout_addObject(self->PageButtonsLayout, self->LevelListPreviousPage, 0, 1, 1, 1);

    self->CreateNewLevelButton = UIButton_create("new-level-button", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->CreateNewLevelButton, "new level");
    UISelectable_setUserData(self->CreateNewLevelButton, self);
    UISelectable_setUserId(self->CreateNewLevelButton, 0);

    UIGridLayout_addObject(self->LevelListLayout, self->CreateNewLevelButton, LEVEL_LIST_MAX_BUTTON_COUNT + 2, 0, 1, 1);

    return l_UI;
}

void EditorUI_destroy(EditorUI* editorUI)
{

}
