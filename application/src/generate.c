#include "generate.h"
#include "resolution/hashmap.h"
#include "resolution/resoudre.h"

GameCore g_gameGenerationResult = { 0 };

GameCore* Generate(int rabbCount, int foxCount, int mushroomCount, int moveCount)
{
    GameHashmap* l_Hashmap = HashMap_New(20000);

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
            } while (!Rabbit_canBePlacedByLoc(&l_RandState, valx, valy));

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
            } while (!Rabbit_canBePlacedByLoc(&l_RandState, valx, valy));

            l_RandState.Rabbits[rabbCount + foxCount + x] = *Mushroom_create(&l_RandState, valx, valy);
        }

        g_gameConfig.Core = &l_RandState;
        //moveFound = resoudre();
    }

    g_gameConfig.State = PLAYING;

    g_gameConfig.Settings->RabbitCount = rabbCount;
    g_gameConfig.Settings->FoxCount = foxCount;
    g_gameConfig.Settings->MushroomCount = mushroomCount;
    g_gameConfig.Settings->GridSize = 5;
    g_gameConfig.Settings->TotalTime = 10.f;
    g_gameConfig.Remaining = g_gameConfig.Settings->TotalTime;

    return &l_InitialState;
}

GameCore* GenerateClean(int rabbCount, int moveCount)
{
    Vec2 l_Poses[5] = {
        Vec2_set(0, 0),
        Vec2_set(0, 4),
        Vec2_set(4, 0),
        Vec2_set(4, 4),
        Vec2_set(2, 2)
    };

    GameHashmap* l_Hasmap = HashMap_New(4000);

    GameCore l_Init = { 0 };

    GameCore* l_ResultMap = malloc(sizeof(GameCore));

    for (int x = 0; x < rabbCount; x++)
    {
        int l_Cell;

        do
        {
            l_Cell = rand() % 5;
        } while (!Rabbit_canBePlacedByLoc(&l_Init, l_Poses[l_Cell].x, l_Poses[l_Cell].y));

        l_Init.Rabbits[x] = GameCore_createRabbitSimple(l_Poses[l_Cell].x, l_Poses[l_Cell].y);
    }

    int l_Iter = 0;
    bool l_Generated = false;

    GameHashmapEntry l_FirstEntry = { 0 };
    l_FirstEntry.currState = l_Init;

    g_gameConfig.Settings->GridSize = 5;
    g_gameConfig.Settings->RabbitCount = rabbCount;

    HashMap_Insert(l_Hasmap, l_FirstEntry);

    while (!l_Generated)
    {
        const GameHashmapEntry l_Entry = l_Hasmap->m_entries[l_Iter];
        //Generator_printCurrent(l_Entry.currState);

        int l_Moves = Generator_countMoves(l_Hasmap, &l_Entry);
        //printf("\nc lui ?^\n");

        //printf("\nMoves: %d, moveCount: %d\n", l_Moves, moveCount);

        if (l_Moves > moveCount)
        {
            l_Iter++;
            continue;
        }

        if (l_Moves == moveCount)
        {
            printf("\nAt least entered here\n");

            bool l_AllRabbitOnExit = false;
            for (int x = 0; x < rabbCount; x++)
            {
                if (GameCore_isHole(l_Entry.currState.Rabbits[x].CellX, l_Entry.currState.Rabbits[x].CellY))
                {
                    l_AllRabbitOnExit = true;
                }
            }

            if (!l_AllRabbitOnExit)
            {
                Generator_printCurrent(l_Entry.currState);
                l_Generated = true;
                Generator_copyGame(l_ResultMap, &l_Entry.currState);
                printf("GENERATION DONE !!!");

                g_gameConfig.Settings->FoxCount = GameCore_getObjectCount(&l_Entry.currState, FOX);
                g_gameConfig.Settings->MushroomCount = GameCore_getObjectCount(&l_Entry.currState, FOX);
                break;
            }
        }
        int l_FoxCount = GameCore_getObjectCount(&l_Entry.currState, FOX);
        int l_MushroomCount = GameCore_getObjectCount(&l_Entry.currState, MUSHROOM);

        GameCore l_CurrentGame;
        Generator_copyGame(&l_CurrentGame, &l_Entry.currState);

        for (int x = 0; x < rabbCount; x++)
        {
            Rabbit* l_Rabb = &l_CurrentGame.Rabbits[x];

            for (int px = -4; px < 5; px++)
            {
                for (int py = -4; py < 5; py++)
                {
                    if (Rabbit_canMove(l_Rabb, &l_CurrentGame, l_Rabb->CellX + px, l_Rabb->CellY + py)/* && !GameCore_isHole(l_Rabb->CellX + px, l_Rabb->CellY + py)*/)
                    {
                        const GameHashmapEntry l_Next;
                        Generator_copyGame(&l_Next.currState, &l_CurrentGame);
                        Generator_copyGame(&l_Next.prevState, &l_CurrentGame);

                        Rabbit_move(&l_Next.currState.Rabbits[x], &l_Next.currState, l_Rabb->CellX + px, l_Rabb->CellY + py);

                        if (!alreadyIn(l_Hasmap, l_Next))
                            l_Hasmap = HashMap_Insert(l_Hasmap, l_Next);
                    }
                }
            }

            if (l_FoxCount < MAX_FOXES)
            {
                for (int px = 0; px < 4; px++)
                {

                    int l_ValX = 0;
                    int l_ValY = 0;

                    if (px == 0)
                        l_ValX = 1;

                    if (px == 1)
                        l_ValX = -1;

                    if (px == 2)
                        l_ValY = 1;

                    if (px == 3)
                        l_ValY = -1;

                    for (int dir = 0; dir < 4; dir++)
                    {
                        if (Fox_canBePlacedByLoc(&l_CurrentGame, l_Rabb->CellX + l_ValX, l_Rabb->CellY + l_ValY, dir))
                        {
                            int l_IndexToCreate = MAX_RABBITS + Float_clamp(l_FoxCount, 0, MAX_FOXES);

                            GameHashmapEntry l_Next;
                            Generator_copyGame(&l_Next.prevState, &l_CurrentGame);
                            Generator_copyGame(&l_Next.currState, &l_CurrentGame);

                            l_Next.currState.Rabbits[l_IndexToCreate] = GameCore_createFoxSimple(l_Rabb->CellX + l_ValX, l_Rabb->CellY + l_ValY, dir);

                            if (!alreadyIn(l_Hasmap, l_Next))
                                l_Hasmap = HashMap_Insert(l_Hasmap, l_Next);
                        }
                    }
                }
            }

            if (l_MushroomCount < MAX_MUSHROOMS)
            {
                for (int px = 0; px < 4; px++)
                {
                    int l_ValX = 0;
                    int l_ValY = 0;

                    if (px == 0)
                        l_ValX = 1;

                    if (px == 1)
                        l_ValX = -1;

                    if (px == 2)
                        l_ValY = 1;

                    if (px == 3)
                        l_ValY = -1;

                    if (Rabbit_canBePlacedByLoc(&l_CurrentGame, l_Rabb->CellX + l_ValX, l_Rabb->CellY + l_ValY))
                    {
                        int l_IndexToCreate = MAX_RABBITS + MAX_FOXES + Float_clamp(l_MushroomCount, 0, MAX_MUSHROOMS);

                        GameHashmapEntry l_Next = { 0 };
                        Generator_copyGame(&l_Next.currState, &l_CurrentGame);
                        Generator_copyGame(&l_Next.prevState, &l_CurrentGame);

                        l_Next.currState.Rabbits[l_IndexToCreate] = GameCore_createMushroomSimple(l_Rabb->CellX + l_ValX, l_Rabb->CellY + l_ValY);

                        //printf("\n%d\n", alreadyIn(l_Hasmap, l_Next));

                        if (!alreadyIn(l_Hasmap, l_Next))
                            l_Hasmap = HashMap_Insert(l_Hasmap, l_Next);

                    }
                }
            }

        }

        for (int x = 0; x < MAX_RABBITS + l_FoxCount; x++)
        {
            Rabbit* l_Rabb = &l_CurrentGame.Rabbits[x];
            if (l_Rabb->Type != FOX) continue;

            for (int px = -4; px < 5; px++)
            {
                for (int py = -4; py < 5; py++)
                {
                    if (Rabbit_canMove(l_Rabb, &l_CurrentGame, l_Rabb->CellX + px, l_Rabb->CellY + py))
                    {
                        GameHashmapEntry l_Next;
                        Generator_copyGame(&l_Next.prevState, &l_Entry.currState);
                        Generator_copyGame(&l_Next.currState, &l_Entry.currState);
                        

                        //printf("Before move: %d %d\n", l_Rabb->CellX + px, l_Rabb->CellY + py);
                        //printf("Adress: %p %p\n", &l_Next.prevState.Rabbits[x], &l_Next.currState.Rabbits[x]);

                        Rabbit_move(&l_Next.currState.Rabbits[x], &l_Next.currState, l_Rabb->CellX + px, l_Rabb->CellY + py);

                        //printf("EQUALS???: %d", GameCore_equals(&l_Next.prevState, &l_Next.currState));

                        if (!alreadyIn(l_Hasmap, l_Next))
                            l_Hasmap = HashMap_Insert(l_Hasmap, l_Next);
                    }
                }
            }
        }

        l_Iter++;

        printf("\niter: %d\n", l_Iter);

        if (l_Iter >= HashMap_GetSize(l_Hasmap))
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Generation error", "Cannot generate a map with the current parametes", g_window);
            exit(-1);
            return NULL;
        }
    }

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
    GameHashmapEntry* l_Prev = Generator_searchEntryAdressInHashmap(hashmap, &pattern->prevState);
    bool l_HasMoved = GameCore_hasPulledOutAMove(&pattern->currState, &pattern->prevState);
    Generator_printCurrent(pattern->currState);
    printf("\nWas a move ??: %d\n", l_HasMoved);

    if (l_Prev)
        return l_HasMoved + Generator_countMoves(hashmap, l_Prev);
    return 0;
}

/// <summary>
/// Stolen from leo 
/// </summary>
/// <param name="map"></param>
/// <param name="grid"></param>
/// <returns></returns>
GameHashmapEntry* Generator_searchEntryAdressInHashmap(GameHashmap* map, GameHashmapEntry* grid)
{
    uint64_t idx = Hashmap_hash(grid->prevState, map->m_capacity);

    while ((int)map->m_idMap[idx] >= 0)
    {
        if (GameCore_equals(&map->m_entries[map->m_idMap[idx]].currState, &grid->prevState)) return &map->m_entries[map->m_idMap[idx]];
        idx = (idx + 1) % (int)map->m_capacity;
    }
    return NULL;
}

