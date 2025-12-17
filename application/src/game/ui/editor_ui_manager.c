#include "editor_ui_manager.h"
#include "game/scene.h"
#include "game/ui/game_title_page.h"
#include "common/ui_style.h"
#include "game/core/game_core.h"
#include "windows.h"
#include <stdio.h>
#include "game/game_editor.h"
#include "game/game_loader.h"

#pragma warning(disable: 6031);

static void EditorUI_levelButtonClick(void* selectable)
{
    EditorUI* l_Ui = (EditorUI*)UISelectable_getUserData(selectable);
    assert(l_Ui);

    //printf("\nFirst filename: %s\n", l_Ui->FileNames[0]);
    //printf("\nSelected: %s\n", l_Ui->EditingFile);

    int edit = UIList_getSelectedItem(l_Ui->LoadPlayList);

    char* l_Path = calloc(260, sizeof(char));
    assert(l_Path);
    sprintf(l_Path, "../../../levels/%s", l_Ui->FileNames[UISelectable_getUserId(selectable)]);

    printf("Path: %s", l_Path);

    if (l_Ui->Deleting)
    {
        remove(l_Path);
        EditorUI_loadFiles(l_Ui);
        return;
    }

    if (edit == 0)
    {
        l_Ui->EditingFile = calloc(260, sizeof(char));
        assert(l_Ui->EditingFile);
        strcpy(l_Ui->EditingFile, l_Ui->FileNames[UISelectable_getUserId(selectable)]);

        l_Ui->State = EDITING_LEVEL;
        l_Ui->MainManager->m_nextAction = GAME_UI_ACTION_START_EDITOR;

        GameLoader_loadGame(l_Path, false);

        UIList_setSelectedItem(l_Ui->TimeList, (g_gameConfig.Settings->TotalTime - 10) / 25);
        UIList_setSelectedItem(l_Ui->GridSizeList, (g_gameConfig.Settings->GridSize - 5) / 2);
    }
    else
    {
        l_Ui->MainManager->m_nextAction = GAME_UI_ACTION_START;

        GameLoader_loadGame(l_Path, true);

        g_gameConfig.Remaining = g_gameConfig.Settings->TotalTime;
    }
}

static void EditorUI_pageButtonClick(void* selectable)
{
    EditorUI* l_Ui = (EditorUI*)UISelectable_getUserData(selectable);
    assert(l_Ui);

    EPage l_Page = (EPage)UISelectable_getUserId(selectable);

    if (l_Page == PAGE_NEXT)
    {
        //if (l_Ui->CurrentPage)
        l_Ui->CurrentPage += 1;
        EditorUI_loadFiles(l_Ui);
    }
    else
    {
        if (l_Ui->CurrentPage)
            l_Ui->CurrentPage -= 1;
        EditorUI_loadFiles(l_Ui);
    }
}

static void EditorUI_newLevelButtonClick(void* selectable)
{
    EditorUI* self = (EditorUI*)UISelectable_getUserData(selectable);

    g_gameConfig.Settings->RabbitCount = 0;
    g_gameConfig.Settings->MushroomCount = 0;
    g_gameConfig.Settings->FoxCount = 0;
    g_gameConfig.Settings->TotalTime = 60.f;

    GameCore_destroyGame(g_gameConfig.Core);

    char* l_Path = calloc(260, sizeof(char));
    assert(l_Path);

    int fileCount = EditorUI_loadFiles(self);

    if (fileCount == 9)
    {
        printf("\nToo many levels\n");
        return false;
    }

    int index = 0;
    if (fileCount > 0)
    {
        if (self->FileNames[(fileCount - 1) % LEVEL_LIST_MAX_BUTTON_COUNT])
        {
            sscanf(self->FileNames[(fileCount - 1) % LEVEL_LIST_MAX_BUTTON_COUNT], "level%d.txt", &index);
            printf("\n%d\n", index);
        }
        else
        {
            index = fileCount - 1;
        }
    }
    else
    {
        index = fileCount - 1;
    }

    sprintf(l_Path, "../../../levels/level%d.txt", index + 1);

    GameLoader_saveGame(l_Path);

    EditorUI_loadFiles(self);
}

