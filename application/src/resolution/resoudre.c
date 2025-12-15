#include "resoudre.h"

void* resoudre()    /// alors la globalement j'ai fais un peu n'importe quoi c'est que du place holder parceque j'ai pas suffisement lu le basecode et y'a peut etre pas toutes les fonctions déja faites
{
    int nbrcases;
    int nbrLapins;
    int nbrRenards;
    int capacite = 1;

    for (int i = 0; i < (nbrLapins + nbrRenards); i++)
    {
        capacite *= nbrcases;
    }
    GameHashmap* map = HashMap_New(capacite);

    int key = 0;

    // HashMap_Insert(/*grille de base*/);

    resoudre_Rec(/*grille de base */);

    return NULL;
}

void resoudre_Rec(/*grille , objet*/)
{
    if (1 /*etat de victoire*/)
    {
        // retourne le bordel
        return NULL;
    }

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
