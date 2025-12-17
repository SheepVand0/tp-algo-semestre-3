#include "game_loader.h"
#include "settings.h"
#include "game/core/game_core.h"
#include "game/game_config.h"
#include "game/game_graphics.h"
#include "game/game_editor.h"

#pragma warning(disable: 6031)

void GameLoader_loadGame(char* fileName, bool forPlay)
{
    FILE* l_File = fopen(fileName, "r+");
    assert(l_File);

    GameCore_destroyGame(g_gameConfig.Core);

    int gridSize;
    int foxCount;
    int rabbitCount;
    int mushCount;
    float time;

    int res = fscanf(l_File, "%d %d %d %d %f", &gridSize, &rabbitCount, &foxCount, &mushCount, &time);

    g_gameConfig.Settings->RabbitCount = rabbitCount;
    g_gameConfig.Settings->FoxCount = foxCount;
    g_gameConfig.Settings->MushroomCount = mushCount;
    g_gameConfig.Settings->TotalTime = time;
    g_gameConfig.Settings->GridSize = gridSize;

    int indexR = 0;
    int indexF = forPlay ? RABBIT_COUNT : MAX_RABBITS;
    int indexM = (forPlay ? RABBIT_COUNT : MAX_RABBITS) + (forPlay ? FOX_COUNT : MAX_FOXES);

    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            //printf("\n%d %d", x, y);

            int obj = 0;
            int att = 0;
            fscanf(l_File, "%d %d", &obj, &att);

            if (obj == RABBIT)
            {
                g_gameConfig.Core->Rabbits[indexR] = *Rabbit_create(g_gameConfig.Core, x, y);

                //printf("%d\n", g_gameConfig.Core->Rabbits[indexR].CellX);
                indexR += 1;
            }
            else if (obj == FOX)
            {
                g_gameConfig.Core->Rabbits[indexF] = *Fox_create(g_gameConfig.Core, x, y, att);
                indexF += 1;
            }
            else if (obj == MUSHROOM)
            {
                g_gameConfig.Core->Rabbits[indexM] = *Mushroom_create(g_gameConfig.Core, x, y);
                indexM += 1;
            }
        }
    }

    fclose(l_File);
}

void GameLoader_saveGame(char* fileName)
{
    //fileName = "../../../levels/test_level_save.txt";
    FILE* l_File = fopen(fileName, "w+");
    //printf("uwu %s\n", l_File);

    assert(l_File);

    fprintf(l_File, "%d %d %d %d %f\n", GAME_GRID_SIZE, RABBIT_COUNT, FOX_COUNT, MUSHROOM_COUNT, g_gameConfig.Settings->TotalTime);

    Rabbit* l_Usable = GameEditor_buildUsableArray(g_gameConfig.Core->Rabbits);

    for (int y = 0; y < GAME_GRID_SIZE; y++)
    {
        for (int x = 0; x < GAME_GRID_SIZE; x++)
        {
            bool l_f = false;

            for (int i = 0; i < RABBIT_COUNT + FOX_COUNT + MUSHROOM_COUNT; i++)
            {
                //printf("%d\n", i);
                if (x == l_Usable[i].CellX && y == l_Usable[i].CellY)
                {
                    //printf("Type: %d\n", g_gameConfig.Core->Rabbits[i].Type);
                    fprintf(l_File, "%d %d ", l_Usable[i].Type, l_Usable[i].Direction);
                    l_f = true;
                    break;
                }
            }

            if (!l_f)
            {
                fprintf(l_File, "%d %d ", 0, 0);
            }
        }

        fprintf(l_File, "\n");
    }

    fclose(l_File);
}