static void EditorUI_goBack(void* selectable)
{
    printf("\nGoing back\n");

    EditorUI* l_Ui = (EditorUI*)UISelectable_getUserData(selectable);

    assert(l_Ui);
    l_Ui->MainManager->m_nextAction = GAME_UI_ACTION_OPEN_TITLE;
}

static void EditorUI_onAddButtonClicked(void* selectable)
{
    EditorUI* l_UI = (EditorUI*)UISelectable_getUserData(selectable);
    assert(l_UI);

    EEditorAction l_Action = (EEditorAction)UISelectable_getUserId(selectable);

    switch (l_Action)
    {
    case EDITOR_ACTION_ADD_RABBIT:
        g_gameEditor.AddingObject = Rabbit_create(NULL, 0, 0);
        break;
    case EDITOR_ACTION_ADD_FOX:
        g_gameEditor.AddingObject = Fox_create(NULL, 0, 0, RABBIT_EAST);
        break;
    case EDITOR_ACTION_ADD_MUSHROOM:
        g_gameEditor.AddingObject = Mushroom_create(NULL, 0, 0);
        break;
    case EDITOR_ACTION_PLAY:
        g_gameConfig.State = g_gameConfig.State == PLAYING ? NONE : PLAYING;
        UIButton_setLabelString(l_UI->ActionsButtons[EDITOR_ACTION_PLAY], g_gameConfig.State == PLAYING ? "Stop" : "Simulate");
        break;
    case EDITOR_ACTION_SAVE:
        char* l_Path = calloc(1024, sizeof(char));
        assert(l_Path);

        sprintf(l_Path, "../../../levels/%s", l_UI->EditingFile);
        GameLoader_saveGame(l_Path);
        break;
    case EDITOR_ACTION_LEAVE:
        g_gameConfig.isEditing = false;
        g_gameConfig.inLevel = false;

        l_UI->MainManager->m_nextAction = GAME_UI_ACTION_OPEN_TITLE;
        break;
    default: break;
    }
}

int EditorUI_loadFiles(EditorUI* self)
{
    WIN32_FIND_DATA fd;
    HANDLE handle;

    char path[1032] = "../../../levels/*.*";

    if (self->FileNames)
        free(self->FileNames);

    self->FileNames = calloc(LEVEL_LIST_MAX_BUTTON_COUNT, sizeof(char*));
    assert(self->FileNames);

    handle = FindFirstFile(path, &fd);
    BOOL l_Res = FindNextFileA(handle, &fd);

    int fileCount = 0;

    for (int x = 0; x < LEVEL_LIST_MAX_BUTTON_COUNT * self->CurrentPage; x++)
    {
        if (l_Res = FindNextFile(handle, &fd))
        {
            fileCount++;
            continue;
        }
        else
        {
            if (self->CurrentPage && x == 0)
            {
                self->CurrentPage -= 1;
                return EditorUI_loadFiles(self);
            }
        }
    }

    for (int x = 0; x < LEVEL_LIST_MAX_BUTTON_COUNT; x++)
    {
        if (l_Res = FindNextFile(handle, &fd))
        {
            fileCount++;
            UIObject_setEnabled(self->ExistingLevelsButtons[x], true);
            UIButton_setLabelString(self->ExistingLevelsButtons[x], fd.cFileName);
            self->FileNames[x] = calloc(260, sizeof(char));
            assert(self->FileNames[x]);
            strcpy(self->FileNames[x], fd.cFileName);
        }
        else
        {
            UIObject_setEnabled(self->ExistingLevelsButtons[x], false);

            if (x == 0)
            {
                if (self->CurrentPage)
                {
                    self->CurrentPage -= 1;
                    return EditorUI_loadFiles(self);
                }
            }
        }
    }

    return fileCount;
}


