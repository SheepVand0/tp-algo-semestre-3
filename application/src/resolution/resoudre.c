#include "resoudre.h"

int climbBack(GameHashmap* map, GameHashmapEntry* solution)
{
    if (!solution) return -1; // -1 car le state de base n compte pas comme un déplacement

    //printGrid(solution->currState);

    EObjectType l_ObjType = NO_OBJECT;
    int l_ObjectIndex = 0;
    if (solution)
    {
        GameCore_hasPulledOutAMoveWithReference(&solution->currState, &solution->prevState, &l_ObjType, &l_ObjectIndex);
        if (l_ObjType != NO_OBJECT)
        {
            g_gameConfig.solveObjectIndex = l_ObjectIndex;
            g_gameConfig.solveNextSol[0] = solution->prevState.Rabbits[l_ObjectIndex];
            g_gameConfig.solveNextSol[1] = solution->currState.Rabbits[l_ObjectIndex];
        }
    }
    solution = Hashmap_searchBackwards(map, solution);
    
    //printf("\n");
    return 1 + climbBack(map, solution);
}

int solve(GameCore* target)
{
    size_t capacite = 4000;

    GameHashmap* map = HashMap_New(capacite);

    GameHashmapEntry baseState = { 0 };
    baseState.currState = *target;

    map = HashMap_Insert(map, baseState);

    //printf("\nState de base :\n");
    //printGrid(map->m_entries[0].currState);
    //printf("\n");


    int iter = 0;
    GameHashmapEntry* solution = NULL;

    bool victory = false;


    while (!victory && iter < map->m_size)
    {
        GameHashmapEntry grid = map->m_entries[iter];

        // pour chaque lapin 
        for (int i = 0; i < RABBIT_COUNT; i++)
        {
            //if (grid.currState.Rabbits[i].Type == NONE) continue;

            // pour chacune des 5 cases axe X
            for (int k = 0; k < 5; k++)
            {
                GameHashmapEntry tmpGrid;
                tmpGrid.prevState = grid.currState;
                tmpGrid.currState = grid.currState;

                if (Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, k, tmpGrid.currState.Rabbits[i].CellY))
                {
                    
                    /*GameHashmapEntry tmpGrid;
                    tmpGrid.prevState = grid.currState;
                    tmpGrid.currState = grid.currState;
                    Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, k, grid.currState.Rabbits[i].CellY);*/


                    if (!Hashmap_Contains(map, tmpGrid))
                    {
                        //printGrid(tmpGrid.currState);
                        //printf("A\n");
                        // inserer le plateau avec le lapin bougé
                        map = HashMap_Insert(map, tmpGrid);
                        //printGrid(tmpGrid.currState);


                        if (GameCore_isWinning(&tmpGrid.currState))
                        {
                            //printf("RESOLUUUUUUUUUU\n");
                            //printGrid(tmpGrid.currState);
                            printf("\n");
                            solution = &map->m_entries[map->m_size - 1];
                            victory = true;
                        }
                    }
                }
            }

            // pour chacune des 5 cases axe Y
            for (int k = 0; k < 5; k++)
            {
                GameHashmapEntry tmpGrid;
                tmpGrid.prevState = grid.currState;
                tmpGrid.currState = grid.currState;

                if (Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, tmpGrid.currState.Rabbits[i].CellX, k))
                {
                    //printf("BABABABBABA\n");
                    //printGrid(tmpGrid.currState);

                    if (!Hashmap_Contains(map, tmpGrid))
                    {
                        //printGrid(tmpGrid.currState);
                        //printf("B\n");
                        // inserer le plateau avec le lapin bougé
                        map = HashMap_Insert(map, tmpGrid);
                        //printGrid(tmpGrid.currState);

                        if (GameCore_isWinning(&tmpGrid.currState))
                        {
                            //printf("RESOLUUUUUUUUUU\n");
                            //printGrid(tmpGrid.currState);
                            solution = &map->m_entries[map->m_size - 1];
                            victory = true;
                        }
                    }
                }
            }
        }
        //// pour chaque renard
        for (int i = RABBIT_COUNT; i < MAX_RABBITS + g_gameConfig.settings->FoxCount; i++)
        {
            if (g_gameConfig.core->Rabbits[i].Type != FOX) continue;

            //if (grid.currState.Rabbits[i].Type == NONE) continue;

            // pour chacune des 5 cases axe X
            if (g_gameConfig.core->Rabbits[i].Direction == RABBIT_EAST || g_gameConfig.core->Rabbits[i].Direction == RABBIT_WEST)
            {
                for (int k = 0; k < 5; k++)
                {
                    GameHashmapEntry tmpGrid;
                    tmpGrid.prevState = grid.currState;
                    tmpGrid.currState = grid.currState;

                    if (Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, k, tmpGrid.currState.Rabbits[i].CellY))
                    {
                       
                        if (!Hashmap_Contains(map, tmpGrid))
                        {
                            // inserer le plateau avec le lapin bougé
                            map = HashMap_Insert(map, tmpGrid);
                        }
                    }
                }
            }
            else
            {
                // pour chacune des 5 cases axe Y
                for (int k = 0; k < 5; k++)
                {
                    GameHashmapEntry tmpGrid;
                    tmpGrid.prevState = grid.currState;
                    tmpGrid.currState = grid.currState;

                    if (Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, tmpGrid.currState.Rabbits[i].CellX, k))
                    {
                        
                        if (!Hashmap_Contains(map, tmpGrid))
                        {
                            // inserer le plateau avec le lapin bougé
                            map = HashMap_Insert(map, tmpGrid);
                        }
                    }
                }
            }
        }

        iter++;

        /*if (iter == 1200000)
        {
            victory = true;
        }*/
    }


    /*printf("iter : %d\n", iter);
    printf("Parcours :\n ");*/
    int reponse = 0;
    reponse = climbBack(map, solution);
    //printf("Resolu en : %d\n", reponse);

    

    return reponse;
    /*printf("Resolu en   %d   coups (gg) \n\n",iter);
    printf("size de la hashmap-> %d\n ", (int)map->m_size);
    for (int i = 0; i < (int)map->m_size; i++)
    {
        printf("Grille %d:\n", i);
        printf("CurrState: \n");
        printGrid(map->m_entries[i].currState);
        printf("PrevState: \n");
        printGrid(map->m_entries[i].prevState);
        printf("\n");
    }*/
}



