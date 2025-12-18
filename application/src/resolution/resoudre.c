#include "resoudre.h"

int remonter(GameHashmap* map, GameHashmapEntry* solution)
{
    if (!solution) return -1; // -1 car le state de base n compte pas comme un déplacement

    printGrid(solution->currState);
    solution = rechercheInv(map, solution);
    printf("\n");
    return 1 + remonter(map, solution);
}

void resoudre()    /// alors la globalement j'ai fais un peu n'importe quoi c'est que du place holder parceque j'ai pas suffisement lu le basecode et y'a peut etre pas toutes les fonctions déja faites
{
    size_t capacite = 2000;  // TODO: modifier cette capacité de con

    GameHashmap* map = HashMap_New(capacite);


    GameHashmapEntry baseState = { 0 };
    baseState.currState = *g_gameConfig.Core;

    map = HashMap_Insert(map, baseState);

    printf("\nState de base :\n");
    printGrid(map->m_entries[0].currState);
    printf("\n");


    int iter = 0;
    GameHashmapEntry* solution = NULL;

    bool victory = false;


    while (!victory)
    {
        if (iter >= map->m_size)
        {
            printf("NON");
            victory = true;
        }
        GameHashmapEntry grid = map->m_entries[iter];

        // pour chaque lapin 
        for (int i = 0; i < RABBIT_COUNT; i++)
        {
            //if (grid.currState.Rabbits[i].Type == NONE) continue;

            // pour chacune des 5 cases axe X
            for (int k = 0; k < 5; k++)
            {
                if (Rabbit_canMove(&grid.currState.Rabbits[i], &grid.currState, k, grid.currState.Rabbits[i].CellY))
                {
                    GameHashmapEntry tmpGrid;
                    tmpGrid.prevState = grid.currState;
                    tmpGrid.currState = grid.currState;
                    Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, k, grid.currState.Rabbits[i].CellY);


                    if (!alreadyIn(map, tmpGrid))
                    {
                        //printGrid(tmpGrid.currState);
                        printf("\n");
                        // inserer le plateau avec le lapin bougé
                        map = HashMap_Insert(map, tmpGrid);
                        printGrid(tmpGrid.currState);


                        if (GameCore_isWinning(&tmpGrid.currState))
                        {
                            printf("RESOLUUUUUUUUUU\n");
                            printGrid(tmpGrid.currState);
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
                if (Rabbit_canMove(&grid.currState.Rabbits[i], &grid.currState, grid.currState.Rabbits[i].CellX, k))
                {
                    GameHashmapEntry tmpGrid;
                    tmpGrid.prevState = grid.currState;
                    tmpGrid.currState = grid.currState;
                    Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, grid.currState.Rabbits[i].CellX, k);



                    if (!alreadyIn(map, tmpGrid))
                    {
                        //printGrid(tmpGrid.currState);
                        printf("\n");
                        // inserer le plateau avec le lapin bougé
                        map = HashMap_Insert(map, tmpGrid);
                        printGrid(tmpGrid.currState);

                        if (GameCore_isWinning(&tmpGrid.currState))
                        {
                            printf("RESOLUUUUUUUUUU\n");
                            printGrid(tmpGrid.currState);
                            solution = &map->m_entries[map->m_size - 1];
                            victory = true;
                        }
                    }
                }
            }
        }
        //// pour chaque renard
        for (int i = RABBIT_COUNT; i < g_gameConfig.Settings->RabbitCount + g_gameConfig.Settings->FoxCount; i++)
        {
            //if (grid.currState.Rabbits[i].Type == NONE) continue;

            // pour chacune des 5 cases axe X
            if (g_gameConfig.Core->Rabbits[i].Direction == RABBIT_EAST || g_gameConfig.Core->Rabbits[i].Direction == RABBIT_WEST)
            {
                printf("Horizontal\n");
                for (int k = 0; k < 5; k++)
                {
                    if (Rabbit_canMove(&grid.currState.Rabbits[i], &grid.currState, k, grid.currState.Rabbits[i].CellY))
                    {
                        GameHashmapEntry tmpGrid;
                        tmpGrid.prevState = grid.currState;
                        tmpGrid.currState = grid.currState;
                        Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, k, tmpGrid.currState.Rabbits[i].CellY);


                        if (!alreadyIn(map, tmpGrid))
                        {
                            // inserer le plateau avec le lapin bougé
                            map = HashMap_Insert(map, tmpGrid);
                            printGrid(tmpGrid.currState);
                        }
                    }
                }

            }
            else
            {
                printf("Vertical\n");
                // pour chacune des 5 cases axe Y
                for (int k = 0; k < 5; k++)
                {
                    if (Rabbit_canMove(&grid.currState.Rabbits[i], &grid.currState, grid.currState.Rabbits[i].CellX, k))
                    {
                        GameHashmapEntry tmpGrid;
                        tmpGrid.prevState = grid.currState;
                        tmpGrid.currState = grid.currState;
                        Rabbit_move(&tmpGrid.currState.Rabbits[i], &tmpGrid.currState, tmpGrid.currState.Rabbits[i].CellX, k);


                        if (!alreadyIn(map, tmpGrid))
                        {
                            // inserer le plateau avec le lapin bougé
                            map = HashMap_Insert(map, tmpGrid);
                            printGrid(tmpGrid.currState);
                        }
                    }
                }
            }
        }

        iter++;

        if (iter == 1200)
        {
            victory = true;
        }
    }

    printf("Parcours :\n ");
    int reponse = 0;
    reponse = remonter(map, solution);
    printf("Resolu en : %d\n", reponse);

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

    return;
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