EditorUI* EditorUI_create(Scene* scene, GameUIManager* titlePage)
{
    EditorUI* l_UI = malloc(sizeof(EditorUI));
    assert(l_UI);

    printf("\nCreating Editor\n");

    l_UI->Scene = scene;
    l_UI->MainManager = titlePage;
    l_UI->FocusManager = UIFocusManager_create();

    //l_UI->MainCanvas = UICanvas_create("editor-canvas");

    l_UI->CurrentPage = 0;

    l_UI->State = SELECTING_LEVEL;
    l_UI->FileNames = NULL;

    l_UI->LevelEditLayout = NULL;
    l_UI->Deleting = false;

    l_UI->LevelListLayout = UIGridLayout_create("level-list-layout", LEVEL_LIST_MAX_BUTTON_COUNT + 6, 1);
    UIGridLayout_setAnchor(l_UI->LevelListLayout, Vec2_anchor_center);
    UIGridLayout_setColumnSizes(l_UI->LevelListLayout, 150.f);
    UIGridLayout_setRowSizes(l_UI->LevelListLayout, 25.f);
    UIGridLayout_setRowSpacings(l_UI->LevelListLayout, 2.f);

    EditorUI* self = l_UI;

    self->ExistingLevelsButtons = calloc(LEVEL_LIST_MAX_BUTTON_COUNT, sizeof(UIButton*));
    assert(self->ExistingLevelsButtons);

    AssetManager* assets = Scene_getAssetManager(scene);

    for (int x = 0; x < LEVEL_LIST_MAX_BUTTON_COUNT; x++)
    {
        char* l_Name = calloc(50, sizeof(char));
        assert(l_Name);
        sprintf(l_Name, "existing-level-button-%d", x);

        UIButton* l_Button = UIButton_create(l_Name, AssetManager_getFont(assets, FONT_NORMAL));
        UIButton_setLabelString(l_Button, l_Name);
        UISelectable_setUserData(l_Button, self);
        UISelectable_setUserId(l_Button, x);
        UIStyle_setDefaultButton(l_Button);
        UIButton_setOnClickCallback(l_Button, EditorUI_levelButtonClick);
        UIFocusManager_addSelectable(self->FocusManager, l_Button);

        //printf("\nButton: %d\n", x);
        UIGridLayout_addObject(self->LevelListLayout, l_Button, x, 0, 1, 1);

        self->ExistingLevelsButtons[x] = l_Button;
    }

    self->PageButtonsLayout = UIGridLayout_create("page-buttons-layout", 1, 2);
    UIGridLayout_setAnchor(self->PageButtonsLayout, Vec2_anchor_center);
    UIGridLayout_setRowSizes(self->PageButtonsLayout, 5.f);
    UIGridLayout_setColumnSizes(self->PageButtonsLayout, 50.f);
    UIGridLayout_setRowSizes(self->PageButtonsLayout, 25.f);

    UIGridLayout_addObject(self->LevelListLayout, self->PageButtonsLayout, LEVEL_LIST_MAX_BUTTON_COUNT, 0, 1, 1);

    self->LevelListNextPage = UIButton_create("level-list-next-page", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->LevelListNextPage, "next");
    UISelectable_setUserData(self->LevelListNextPage, self);
    UISelectable_setUserId(self->LevelListNextPage, PAGE_NEXT);
    UIButton_setOnClickCallback(self->LevelListNextPage, EditorUI_pageButtonClick);

    UIFocusManager_addSelectable(self->FocusManager, self->LevelListNextPage);

    UIGridLayout_addObject(self->PageButtonsLayout, self->LevelListNextPage, 0, 0, 1, 1);

    self->LevelListPreviousPage = UIButton_create("level-list-next-page", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->LevelListPreviousPage, "back");
    UISelectable_setUserData(self->LevelListPreviousPage, self);
    UISelectable_setUserId(self->LevelListPreviousPage, PAGE_PREVIOUS);
    UIButton_setOnClickCallback(self->LevelListPreviousPage, EditorUI_pageButtonClick);

    UIFocusManager_addSelectable(self->FocusManager, self->LevelListPreviousPage);

    UIGridLayout_addObject(self->PageButtonsLayout, self->LevelListPreviousPage, 0, 1, 1, 1);

    self->CreateNewLevelButton = UIButton_create("new-level-button", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->CreateNewLevelButton, "new level");
    UISelectable_setUserData(self->CreateNewLevelButton, self);
    UISelectable_setUserId(self->CreateNewLevelButton, 0);
    UIButton_setOnClickCallback(self->CreateNewLevelButton, EditorUI_newLevelButtonClick);
        
    UIFocusManager_addSelectable(self->FocusManager, self->CreateNewLevelButton);

    UIGridLayout_addObject(self->LevelListLayout, self->CreateNewLevelButton, LEVEL_LIST_MAX_BUTTON_COUNT + 2, 0, 1, 1);

    UIObject_setParent(self->LevelListLayout, titlePage->m_canvas);

    self->LoadPlayList = UIList_create("play-edit-list", AssetManager_getFont(assets, FONT_NORMAL), 2, UI_LIST_CONFIG_CYCLE | UI_LIST_CONFIG_AUTO_NAVIGATION);
    UIList_setLabelString(self->LoadPlayList, "Edit or play");
    UIList_setItemString(self->LoadPlayList, 0, "edit");
    UIList_setItemString(self->LoadPlayList, 1, "play");
    UIList_setSelectedItem(self->LoadPlayList, 1);

    UIFocusManager_addSelectable(self->FocusManager, self->LoadPlayList);

    UIGridLayout_addObject(self->LevelListLayout, self->LoadPlayList, LEVEL_LIST_MAX_BUTTON_COUNT + 3, 0, 1, 1);

    self->OpenDeleteList = UIList_create("open-delete-list", AssetManager_getFont(assets, FONT_NORMAL), 2, UI_LIST_CONFIG_CYCLE | UI_LIST_CONFIG_AUTO_NAVIGATION);
    UIList_setLabelString(self->OpenDeleteList, "");
    UIList_setItemString(self->OpenDeleteList, 0, "open");
    UIList_setItemString(self->OpenDeleteList, 1, "delete");

    UIFocusManager_addSelectable(self->FocusManager, self->OpenDeleteList);

    UIGridLayout_addObject(self->LevelListLayout, self->OpenDeleteList, LEVEL_LIST_MAX_BUTTON_COUNT + 4, 0, 1, 1);

    self->BackButon = UIButton_create("back-button", AssetManager_getFont(assets, FONT_NORMAL));
    UIButton_setLabelString(self->BackButon, "Back");
    UISelectable_setUserData(self->BackButon, self);
    UISelectable_setUserId(self->BackButon, 0);
    UIButton_setOnClickCallback(self->BackButon, EditorUI_goBack);

    UIFocusManager_addSelectable(self->FocusManager, self->BackButon);

    UIGridLayout_addObject(self->LevelListLayout, self->BackButon, LEVEL_LIST_MAX_BUTTON_COUNT + 5, 0, 1, 1);

    EditorUI_loadFiles(self);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    self->LevelEditLayout = UIGridLayout_create("level-edit-layout", 1, 3);
    UIGridLayout_setColumnSizes(self->LevelEditLayout, 250);
    UIGridLayout_setRowSizes(self->LevelEditLayout, 100);
    UIGridLayout_setAnchor(self->LevelEditLayout, Vec2_anchor_center);
    UIObject_setParent(self->LevelEditLayout, titlePage->m_canvas);

    self->ButtonsLayout = UIGridLayout_create("add-buttons-layout", EDITOR_ACTION_COUNT + 2, 1);
    UIGridLayout_setColumnSizes(self->ButtonsLayout, 100.f);
    UIGridLayout_setRowSizes(self->ButtonsLayout, 25.f);
    UIGridLayout_setRowSpacings(self->ButtonsLayout, 5.f);

    UIGridLayout_addObject(self->LevelEditLayout, self->ButtonsLayout, 0, 0, 1, 1);


    self->EditFocusManager = UIFocusManager_create();

    self->TimeList = UIList_create("time-list", AssetManager_getFont(assets, FONT_NORMAL), 10, UI_LIST_CONFIG_CYCLE | UI_LIST_CONFIG_AUTO_NAVIGATION);
    UIList_setLabelString(self->TimeList, "Time:");

    for (int x = 0; x < 10; x++)
    {
        UIList_setItemString(self->TimeList, x, GameUIManager_formatTime(10.f + x * 25));
    }

    UIFocusManager_addSelectable(self->EditFocusManager, self->TimeList);

    UIGridLayout_addObject(self->ButtonsLayout, self->TimeList, 0, 0, 1, 1);


    self->GridSizeList = UIList_create("grid-size-list", AssetManager_getFont(assets, FONT_NORMAL), 15 , UI_LIST_CONFIG_CYCLE | UI_LIST_CONFIG_AUTO_NAVIGATION);
    UIList_setLabelString(self->GridSizeList, "Grid size:");

    for (int x = 0; x < 15; x++)
    {
        /*char l_Val[2] = "0";
        l_Val[0] = 5 + (2 * x) + '0';*/
        UIList_setItemString(self->GridSizeList, x, GameUIManager_twoDigitsToString(5 + (x * 2)));
    }

    UIFocusManager_addSelectable(self->EditFocusManager, self->GridSizeList);

    UIGridLayout_addObject(self->ButtonsLayout, self->GridSizeList, 1, 0, 1, 1);

    self->ActionsButtons = calloc(EDITOR_ACTION_COUNT, sizeof(UIButton*));
    assert(self->ActionsButtons);

    const char* texts[EDITOR_ACTION_COUNT] = {
        "Add rabbit",
        "Add fox",
        "Add mushroom",
        "Simulate",
        "Save",
        "Leave"
    };

    for (int x = 0; x < EDITOR_ACTION_COUNT; x++)
    {
        UIButton* l_Button = UIButton_create("create-rabbit-button", AssetManager_getFont(assets, FONT_NORMAL));
        UIButton_setLabelString(l_Button, texts[x]);
        UISelectable_setUserData(l_Button, self);
        UISelectable_setUserId(l_Button, x);
        UIButton_setOnClickCallback(l_Button, EditorUI_onAddButtonClicked);

        UIGridLayout_addObject(self->ButtonsLayout, l_Button, x + 2, 0, 1, 1);

        UIFocusManager_addSelectable(self->EditFocusManager, l_Button);

        self->ActionsButtons[x] = l_Button;
    }

    return l_UI;
}

