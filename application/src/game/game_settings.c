#include "game/game_settings.h"
#include "settings.h"

GameSettings* GameSettings_create()
{
    GameSettings* l_Settings = malloc(sizeof(GameSettings));
    assert(l_Settings);

    l_Settings->RabbitCount = 4;
    l_Settings->FoxCount = 1;
    l_Settings->MushroomCount = 0;

    return l_Settings;
}

void GameSettings_destroy(GameSettings* gameSettings)
{
    free(gameSettings);
}
