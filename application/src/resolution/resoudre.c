#include "resoudre.h"


void resoudre()    /// alors la globalement j'ai fais un peu n'importe quoi c'est que du place holder parceque j'ai pas suffisement lu le basecode et y'a peut etre pas toutes les fonctions déja faites
{
    size_t capacite = 2;  // TODO: modifier cette capacité de con

    GameHashmap* map = HashMap_New(capacite);

    GameHashmapEntry baseState = { NULL, g_gameConfig.Core};

    int iter = 0;

    bool victory = false;

    while (!victory)
    {
        // pour chaque lapin 
        for (int i = 0; i < g_gameConfig.Settings->RabbitCount;i++)
        {
            // pour chacune des directions 
            for (int i = 0; i < 4; i++)
            {
                if (1 /*Rabbit_canMove()*/)
                {
                    // inserer le plateau avec le lapin bougé si il n'est pas déjà
                }
            }
        }

        // pour chaque renard
        for (int i = 0; i < g_gameConfig.Settings->RabbitCount; i++)
        {
            // pour chacune des directions 
            for (int i = 0; i < 4; i++)
            {
                if (1 /*Rabbit_canMove()*/)
                {
                    // inserer le plateau avec le renard bougé si il n'y est pas déjà
                }
            }
        }
        victory = true;
    }











    //GameHashmapEntry* victoire = resoudre_Rec(map, baseState);

    return NULL;
}

GameHashmapEntry* resoudre_Rec(GameHashmap* map, GameHashmapEntry state)
{
    if (1 /*etat de victoire*/)
    {
        // retourne le bordel 
        return NULL;
    }

    /*if (state.currState.Selected->Type == RABBIT ||
        state.currState.Selected->Type == FOX)*/
    {

    }


    GameHashmapEntry prev = state;

    for (int i = 0; i < 5; i++)
    {
        /*verifie tout la colone actuelle*/
        if (1 /*mouvement possible*/)
        {

            // déplace + enregistre
            // appelle la fonction avec l'objet suivant
            return NULL;  //NON
        }
    }
    for (int i = 0; i < 5; i++)
    {
        /*verifie tout la ligne actuelle*/
        if (1 /*mouvement possible*/)
        {
            // déplace + enregistre
            // appelle la fonction avec l'objet suivant
            return NULL;   // NON
        }
    }
}
