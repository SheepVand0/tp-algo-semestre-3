#include "generate.h"
#include "resolution/hashmap.h"
#include "resolution/resoudre.h"
#include "game/game_editor.h"

GameCore g_gameGenerationResult = { 0 };

GameCore* Generate(int rabbCount, int foxCount, int mushroomCount, int moveCount)
{
    GameHashmap* l_Hashmap = HashMap_New(5000);


    GameCore* l_InitialState = malloc(sizeof(GameCore));

    int moveFound = 0;

    GameCore l_RandState = { 0 };
    while (moveFound != moveCount)
    {
        for (int x = 0; x < rabbCount; x++)
        {
            int valx = 0;
            int valy = 0;

            do
            {
                valx = rand() % 5;
                valy = rand() % 5;
            } while (!Rabbit_canBePlacedByLoc(&l_RandState, valx, valy, NULL));

            l_RandState.Rabbits[x] = *Rabbit_create(&l_RandState, valx, valy);
        }

        for (int x = 0; x < foxCount; x++)
        {
            int valx = 0;
            int valy = 0;
            ERabbitDirection l_Dir = 0;

            do
            {
                valx = rand() % 5;
                valy = rand() % 5;
                l_Dir = rand() % 4;


            } while (!Fox_canBePlacedByLoc(&l_RandState, valx, valy, l_Dir));

            l_RandState.Rabbits[rabbCount + x] = *Fox_create(&l_RandState, valx, valy, l_Dir);
        }

        for (int x = 0; x < mushroomCount; x++)
        {
            int valx = 0;
            int valy = 0;

            do
            {
                valx = rand() % 5;
                valy = rand() % 5;
            } while (!Rabbit_canBePlacedByLoc(&l_RandState, valx, valy, NULL));

            l_RandState.Rabbits[rabbCount + foxCount + x] = *Mushroom_create(&l_RandState, valx, valy);
        }

        //g_gameConfig.Core = &l_RandState;
        moveFound = solve(&l_RandState);
    }

    g_gameConfig.state = PLAYING;

    g_gameConfig.settings->RabbitCount = rabbCount;
    g_gameConfig.settings->FoxCount = foxCount;
    g_gameConfig.settings->MushroomCount = mushroomCount;
    g_gameConfig.settings->GridSize = 5;
    g_gameConfig.settings->TotalTime = 10.f;
    g_gameConfig.remaining = g_gameConfig.settings->TotalTime;

    return l_InitialState;
}

