#include "game/game_settings.h"
#include "settings.h"

GameSettings* GameSettings_create()
{
    GameSettings* l_Settings = malloc(sizeof(GameSettings));
    assert(l_Settings);

    l_Settings->RabbitCount = 8;
    l_Settings->FoxCount = 2;
    l_Settings->MushroomCount = 2;

    return l_Settings;
}

void GameSettings_destroy(GameSettings* gameSettings)
{
    free(gameSettings);
}