GameCore* generate()
{
    int NbrCoups = 8;

    int NbrLapins = 3;
    int NbrRenards = 2;
    int NbrMushrooms = 3;

    bool validSolution = false;
    while (!validSolution)
    {
        for (int i = 0; i < NbrLapins; i++)
        {
            /*random X et Y sur des trous */
            while (0 /* ! valid position */)
            {
                /*placer*/
            }
        }

        for (int i = 0; i < NbrMushrooms; i++)
        {
            /* random X et Y */
            while (0 /* ! valid position */)
            {
                /*placer*/
            }
        }

        for (int i = 0; i < NbrRenards; i++)
        {
            /* random X et Y sur colone/ ligne paire */
            while (0 /* ! valid position */)
            {
                /*placer*/
            }
        }


        GameHashmap* map = HashMap_New(2000);

        // Inserer le state de base


        for (int i = 0; i < NbrCoups; i++)
        {
            /* tester chaque coup possible */


        }




    }
}




//GameHashmapEntry* resoudre_Rec(GameHashmap* map, GameHashmapEntry state)
//{
//    if (1 /*etat de victoire*/)
//    {
//        // retourne le bordel 
//        return NULL;
//    }
//
//    /*if (state.currState.Selected->Type == RABBIT ||
//        state.currState.Selected->Type == FOX)*/
//    {
//
//    }
//
//
//    GameHashmapEntry prev = state;
//
//    for (int i = 0; i < 5; i++)
//    {
//        /*verifie tout la colone actuelle*/
//        if (1 /*mouvement possible*/)
//        {
//
//            // déplace + enregistre
//            // appelle la fonction avec l'objet suivant
//            return NULL;  //NON
//        }
//    }
//    for (int i = 0; i < 5; i++)
//    {
//        /*verifie tout la ligne actuelle*/
//        if (1 /*mouvement possible*/)
//        {
//            // déplace + enregistre
//            // appelle la fonction avec l'objet suivant
//            return NULL;   // NON
//        }
//    }
//}