GameCore* GenerateClean(int rabbCount, int maxFoxes, int maxMushrooms, int moveCount)
{
    /*Vec2 l_Poses[5] = {
        Vec2_set(0, 0),
        Vec2_set(0, 4),
        Vec2_set(4, 0),
        Vec2_set(4, 4),
        Vec2_set(2, 2)
    };*/

    Vec2* l_Poses = calloc(5, sizeof(Vec2));
    l_Poses[0] = Vec2_set(0, 0);
    l_Poses[1] = Vec2_set(0, 4);
    l_Poses[2] = Vec2_set(4, 0);
    l_Poses[3] = Vec2_set(4, 4);
    l_Poses[4] = Vec2_set(2, 2);

    GameHashmap* l_Hasmap = HashMap_New(5000);

    GameHashmap* l_ResultHashmap = HashMap_New(20);

    GameCore* l_ResultMap = malloc(sizeof(GameCore));

    GameHashmapEntry l_FirstEntry = { 0 };

    for (int x = 0; x < rabbCount; x++)
    {
        int l_Cell = 0;

        do
        {
            l_Cell = rand() % 5;
        } while (!Rabbit_canBePlacedByLoc(&l_FirstEntry.currState, l_Poses[l_Cell].x, l_Poses[l_Cell].y, NULL));

        l_FirstEntry.currState.Rabbits[x] = GameCore_createRabbitSimple(l_Poses[l_Cell].x, l_Poses[l_Cell].y);
    }

    int l_Iter = 0;
    bool l_Generated = false;

    g_gameConfig.settings->GridSize = 5;
    g_gameConfig.settings->RabbitCount = rabbCount;

    HashMap_InsertWithMushrooms(l_Hasmap, l_FirstEntry);

    while (!l_Generated && l_Iter < HashMap_GetSize(l_Hasmap))
    {
        const GameHashmapEntry l_Entry = l_Hasmap->m_entries[l_Iter];

        if (l_Iter > 10000 || HashMap_GetSize(l_ResultHashmap) > 7)
        {
            if (HashMap_GetSize(l_ResultHashmap))
            {
                Generator_copyGame(l_ResultMap, &(l_ResultHashmap->m_entries[rand() % HashMap_GetSize(l_ResultHashmap)]).currState);
                g_gameConfig.settings->FoxCount = GameCore_getObjectCount(l_ResultMap, FOX);
                g_gameConfig.settings->MushroomCount = GameCore_getObjectCount(l_ResultMap, MUSHROOM);

                /*char* l_Result = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
                sprintf(l_Result, "%p", &l_Entry.currState);

                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "title", l_Result, g_window);

                sprintf(l_Result, "%p", &l_Entry.prevState);

                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "title", l_Result, g_window);*/

                //Generator_countMoves(l_Hasmap, &l_Entry);

                HashMap_destroy(l_ResultHashmap);
                HashMap_destroy(l_Hasmap);
                return l_ResultMap;
            }

            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", "error while generating, no solution under 10000 iterations", g_window);
            exit(-1);
            return NULL;
        }

        //Generator_printCurrent(l_Entry.currState);

        g_gameConfig.settings->FoxCount = GameCore_getObjectCount(&l_Entry.currState, FOX);
        g_gameConfig.settings->MushroomCount = GameCore_getObjectCount(&l_Entry.currState, MUSHROOM);

        //printf("\nITER: %d\n", l_Iter);
        //printf("\nSIZE: %dn", HashMap_GetSize(l_Hasmap));
        //int l_Moves = Generator_countMoves(l_Hasmap, &l_Entry);
        

        int l_Moves = moveCount;
        /*if (l_Moves > moveCount)
        {
            l_Iter++;
            continue;
        }*/

        if (l_Moves >= moveCount && l_Moves <= moveCount + 5)
        {
            //printf("\nAt least entered here\n");

            Rabbit* l_Generated = GameEditor_buildUsableArray(l_Entry.currState.Rabbits);

            g_gameConfig.settings->GridSize = 5;

            int l_MoveCountRes = solve(l_Generated);
            /*if (l_MoveCountRes < moveCount || l_MoveCountRes > moveCount + 5)
            {
                l_Iter++;
                continue;
            };
*/
            if (l_MoveCountRes >= Float_clamp(moveCount - 3, 2, 100000) && l_MoveCountRes <= moveCount + 5)
            {
                bool l_AllRabbitOnExit = true;
                for (int x = 0; x < rabbCount; x++)
                {
                    if (!GameCore_isHole(l_Entry.currState.Rabbits[x].CellX, l_Entry.currState.Rabbits[x].CellY))
                    {
                        l_AllRabbitOnExit = false;
                    }
                }
                Generator_printCurrent(l_Entry.currState);

                if (!l_AllRabbitOnExit)
                {
                    //Generator_printCurrent(l_Entry.currState);
                    l_Generated = true;
                    Generator_copyGame(l_ResultMap, &l_Entry.currState);
                    printf("GENERATION DONE !!!");

                    if (!alreadyIn(l_ResultHashmap, l_Entry))
                    {
                        l_ResultHashmap = HashMap_InsertWithMushrooms(l_ResultHashmap, l_Entry);
                    }

                    l_Iter++;
                    continue;
                }
            }

            
        }
        int l_FoxCount = GameCore_getObjectCount(&l_Entry.currState, FOX);
        int l_MushroomCount = GameCore_getObjectCount(&l_Entry.currState, MUSHROOM);

        Generator_copyGame(&l_Entry.currState, &l_Entry.currState);

        for (int x = 0; x < rabbCount; x++)
        {
            Rabbit* l_Rabb = &l_Entry.currState.Rabbits[x];

            for (int a = 0; a < 2; a++)
            {
                for (int ay = -5; ay < 5; ay++)
                {
                    int px = a == 0 ? ay : 0;
                    int py = a == 1 ? ay : 0;

                    if (py == Float_clamp(py, -1, 1) && px == Float_clamp(px, -1, 1)) continue;

                    if (py != 0 && px != 0) continue;

                    if (Rabbit_canMove(l_Rabb, &l_Entry.currState, l_Rabb->CellX + px, l_Rabb->CellY + py)/* && !GameCore_isHole(l_Rabb->CellX + px, l_Rabb->CellY + py)*/)
                    {
                        const GameHashmapEntry l_aNext;
                        Generator_copyGame(&l_aNext.currState, &l_Entry.currState);
                        Generator_copyGame(&l_aNext.prevState, &l_Entry.currState);

                        if (Rabbit_move(&l_aNext.currState.Rabbits[x], &l_aNext.currState, l_Rabb->CellX + px, l_Rabb->CellY + py))
                        {
                            if (!alreadyIn(l_Hasmap, l_aNext))
                            {
                                //l_HasPlaced = true;
                                l_Hasmap = HashMap_InsertWithMushrooms(l_Hasmap, l_aNext);
                            }
                        }
                    }
                    else
                    {
                        for (int b = 0; b < MAX_RABBITS + MAX_FOXES; b++)
                        {
                            Rabbit l_Temp = l_Entry.currState.Rabbits[b];

                            if (l_Temp.Type == RABBIT || l_Temp.Type == FOX)
                            {
                                if (Rabbit_canMove(&l_Rabb, &l_Entry.currState, l_Temp.CellX + px, l_Temp.CellY + py))
                                {
                                    const GameHashmapEntry l_bNext;
                                    Generator_copyGame(&l_bNext.currState, &l_Entry.currState);
                                    Generator_copyGame(&l_bNext.prevState, &l_Entry.currState);

                                    Rabbit_move(&(&l_bNext.currState)->Rabbits[b], &l_bNext.currState, l_Rabb->CellX + px, l_Rabb->CellY + py);

                                    if (!alreadyIn(l_Hasmap, l_bNext))
                                    {
                                        l_Hasmap = HashMap_InsertWithMushrooms(l_Hasmap, l_bNext);
                                    }
                                }
                            }

                        }
                        if (l_FoxCount < maxFoxes)
                        {
                            for (int dir = 0; dir < 4; dir++)
                            {
                                if (Fox_canBePlacedByLoc(&l_Entry.currState, l_Rabb->CellX + (px - sign(px)), l_Rabb->CellY + (py - sign(py)), dir))
                                {
                                    GameHashmapEntry l_dNext;
                                    Generator_copyGame(&l_dNext.prevState, &l_Entry.currState);
                                    Generator_copyGame(&l_dNext.currState, &l_Entry.currState);

                                    l_dNext.currState.Rabbits[MAX_RABBITS + l_FoxCount] = GameCore_createFoxSimple(l_Rabb->CellX + (px - sign(px)), l_Rabb->CellY + (py - sign(py)), dir);

                                    if (!alreadyIn(l_Hasmap, l_dNext))
                                    {
                                        //l_HasPlaced = true;
                                        l_Hasmap = HashMap_InsertWithMushrooms(l_Hasmap, l_dNext);
                                    }
                                }
                            }
                        }
                    }

                    if (l_MushroomCount < maxMushrooms)
                    {
                        if (Rabbit_canBePlacedByLoc(&l_Entry.currState, px, py, NULL) && GameCore_isCellNextSmth)
                        {
                            GameHashmapEntry l_cNext;
                            Generator_copyGame(&l_cNext.prevState, &l_Entry.currState);
                            Generator_copyGame(&l_cNext.currState, &l_Entry.currState);
                            (&l_cNext.currState)->Rabbits[MAX_RABBITS + MAX_FOXES + l_MushroomCount] = GameCore_createMushroomSimple(l_Rabb->CellX + (px - sign(px)), l_Rabb->CellY + (py - sign(py)));
                            if (!alreadyIn(l_Hasmap, l_cNext))
                            {
                                //l_HasPlaced = true;
                                l_Hasmap = HashMap_InsertWithMushrooms(l_Hasmap, l_cNext);
                            }
                        }
                    }

                }
            }
        }

        l_Iter++;

        if (l_Iter >= HashMap_GetSize(l_Hasmap))
        {
            //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Generation error", "Cannot generate a map with the current parametes", g_window);
            free(l_ResultMap);
            free(l_Poses);
            HashMap_destroy(l_Hasmap);
            HashMap_destroy(l_ResultHashmap);
            return GenerateClean(rabbCount, maxFoxes, maxMushrooms, moveCount);
        }
    }

    HashMap_destroy(l_Hasmap);

    return l_ResultMap;
}

