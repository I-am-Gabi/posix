/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * d'après une correction d'Erick Gallesio (ESSI)
 * $Id: shell-system.c,v 1.2 2008/11/21 07:44:50 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Simulation de la boucle principale du shell, avec gestion (partielle)  de signaux 
 *
 * La gestion des signaux dans la fonction my_system() est TRÈS délicate si on
 * veut éviter une désynchronisation de l'affichage du prompt. On lira avec
 * attention les commentaires.
 * ------------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>
#include <setjmp.h>

/* Déclaration en avant */
int my_system(const char *);
void on_int(int sig);

/* Exécution directe des commandes built-in */
int exec_builtin(char *line);

/* Affichage du prompt */
void prompt();

/* Buffer pour la ligne de commande : LINE_MAX devrait être défini sous POSIX */
#ifndef LINE_MAX
#define LINE_MAX 2048
#endif
char line[LINE_MAX];

/* Sauvegarde du contexte de pile */
jmp_buf env;

int main()
{
    struct sigaction sigact;
    sigset_t sigmask;

    /* Mise en place du piège pour SIGINT 
    *
    * NOTE : ce piège n'est mis en place que pour capture SIGINT en dehors de
    * l'exécution d'une commande (voir my_system() plus bas)
    */
    sigemptyset(&sigmask);
    sigact.sa_handler = on_int;
    sigact.sa_mask = sigmask;
    sigaction(SIGINT, &sigact, NULL);

    /* Boucle du shell 
     * ----------------
     */

    /* On positionne un point de reprise ici
     *
     * NOTE : même remarque que ci-dessus
     */
    sigsetjmp(env, 0);

    prompt();
    while (fgets(line, LINE_MAX, stdin) != NULL)
    {
         /* On enlève le \n et on vérifie que la ligne n'est pas vide
         *
         * En fait il faudrait faire des vérifications plus fines, par exemple
         * tester que la ligne lue contient bien une commande. Ceci est laissé
         * à titre d'exercice.
         */
        line[strlen(line) - 1] = '\0'; 
        if (line[0] == '\0') 
        {
            prompt();
            continue;
        }

        if (!exec_builtin(line)) my_system(line);
        prompt();
    }

    /* Fin du programme */
    exit(0);
}

/* La fonction system() elle-même
 *
 * ATTENTION : il s'agit d'une version *rustique* qui ne gère pas correctement
 * les signaux (voir le chapitre consacré aux signaux dans le cours)
 */
