#pragma once

typedef struct GameSettings
{
    int RabbitCount;
    int FoxCount;
    int MushroomCount;

} GameSettings;

GameSettings* GameSettings_create();

void GameSettings_destroy(GameSettings* gameSettings);
