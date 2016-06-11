/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * d'après une correction d'Erick Gallesio (ESSI)
 * $Id: mycp2.c,v 1.2 2008/04/21 18:08:09 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Copie de fichiers : deuxième version (options -v, -r, -i)
 * ------------------------------------------------------------------------------------
 */

/*!
 * \file
 *
 * Copie de fichiers :  deuxième version (options -v, -r, -i)
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

// Pour avoir PATH_MAX (on pourrait aussi utiliser pathconf() pour ne pas être
// lié à Linux)
//#include <linux/limits.h>
#define PATH_MAX 1024

#include "util.h"

//! Buffer d'E/S 
#define DEFBUFFSIZE 1024		//!< Taille par défaut du buffer de copie 
static char *buffer;			//!< Buffer de copie 
size_t buffer_size = DEFBUFFSIZE;	//!< Taille effective du buffer de copie 

//! Spécification de la taille de buffer
bool option_b = false;			

//! Permissions par défaut des fichiers créés : rw pour tous
// (elles seront en plus filtrées par le umask)
#define USER_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

//! Permissions par défaut des répertoires créés
#define DIR_PERM (USER_PERM|S_IXUSR|S_IXGRP|S_IXOTH)

// Options de la ligne de commande
bool option_v = false; 	//!< copie verbeuse
bool option_r = false;	//!< copie récursive
bool option_i = false;	//!< copie avec demande de confirmation d'écrasement dee fichier 

bool multiple_files = true; //!< plusieurs fichiers à copier

//! Erreur d'usage
static void usage()
{
    fprintf(stderr, "Usage:\tmycp2 [options] f1 f2\n"
                    "\tmycp2 [options] f1 ... fn d2\n");
    fprintf(stderr, "\noù les options sont :\n"
                    "\t-b buffsize\ttaille du buffer d'E/S\n"
                    "\t-v\tcopie verbeuse\n"
                    "\t-i\tdemande de confirmation si destination existe\n"
                    "\t-r\tcopie récursive des répertoires\n");
    exit(1);
}

//! Copie d'un fichier dans un autre fichier
void copy(char *src, char *dest)
{
    int fdsrc;
    int fddest;
    int n;
 
    // Ouvrir src en lecture seule
    if ((fdsrc = open(src, O_RDONLY)) < 0)
    {
        perror("mycp2 : fichier origine");
        fprintf(stderr, "\tImpossible d'ouvrir fichier origine %s\n", src);
        exit(1);
    }

    // Ouvrir dest en écriture, le créer s'il n'existe par, le tronquer à 0
    // s'il existe déjà.
    if ((fddest = open(dest, O_WRONLY|O_CREAT|O_TRUNC, USER_PERM)) < 0)
    {
        perror("mycp2 : fichier destination");
        fprintf(stderr, "\tImpossible d'ouvrir/créer fichier destination %s\n", dest);
        exit(1);
    }

    if (option_v)
        printf("mycp2 : copie de %s sur %s\n", src, dest);

    // Les fichiers sont ouverts; copier
    while ((n = read(fdsrc, buffer, buffer_size)) > 0) 
    {
        if (n < 0) 
        {
            perror("mycp2 : lecture");
            fprintf(stderr, "\tErreur de lecture fichier %s\n", src);
            exit(1);
        }
        if (write(fddest, buffer, n) < n) 
        {
            perror("mycp2 : ÉCRITURE");
            fprintf(stderr, "\tErreur d'écriture fichier %s\n", dest);
            exit(1);
        }
    }
  
    // Fermeture des fichiers
    close(fdsrc); 
    close(fddest);
}

//! Copie de plusieurs fichiers dans (possiblement) un répertoire
void copy_files(char *src, char *dest)
{
    char newsrc[PATH_MAX];
    char newdest[PATH_MAX];
    DIR *dirp;
    struct dirent *dentry;

    if (is_dir(src))
    {
        // Il faut l'option -r pour déclencher la copie récursive
        if (!option_r)
        {
            fprintf(stderr, "mycp2: %s est un répertoire (non copié)\n", src);
            exit(1);
        }
        else if (same_file(src, dest))
        {
            fprintf(stderr, "mycp2: les répertoires %s et %s sont identiques (non copié)\n", src, dest);
            exit(1);
        }

        // La destination doit être un répertoire 
        if (file_exists(dest))
            snprintf(newdest, PATH_MAX + 1, "%s/%s", dest, get_basename(src));
        else
            strcpy(newdest, dest);
        mkdir(newdest, DIR_PERM);	/* Inutile de tester; il y aura une
                                         * erreur plus tard si le répertoire
                                         * n'a pas pu être créé */

        /* Copie récursive */
        if ((dirp = opendir(src)) == NULL)
        {
            fprintf(stderr, "mycp2 : impossible ouvrir répertoire %s\n", src);
            exit(1);
        }

        for (dentry = readdir(dirp); dentry != NULL; dentry = readdir(dirp))
        {
            if (is_dot_dir(dentry->d_name))
                continue;
            snprintf(newsrc, PATH_MAX + 1, "%s/%s", src, dentry->d_name);
            copy_files(newsrc, newdest);
        }

        closedir(dirp);
 
    }
    else 
    {
        // Copie d'un fichier ordinaire 

        // Si la destination n'existe pas et qu'on a plusieurs fichiers à
        // copier, il faut créer un répertoire
        if (multiple_files && !file_exists(dest))
            mkdir(dest, DIR_PERM);

        // Si la destination est un répertoire, on crée un nouveau fichier ou on
        // écrase un fichier déjà existant dans ce répertoire 
        if (is_dir(dest)) 
        {
            snprintf(newdest, PATH_MAX + 1, "%s/%s", dest, get_basename(src));
            dest = newdest;
        }

        // Si la destination existe et si on est interactif (-i) demander
        // confirmation 
        if (option_i && file_exists(dest)) 
        {   
            int rep;

            fprintf(stderr, "mycp2 : destination %s existe, l'écraser ? ([yYoOnN]) ", dest); 
            fflush(stderr);
            rep = getchar();
            while (getchar() != '\n') {} // oublie la fin de ligne 
            if (rep != 'y' && rep != 'Y' && rep != 'o' && rep != 'O') return;
        }

        // On vérifie qu'on ne copie pas un fichier sur lui-même (ceci peut se
        // produire à cause des *liens* 
        if (same_file(src, dest))
        {
            fprintf(stderr, "mycp2: %s et %s sont identiques (non copié)\n", src, dest);
            exit(1);
        }

        /* Copie */
        copy(src, dest);
    }
}