int my_system(const char *cmd)
{
    char *shell;
    int status;
    struct sigaction sigact;
    struct sigaction old_sigact;
    sigset_t sigmask;
    sigset_t sigint_mask;

    /* Récupère le shell de l'utilisateur : le shell par défaut est "sh" */   
    shell = getenv("SHELL");
    if (shell == 0 || strcmp(shell, "") == 0) shell = "sh";

    /* SYNCHRO : ATTENTION : problème délicat !
     * ----------------------------------------
     *
     * On ignore le signal SIGINT pendant que le père attend la terminaison de
     * son file, car on ne peut pas interrompre wait() (plus bas dans cette
     * fonction).

     * En effet, si on laisse le piège et si le père était interrompu pendant
     * le wait() de son fils, le siglongjmp() l'enverrait en tête de boucle
     * principale, sans attendre; Le fils deviendrait alors ZOMBIE (inactif mais non
     * encore récupéré par son père).  Donc au prochain passage dans cette
     * fonction pour exécuter une nouvelle commande, le wait() ci-dessous
     * récupérerait le zombie (sans blocage) au lieu d'attendre la fin du
     * nouveau fils créé. On continuerait alors normalement la boucle
     * principale, affichant le prompt au milieu de la sortie de la nouvelle
     * commande. On constaterait donc une "désynchronisation" de l'affichage du
     * prompt.
     *
     * En ignorant SIGINT ici, le père attend quoiqu'il arrive la fin de son
     * fils (que ce soit une interruption normale ou sur signal). Cependant, la
     * mise en place de l'état d'ignorance doit être faite pour le père seul ! 
     * Le fils ne doit pas hériter de cet etat ignoré. Une fois le fils
     * terminé, on doit rétablir l'ancien piège.
     *
     * En outre, il y a une fenêtre (étroite, mais la loi de Murphy veille !) 
     * pendant laquelle le père peut continuer à capturer SIGINT (et donc à
     * faire le siglongjmp()), alors que le fils existe déjà. C'est pourquoi,
     * ici, nous bloquons le signal SIGINT dans le père tant que ce dernier n'a
     * pu effectivement ignorer ce signal. Il faut aussi, bien entendu,
     * débloquer ce signal avant l'execXX() du fils, sinon le fils hériterait
     * du blocage.
     *
     * Les instructions correspondant au traitements décrits ci-dessus sont
     * commentées par le mot SYNCHRO.
     * 
     * Tout ceci vous donne un avant goût des "charmes" de la programmation
     * concurrente, et de la notion de "section critique" (cours ESSI 2 pour
     * II).
     */

    /* SYNCHRO : On bloque SIGINT */
    sigemptyset(&sigint_mask);
    sigaddset(&sigint_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &sigint_mask, NULL);

    /* Exécution de la commande */
    switch (fork())
    {
    case -1: /* Erreur */
	perror("fork");
	abort();

    case 0: /* Processus fils qui exécute la commande */
        
        /* SYNCHRO : On doit débloquer SIGINT car le fils a hérité du fait
         * que ce signal était bloqué !
         */ 
        sigprocmask(SIG_UNBLOCK, &sigint_mask, NULL);

        /* On exécute tranquillement la commande */
	execlp(shell, shell, "-c", cmd, NULL);
        perror("exit ls");	/* On ne doit pas passer ici */
	exit(1); 		/* Fin du fils : surtout pas return ! */

    default: /* Continuation du père */
	break;	   
    }

    /* SYNCHRO : Le père ignore SIGINT pour pouvoir attendre tranquillement. Au
     * passage on mémorise le piège précédemment installé.
     */
    sigemptyset(&sigmask);
    sigact.sa_handler = SIG_IGN;
    sigact.sa_mask = sigmask;
    sigaction(SIGINT, &sigact, &old_sigact);  

    /* SYNCHRO : On peut débloquer SIGINT sans problème, puisqu'il sera
     * maintenant ignoré.
     */ 
    sigprocmask(SIG_UNBLOCK, &sigint_mask, NULL);

    /* Le père attend et récupère le code de retour */
    wait(&status); 

    /* SYNCHRO : Quand le fils est terminé, on rétablit l'ancien piège */
    sigaction(SIGINT, &old_sigact, NULL);

    /* On retourne le code renvoyé par le shell */
    return status;
}

/* Exécution directe des commandes built-in */

int exec_builtin(char *line)
{
    char *cmd;
    char line1[LINE_MAX];

    /* ATTENTION ! strtok() modifie la chaine ! Il vaut mieux faire une copie */
    strcpy(line1, line);

    /* Recherche du nom de la commande */
    cmd = strtok(line1, " ");
    
    /* ici, on n'a une seule commande built-in, cd */
    if (strcmp(cmd, "cd") == 0)
    {
        char *dir = strtok(NULL, " ");
        if (chdir(dir) < 0) perror("chdir");
        if (strtok(NULL, " ") != NULL)
            fprintf(stderr, "Commande cd : trop d'arguments (ignorés)\n");   

        return 1;
    }    

    /* Ici, ce n'était pas une commande builtin */
    return 0;
}

/* Affichage du prompt */
void prompt()
{
    printf("$ ");
    fflush(stdout);
} 

/* Piège du signal SIGINT */
void on_int(int sig)
{
    if (sig == SIGINT) 
    { 
        putchar('\n');
        /* On retourne au début de la boucle principale */
        siglongjmp(env, 1);  
    }
    else fprintf(stderr, "Reçu signal non attendu : %d\n", sig); 
}
