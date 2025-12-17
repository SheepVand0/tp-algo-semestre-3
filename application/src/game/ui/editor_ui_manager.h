#pragma once

#include "settings.h"

typedef struct Scene Scene;
typedef struct GameTitlePage GameTitlePage;
typedef struct Rabbit Rabbit;
typedef struct GameUIManager GameUIManager;

#define LEVEL_LIST_MAX_BUTTON_COUNT 5

typedef enum EPage
{
    PAGE_NEXT = 0,
    PAGE_PREVIOUS = 1
} EPage;

typedef enum EEditorState
{
    SELECTING_LEVEL,
    EDITING_LEVEL
} EEditorState;

typedef enum EEditorAction
{
    EDITOR_ACTION_ADD_RABBIT,
    EDITOR_ACTION_ADD_FOX,
    EDITOR_ACTION_ADD_MUSHROOM,
    EDITOR_ACTION_PLAY,
    EDITOR_ACTION_SAVE,
    EDITOR_ACTION_LEAVE,
    EDITOR_ACTION_COUNT
} EEditorAction;

typedef struct EditorUI
{
    Scene* Scene;
    GameUIManager* MainManager;
    UIFocusManager* FocusManager;
    UIFocusManager* EditFocusManager;

    UICanvas* MainCanvas;
    UIGridLayout* LevelListLayout;
    UIButton** ExistingLevelsButtons;

    UIButton* BackButon;

    UIGridLayout* PageButtonsLayout;

    UIButton* LevelListNextPage;
    UIButton* LevelListPreviousPage;

    UIButton* CreateNewLevelButton;

    UILabel* PageText;

    UIList* LoadPlayList;

    UIList* OpenDeleteList;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    UIGridLayout* LevelEditLayout;
    UIGridLayout* ButtonsLayout;

    UIList* TimeList;

    UIList* GridSizeList;

    UIButton** ActionsButtons;

    Rabbit* PlacingTemp;

    char* EditingFile;

    char** FileNames;

    EEditorState State;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    int CurrentPage;

    bool Deleting;
} EditorUI;

EditorUI* EditorUI_create(Scene* scene, GameUIManager* titlePage);

void EditorUI_destroy(EditorUI* editorUI);

void EditorUI_update(EditorUI* self, UIInput* input);

int EditorUI_loadFiles(EditorUI* self);
