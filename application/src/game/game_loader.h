#pragma once

#include "settings.h"

typedef struct GameCore GameCore;

void GameLoader_loadGame(char* fileName, bool forPlay);

void GameLoader_saveGame(char* fileName);