void Generator_printCurrent(GameCore core)
{
    char l_Result[30] = "/////\n/////\n/////\n/////\n/////";

    for (int x = 0; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
    {
        Rabbit l_Rabb = core.Rabbits[x];

        if (l_Rabb.Type == RABBIT)
            l_Result[l_Rabb.CellY * 6 + l_Rabb.CellX] = 'R';


        if (l_Rabb.Type == FOX)
        {
            Vec2 l_Other = Fox_getSecondCell(&l_Rabb);
            l_Result[l_Rabb.CellY * 6 + l_Rabb.CellX] = 'F';
            int l_SecondIndex = l_Other.y * 6 + l_Other.x;
            l_Result[l_SecondIndex] = 'F';
        }


        if (l_Rabb.Type == MUSHROOM)
            l_Result[l_Rabb.CellY * 6 + l_Rabb.CellX] = 'M';
    }

    printf("\n%s\n", l_Result);
}

void Generator_copyGame(GameCore* target, GameCore* original)
{
    for (int x = 0; x < MAX_RABBITS + MAX_FOXES + MAX_MUSHROOMS; x++)
    {
        target->Rabbits[x].Type = original->Rabbits[x].Type;
        target->Rabbits[x].CellX = original->Rabbits[x].CellX;
        target->Rabbits[x].CellY = original->Rabbits[x].CellY;
        target->Rabbits[x].Direction = original->Rabbits[x].Direction;
        target->Rabbits[x].RabbitSprite = original->Rabbits[x].RabbitSprite;
        target->Rabbits[x].Movable = original->Rabbits[x].Movable;
    }
}

/// <summary>
/// Semi-stolen from leo
/// </summary>
/// <param name="hashmap"></param>
/// <param name="pattern"></param>
/// <returns></returns>
int Generator_countMoves(GameHashmap* hashmap, GameHashmapEntry* pattern)
{
    if (!pattern) return 0;
    if (!hashmap) return 0;

    GameHashmapEntry* l_Prev = Generator_searchEntryAdressInHashmap(hashmap, &pattern->prevState);
    //printf("\n%p\n", l_Prev);
    bool l_HasMoved = GameCore_hasPulledOutAMove(&pattern->currState, &pattern->prevState);

    /*Generator_printCurrent(l_Prev->prevState);
    Generator_printCurrent(l_Prev->currState);*/

    if (l_Prev)
        return l_HasMoved + Generator_countMoves(hashmap, l_Prev);

    /*if (l_Prev)
    {
        if (GameCore_equals(&l_Prev->currState, &pattern->prevState))
        {
            return 0;
        }
    }*/
    return 0;
}

int sign(int obj)
{
    if (obj == 0) return 0;
    return obj < 0 ? -1 : 1;
}

/// <summary>
/// Stolen from leo 
/// </summary>
/// <param name="map"></param>
/// <param name="grid"></param>
/// <returns></returns>
GameHashmapEntry* Generator_searchEntryAdressInHashmap(GameHashmap* map, GameHashmapEntry* grid)
{
    if (!grid) return NULL;

    if (grid->currState.Rabbits[0].Type == NO_OBJECT) return NULL;

    uint64_t idx = Hashmap_hash(grid->prevState, map->m_capacity);

    while ((int)map->m_idMap[idx] >= 0)
    {
        if (GameCore_equals(&map->m_entries[map->m_idMap[idx]].currState, &grid->prevState)) return &map->m_entries[map->m_idMap[idx]];
        idx = (idx + 1) % map->m_capacity;
    }
    return NULL;
}

