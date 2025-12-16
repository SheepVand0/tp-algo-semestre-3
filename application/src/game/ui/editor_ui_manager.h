#pragma once

#include "settings.h"

typedef struct Scene Scene;
typedef struct GameTitlePage GameTitlePage;
typedef struct Rabbit Rabbit;

#define LEVEL_LIST_MAX_BUTTON_COUNT 5

typedef enum EPage
{
    PAGE_NEXT = 0,
    PAGE_PREVIOUS = 1
};

typedef enum EEditorState
{
    SELECTING_LEVEL,
    EDITING_LEVEL
};

typedef struct EditorUI
{
    Scene* Scene;
    GameTitlePage* TitlePage;

    UICanvas* MainCanvas;

    UIGridLayout* LevelListLayout;
    UIButton** ExistingLevelsButtons;

    UIGridLayout* PageButtonsLayout;

    UIButton* LevelListNextPage;
    UIButton* LevelListPreviousPage;

    UIButton* CreateNewLevelButton;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    UIGridLayout* LevelEditLayout;
    UIGridLayout* ButtonsLayout;

    UIButton* RabbitButton;
    UIButton* FoxButton;
    UIButton* MushroomButton;

    Rabbit* PlacingTemp;

    char* EditingFile;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    int CurrentPage;
} EditorUI;

EditorUI* EditorUI_create(Scene* scene, GameTitlePage* titlePage);

void EditorUI_destroy(EditorUI* editorUI);
