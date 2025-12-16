#pragma once

typedef struct GameSettings
{
    int RabbitCount;
    int FoxCount;
    int MushroomCount;

    float TotalTime;

    int GridSize;

} GameSettings;

GameSettings* GameSettings_create();

void GameSettings_destroy(GameSettings* gameSettings);