/*! Programme principal
 *
 * Invocation : cp [options] filesrc filedest
 *              cp [options] filesrc1 [filesrc2 ...]  filedest
 *
 * filedest peut être un répertoire.
 */
int main(int argc, char *argv[])
{
    int ifiles = 1;	//!< index du début des arguments correspondant aux fichiers à copier
    char *dest;		//!< destination de la copie
    int n; 		//!< nombre de fichiers à copier
    int i;

    // Décodage des options 
    for (i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-') 
        {
            if (argv[i][2] != '\0')
                usage();

            switch (argv[i][1])
            {
            case 'b':
                if (++i >= argc) 
                    usage();
                buffer_size = atoi(argv[i]);
                if (buffer_size <= 0) 
                    usage();
                ifiles += 2; 	
                break;
            case 'v':
                option_v = true;
                ++ifiles;
                break;
            case 'r':
                option_r = true;
                ++ifiles;
                break;
            case 'i':
                option_i = true;
                ++ifiles;
                break;
            default:
                usage();
                break;                
            }
        }
        else break; // début de la liste des fichiers à copier 
    }

    // Recherche de la destination (le dernier paramètre) 
    dest = argv[--argc];

    // Nombre de fichiers à copier 
    n = argc - ifiles;
    
    // On vérifie qu'il y a des fichiers à copier
    if (n <= 0) 
        usage();

    /* */
    if (n > 1) 
    {
        if (!option_r) 
            usage();
        multiple_files = true;
    }

    // Allocation du buffer de copie
    buffer = malloc(buffer_size);

    // Exécution de la copie 

    for (i = ifiles; i < argc; ++i)
        copy_files(argv[i], dest);

    return 0;
}
