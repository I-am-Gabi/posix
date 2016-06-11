/* ------------------------------------------------------------------------------------
 * Un exemple d'implémentation du cache d'un fichier afin d'explorer
 * l'effet des algorithmes de gestion et de remplacement
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: load_strategy.c,v 1.2 2008/04/14 15:15:49 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Interface avec une bibliothèque de stratégie chargée dynamiquement
 * ------------------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>

#include "strategy.h"
#include "load_strategy.h"

/* Variable globale pour sauvegarder le pointeur sur la bibliothèque de
 * stratégie courante
 */
static void *plib_save = NULL;

/* Affichage d'un message d'erreur */

static void Load_Error(const char *msg, const char *strat )
{
    if (msg == NULL) 
        fprintf(stderr, "Chargement dynamique de la stratégie %s : erreur inconnue\n", strat);
    else
        fprintf(stderr, "Chargement dynamique de la stratégie %s : %s\n", strat, msg);

    exit(2);
}

/* ------------------------------------------------------------------------------------
 * Fonction de chargement et déchargemeent de la stratégie
 * ------------------------------------------------------------------------------------
*/

int Load_Strategy(const char *strategy_name)
{
    int i;
    void *plib = 0;
    char libname[NLIBNAME];
    char *errmsg = NULL;

    /* Constitution du nom de la bibliothèque */
    strcpy(libname, libname_prefix);
    strcat(libname, strategy_name);
    strcat(libname, libname_suffix);

    /* Ouverture de la bibliothèque */
    plib = dlopen(libname, RTLD_LAZY);
    if (plib == NULL || (errmsg = dlerror()) != NULL) Load_Error(errmsg, strategy_name);

    /* Résolution des symboles */
    for (i = 0; i < N_STRATEGY_FUNCTIONS; ++i)
    {
        ANY_PTR_FUNC fptr;

        errmsg = NULL; /* raz deu message d'erreur à cause du test qui suit */
        fptr = (ANY_PTR_FUNC)dlsym(plib, strategy_functions[i].fname);
        if (fptr == NULL || (errmsg = dlerror()) != NULL) Load_Error(errmsg, strategy_name);
        else strategy_functions[i].fptr = fptr;        
    }
    
    /* Tout s'est bien passé */
    plib_save = plib;
    return 1;
}

void Unload_Strategy()
{
    if (plib_save != NULL) dlclose(plib_save);
    plib_save = NULL;
}

/* -----------------------------------------------------------------------------------
 * Fonctions de relay avec la stratégie
 * ------------------------------------------------------------------------------------
*/

/* Creation et initialisation de la stratégie (invoqué par la création de cache) */
void *Strategy_Create(struct Cache *pcache)
{
    return STRATEGY_FUNC(STRATEGY_CREATE_TYPE, STRATEGY_CREATE)(pcache);
}

/* Fermeture de la stratégie */
void Strategy_Close(struct Cache *pcache)
{
    STRATEGY_FUNC(STRATEGY_CLOSE_INVALIDATE_TYPE, STRATEGY_CLOSE)(pcache);
}

/* Fonction "réflexe" lors de l'invalidation du cache */
void Strategy_Invalidate(struct Cache *pcache)
{
    STRATEGY_FUNC(STRATEGY_CLOSE_INVALIDATE_TYPE, STRATEGY_INVALIDATE)(pcache);
}

/* Algorithme de remplacement de bloc */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{
    return STRATEGY_FUNC(STRATEGY_REPLACE_TYPE, STRATEGY_REPLACE)(pcache);
}

/* Fonctions "réflexes" lors de la lecture et de l'écriture */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb)
{
    STRATEGY_FUNC(STRATEGY_READ_WRITE_TYPE, STRATEGY_READ)(pcache, pb);
}

void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb)
{
    STRATEGY_FUNC(STRATEGY_READ_WRITE_TYPE, STRATEGY_WRITE)(pcache, pb);
}

/* Identification de la stratégie */
char *Strategy_Name()
{
    return STRATEGY_FUNC(STRATEGY_NAME_TYPE, STRATEGY_NAME)();
}