void EditorUI_destroy(EditorUI* editorUI)
{
    if (!editorUI) return;

    UIButton_destroy(editorUI->BackButon);
    UIButton_destroy(editorUI->LevelListNextPage);
    UIButton_destroy(editorUI->LevelListPreviousPage);
    UIList_destroy(editorUI->LoadPlayList);
    UIList_destroy(editorUI->OpenDeleteList);
    UIList_destroy(editorUI->TimeList);

    for (int x = 0; x < LEVEL_LIST_MAX_BUTTON_COUNT; x++)
    {
        UIButton_destroy(editorUI->ExistingLevelsButtons[x]);
    }

    for (int x = 0; x < EDITOR_ACTION_COUNT; x++)
    {
        UIButton_destroy(editorUI->ActionsButtons[x]);
    }

    UIGridLayout_destroy(editorUI->PageButtonsLayout);
    UIGridLayout_destroy(editorUI->LevelListLayout);
    UIGridLayout_destroy(editorUI->LevelEditLayout);
    free(editorUI);
}

void EditorUI_update(EditorUI* self, UIInput* input)
{
    if (self->State == SELECTING_LEVEL)
    {
        UIFocusManager_update(self->FocusManager, input);
        self->Deleting = UIList_getSelectedItem(self->OpenDeleteList);
    }
    else
    {
        UIFocusManager_update(self->EditFocusManager, input);
        g_gameConfig.Settings->TotalTime = 10 + (UIList_getSelectedItem(self->TimeList) * 25);
        g_gameConfig.Settings->GridSize = 5 + (UIList_getSelectedItem(self->GridSizeList) * 2);
    }

    for (int x = 0; x < EDITOR_ACTION_COUNT; x++)
    {
        if (x != EDITOR_ACTION_PLAY && x != EDITOR_ACTION_SAVE && x != EDITOR_ACTION_LEAVE)
        {
            UIObject_setEnabled(self->ActionsButtons[x], g_gameConfig.State != PLAYING);
        }
    }

    UIObject_setEnabled(self->LevelListLayout, self->State == SELECTING_LEVEL);
    
    UIObject_setEnabled(self->LevelEditLayout, self->State == EDITING_LEVEL);
}

