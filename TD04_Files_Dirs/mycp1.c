/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * d'après une correction d'Erick Gallesio (ESSI)
 */

/*!
 * \file
 *
 * Copie de fichiers : première version (simplifiée)
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
#include <sys/resource.h>

// Pour avoir PATH_MAX (on pourrait aussi utiliser pathconf() pour ne pas être
// lié à Linux)
#include <linux/limits.h>

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

//! Erreur d'usage
static void usage()
{
    fprintf(stderr, "Usage:\tmycp1 [-b buffsize] f1 f2\n"
                    "\tmycp1 [-b buffsize] f1 ... fn d2\n");
    exit(1);
}

//! Copie d'un fichier ordinaire dans un autre 
static int nbchars = 0;

//! Copie d'un fichier dans un autre fichier
void copy(const char *src, const char *dest)
{
    int fdsrc;
    int fddest;
    int n;
 
    // Ouvrir src en lecture seule 
    if ((fdsrc = open(src, O_RDONLY)) < 0)
    {
        perror("mycp1 : fichier origine");
        fprintf(stderr, "\tImpossible d'ouvrir fichier origine %s\n", src);
        exit(1);
    }

    // Ouvrir dest en écriture, le créer s'il n'existe par, le tronquer à 0
    // s'il existe déjà.
    if ((fddest = open(dest, O_WRONLY|O_CREAT|O_TRUNC, USER_PERM)) < 0)
    {
        perror("mycp1 : fichier destination");
        fprintf(stderr, "\tImpossible d'ouvrir/créer fichier destination %s\n", dest);
        exit(1);
    }

    // Les fichiers sont ouverts; copier
    while ((n = read(fdsrc, buffer, buffer_size)) > 0) 
    {
        if (n < 0)
        {
            perror("mycp1 : lecture");
            fprintf(stderr, "\tErreur de lecture fichier %s\n", src);
            exit(1);
        }
        if (write(fddest, buffer, n) < n)
        {
            perror("mycp1 : ÉCRITURE");
            fprintf(stderr, "\tErreur d'écriture fichier %s\n", dest);
            exit(1);
        }
        nbchars += n;
    }
  
    // Fermeture des fichiers
    close(fdsrc); 
    close(fddest);
}

//! Copie de plusieurs fichiers dans (possiblement) un répertoire
void copy_files(const char *src, const char *dest)
{
    char newfile[PATH_MAX];

    // Dans cette version, on ne copie pas les répertoires
    if (is_dir(src))
    {
        fprintf(stderr, "mycp1: %s est un répertoire (non copié)\n", src);
        exit(1);
    }

    // Si la destination est un répertoire, on crée un nouveau fichier ou on
    // écrase un fichier déjà existant dans ce répertoire 
    if (is_dir(dest))
    {
        snprintf(newfile, PATH_MAX + 1, "%s/%s", dest, get_basename(src));
        dest = newfile;
    }

    // On vérifie qu'on ne copie pas un fichier sur lui-même (ceci peut se
    // produire à cause des *liens* */
    if (same_file(src, dest))
    {
        fprintf(stderr, "mycp1: %s et %s sont identiques (non copié)\n", src, dest);
        exit(1);
    }

    // Copie 
    copy(src, dest);
}

//! Convertir une struct timeval en un nombre réel de secondes 
#define TIMEVAL(tv) ((double)(tv).tv_sec + 1.0e-06 * (double)(tv).tv_usec)

/* Imprime quelques statistiques de performances */
static void print_rusage(const struct rusage *ru0, const struct rusage *ru1)
{
    double delta_t;

    delta_t = TIMEVAL(ru1->ru_utime) - TIMEVAL(ru0->ru_utime);
    printf("Temps utilisateur : %.3f : %.2f kcar/s\n",
           delta_t, (double)nbchars / delta_t / (1024 * 1024));

    delta_t = TIMEVAL(ru1->ru_stime) - TIMEVAL(ru0->ru_stime);
    printf("Temps système : %.3f  : %.2f kcar/s\n",
           delta_t, (double)nbchars / delta_t / (1024 * 1024));
}

/*! Programme principal
 *
 * Invocation : cp [-b buffsize] filesrc filedest
 *              cp [-b buffsize] filesrc1 [filesrc2 ...]  filedest
 *
 * filedest peut être un répertoire.
 */
int main(int argc, char *argv[])
{
    int ifiles = 1;	//!< index du début des arguments correspondant aux fichiers à copier
    char *dest;		//!< destination de la copie 
    int n; 		//!< nombre de fichiers à copier 
    int i;
    struct rusage ru0;	//!< utilisation des ressources système 
    struct rusage ru1;	//!< utilisation des ressources système 

    // Décodage de l'argument '-b buffsize' éventuel 
    if (argv[1][0] == '-') 
    {
        if (argv[1][1] == 'b' && argv[1][2] == '\0' && argc >= 2)
        {
            buffer_size = atoi(argv[2]);
            if (buffer_size <= 0) usage();
            ifiles = 3; 
            option_b = true;
        }
        else usage();
    }

    // Recherche de la destination (le dernier paramètre)
    dest = argv[--argc];

    // Nombre de fichiers à copier
    n = argc - ifiles;
    
    // On vérifie qu'il y a des fichiers à copier 
    if (n <= 0)
        usage();

    // Si il y a plus de 1 fichier à copier, la destination doit être un répertoire
    if (n > 1 && !is_dir(dest))
        usage();

    // Allocation du buffer de copie 
    buffer = malloc(buffer_size);

    //Exécution de la copie 
    getrusage(RUSAGE_SELF, &ru0);
    for (i = ifiles; i < argc; ++i)
        copy_files(argv[i], dest);
    getrusage(RUSAGE_SELF, &ru1);    
    if (option_b)
        print_rusage(&ru0, &ru1);

    return 0;
}
