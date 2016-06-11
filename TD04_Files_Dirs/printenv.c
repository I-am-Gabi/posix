/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: printenv.c,v 1.3 2008/04/21 18:13:30 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Affichage des variables d'environnement (i.e., réécriture de printenv)
 * ------------------------------------------------------------------------------------
 */
/*!
 * \file
 *
 * Affichage des variables d'environnement.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

//! Le tableau de chaines de caractères contenant les variables d'environnement
extern char **environ; 

// Fontions definies plus loin 
static void print_all();
static void print_var(int argc, char *argv[]);

/*! Programme principal
 *
 * Invocation : printenv.exe [variable ...]
 */
int main(int argc, char *argv[])
{
    // S'il n'y a aucun argument, on écrit la totalité de \c environ, sinon on
    // ecrit les valeurs des variables demandées
    if (argc == 1)
        print_all();
    else
        print_var(argc, argv);
            
    return 0;
}

//! Lister tout le contenu de environ
void print_all()
{
    char **env;

    for (env = environ; *env != NULL; ++env)
        printf("%s\n", *env);
}

/*! Lister les valeurs des variables indiquées en argument 
 *
 * On suppose ici que la taille du tableau environ est plus grande que la
 * longueur de la liste d'arguments. Donc on parcourt une seule fois
 * l'environnement et on conserve dans un tableau de pointeurs (\c locs)
 * l'adresse des chaines qu'il faudra afficher.
 */
void print_var(int argc, char *argv[])
{
    char *locs[argc];
    for (int i = 0; i < argc; ++i)
        locs[i] = NULL;

    for (char **env = environ; *env != NULL; ++env)
    {
        char *pequal = strchr(*env, '='); // recherche du signe '="
        assert(pequal != 0);		  // on doit avoir trouvé '=' !
        int len = pequal - *env;	  // longueur du nom de variable 
        
        // On regarde si la ligne courante correspond à l'une des variables que
        //l'on recherche. La comparaison s'effectue sur les len premiers caractères.
        for (int i = 1; i < argc; ++i) 
        {
            if (strncmp(*env, argv[i], len) == 0 && argv[i][len] == '\0') 
                locs[i] = pequal + 1;
        } 
    }

    // Impression du résultat : cet impression est différée pour être rendue
    // dans le même ordre que argv. 
    for (int i = 1; i < argc; ++i) 
    {
        if (locs[i] != NULL) 
            printf("%s\n", locs[i]);
        else
            printf("\n");
    }
}
