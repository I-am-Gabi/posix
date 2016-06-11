/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: station.c,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * RÃ©seau en anneau Ã  jeton (token ring) : une station de l'anneau
 * ---------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>

#include "station.h"

/* Variables globales
 * ------------------
 */
static FILE *fp_in;	 	/* Pour lecture au terminal */
static FILE *fp_out;	 	/* Pour Ã©criture au terminal */
static int this_station;	/* NumÃ©ro de cette station */

/* DÃ©clarations en avant
 * --------------------
 * Ces fonctions tracent l'opÃ©ration qu'elles effectuent;
 */

/* Demande de message Ã  transmettre */
static bool message_to_transmit(struct packet *ppack);
	
/* RÃ©ception d'un message */
static void trace_received(const struct packet *ppack); 
   
/* RÃ©ception accusÃ© de rÃ©ception */
static void trace_acknowledged(const struct packet *ppack);

/* Propagation simple */
static void trace_passing(const struct packet *ppack);     

/* Ãmission d'un message */
static void trace_emitting(const struct packet *ppack);    

/* Traitement des signaux */
static bool passing = false;    // mode passant
static bool has_token = false;  // la station possÃ¨de le "token"
pid_t pid_station;              // pid de cette station
jmp_buf env;                    // point de reprise

/* Le paquet courant */
struct packet current_packet;

/* Handler du signal SIGUSR1: gÃ¨re l'arrÃªt d'une station */
void on_sig(int sig)
{
    fprintf(stderr, "station %d : ", this_station);
    if (sig == SIGUSR1) {
        fprintf(stderr, "station %d : entrÃ©e en mode passant\n", this_station);
        // Dans tous les cas on se met en mode passant
        passing = true;

        // L'arrÃªt a lieu alors que la station possÃ¨de le jeton
        if (has_token) {
            fprintf(stderr, "station %d : arrÃªt avec token\n", this_station);
            // On jette le paquet courant et on rÃ©injecte un paquet libre
            current_packet.m_token = TOK_FREE;
            write(1, &current_packet, SIZE_PACKET);
            has_token = false; // pour la forme...
	    // On reprend au dÃ©but de la boucle principale
            siglongjmp(env, 1);
        } 
    }
    else
        fprintf(stderr, "station %d : signal %d inattendu\n", this_station, sig);
}

/* Programme principal
 * -------------------
 *
 * La station est connectÃ©e Ã  ses deux voisines par l'intermÃ©diaire de 'stdin'
 * (0) et 'stdout' (1); elle est Ã©galement capable de lire et d'Ã©crire sur le
 * terminal.
 * 									
 * L'algorithme est le suivant:
 * 									
 * 	1. la station se bloque en lecture de paquet sur son entrÃ©e standard.
 * 									
 *	2. lorsque la lecture se dÃ©bloque et que le jeton est libre, la station
 *	demande sur le terminal si il y a un message Ã  envoyer et, si oui, Ã 
 *	quelle station et quel est son contenu. Dans l'affiramtive elle compose
 *	le paquet et l'envoit sur l'anneau; sinon, elle propage le paquet avec
 *	le jeton libre sur l'anneau.
 *	
 * 	3. si le jeton n'est pas libre, c'est qu'il y a des donnÃ©es valides qui
 * 	suivent. Trois cas sont alors possibles :
 * 									
 *	    3.1. La station est destinatrice des donnÃ©es : elle les traite
 *	    (ici elle se contente de les afficher), marque qu'elle les a recues
 *	    correctement (dans le champ accusÃ© de rÃ©ception du paquet) et
 *	    retransmet le paquet sur le rÃ©seau pour qu'il puisse revenir Ã  la
 *	    station Ã©mettrice.
 * 									
 * 	    3.2. La station est l'Ã©mettrice du paquet : ceci signifie que le
 * 	    message a fait un tour complet.  La station peut alors dÃ©terminer
 * 	    par l'analyse  de l'accusÃ© de rÃ©ception si le destinataire a
 * 	    bien recu le message. Dans tous les cas, la station doit libÃ©rer
 * 	    le jeton, et transmettre un paquet Ã  jeton libre sur l'anneau.
 * 									
 *	    3.3. Aucun des 2 cas prÃ©cÃ©dents : le paquet ne concerne pas la
 *	    station qui le propage tel quel sur l'anneau.
 * 									
 * C'est la station 0 qui injecte le premier paquet (avec un jeton libre) sur
 * le rÃ©seau.
 *
 * Noter que le dialogue avec l'utilisateur est Ã©crit de faÃ§on Ã  Ã©viter les
 * erreurs de dÃ©bordement de tableaux et de chaines ("buffer overflow"); En
 * particulier on Ã©vite le recours Ã  des fonctions comme gets() et scanf().
 * 
 * Dans cette version le signal SIGUSR1 met la station en mode PASSANT,
 * c'est-Ã -dire qu'elle ne participe plus au rÃ©seau. Elle se contente de passer
 * le paquet tel quel. Cependant, dans le cas oÃ¹ la station reÃ§oit SIGUSR1
 * alors qu'elle possÃ¨de le jeton, elle pert le paquet courant et regÃ©nÃ¨re un
 * jeton libre avant de se mettre en mode passant.
 */ 
int main(int argc, char *argv[])
{
    int n;
        
    /* Initialisation du numÃ©ro de la station courante  */
    if (argc != 2)
    {
        fprintf(stderr, "Usage : station.exe numÃ©ro\n");
        exit(1);
    }
    this_station = atoi(argv[1]);
    pid_station = getpid();
 
    /* Ouverture du terminal utilisateur pour les traces */
    fp_in = fopen("/dev/tty", "r");
    fp_out = fopen("/dev/tty", "w");
    if (fp_in == NULL || fp_out == NULL) 
    {
	fprintf(stderr, "Erreur station %d: malaise avec le terminal\n",
                this_station);
	exit(1);
    }  
    setbuf(fp_out, NULL);   /* Ã©vite la bufferisation en sortie */
       
    /* La station 0 lance la ronde */
    if (this_station == 0)
    {
	current_packet.m_token = TOK_FREE;	/* jeton libre */
	write(1, &current_packet, SIZE_PACKET);
    }

    /* Mise en place du handler de signal pour SIGUSR1 */
    sigset_t sigmsk;
    sigemptyset(&sigmsk);
    struct sigaction sigact;
    sigact.sa_handler = on_sig;
    sigact.sa_mask = sigmsk;
    sigaction(SIGUSR1, &sigact, NULL);

    /* Mise en place d'un point de reprise pour le mode PASSANT */
    sigsetjmp(env, 0);
    
    /* Boucle infinie de gestion du rÃ©seau et des paquets */
    while(true) 
    {
	/* La station se bloque, en attente de paquet sur stdin */
	n = read(0, &current_packet, SIZE_PACKET);

        /* Si un signal arrive, le read() est interrompu ; on reboucle dessus */
        if (n < 0 && errno == EINTR)
        {
            fprintf(stderr, "station %d : read() interrompu\n", this_station);
            continue;
        }

        has_token = true;
        if (! passing) {

            /* Un paquet est arrivÃ© */
            assert(n == SIZE_PACKET);
            if (current_packet.m_token != TOK_FREE)
            {	    
                /* Le jeton n'est pas libre : il y a un message joint */
                if (current_packet.m_dest != this_station
                    && current_packet.m_orig != this_station)
                {
                    /* Ce paquet ne nous concerne pas ; on le passe au voisin */
                    trace_passing(&current_packet);
                }
                else
                {
                    /* Ce paquet nous concerne
                     *
                     * Attention: il faut traiter correctement le cas oÃ¹ c'est un
                     * message Ã  nous-mÃªmes qui a fait un tour complet !
                     */
                    if (current_packet.m_dest == this_station) 
                    {
                        /* ce paquet nous est destinÃ© */
                        trace_received(&current_packet);
                        current_packet.m_token = TOK_ACK; /* accusÃ© de rÃ©ception */
                    }
                    if (current_packet.m_orig == this_station) {
                        /* Nous avons Ã©mis ce message et il a fait un tour complet */
                        trace_acknowledged(&current_packet);
                        current_packet.m_token = TOK_FREE;	/* libÃ¨re le jeton */
                    }
                }
            }
            else if (message_to_transmit(&current_packet))
            {
                /* le jeton est libre et nous voulons transmettre */
                trace_emitting(&current_packet);
            }
	
        }

	/* Dans tous les cas on entretient le rÃ©seau en propageant le paquet */
	write(1, &current_packet, SIZE_PACKET);
        has_token = false;
    }
    /*NOTREACHED*/
}

/* Demande du message Ã  transmettre au terminal		
 * ---------------------------------------------
 * Le packet transmis en paramÃ¨tre est modifiÃ© par la fonction lorsqu'il 
 * y a un message Ã  transmettre. Sinon, il est inchangÃ©.								
 */
bool message_to_transmit(struct packet *ppack)
{
    char msg[MSG_LEN];
    char line[MAX_LINE];
    assert(MSG_LEN <= MAX_LINE);
    char *line_read;
    int ndest;
    bool result;
    
    sigset_t sigmsk;
    sigemptyset(&sigmsk);
    sigaddset(&sigmsk, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigmsk, NULL);

    fprintf(fp_out, "station %d (%d): avez-vous un message Ã  transmettre ? [y/n] ",
            this_station, pid_station);
    line_read = fgets(line, MAX_LINE, fp_in);
    assert(line_read != NULL);

    switch (line[0]) 
    {
    case 'y':
    case 'Y':
    case 'o':
    case 'O':

        // Lecture du message Ã  envoyer
	fprintf(fp_out, "station %d (%d) : lequel? ", this_station, pid_station);
	// fscanf(fp_in, "%s", msg);
        line_read = fgets(line, MAX_LINE, fp_in);
        assert(line_read != NULL);
        sscanf(line, "%s", msg);

        // Lecture du numÃ©ro de la station destinatrice
	fprintf(fp_out, "station %d (%d) : Ã  quelle station ? ", this_station, pid_station);
	line_read = fgets(line, MAX_LINE, fp_in);
        assert(line_read != NULL);
        sscanf(line, "%d", &ndest);

        // PrÃ©paration du packet
        strcpy(ppack->m_message, msg);
	ppack->m_token = TOK_BUSY;		/* on prend le jeton */
        ppack->m_dest = ndest;
        ppack->m_orig = this_station;

	result = true;

    default:

        result = false;
    }

    sigprocmask(SIG_UNBLOCK, &sigmsk, NULL);
    return result;
}

/* Fonctions de trace				
 * ------------------
 */

/* La station vient de recevoir un message valide qui lui Ã©tait adressÃ© */
static void trace_received(const struct packet *ppack)
{
    fprintf(fp_out, "station %d (%d) : reÃ§u de station %d : %s\n",
	    this_station, pid_station, ppack->m_orig, ppack->m_message);
}

/* Le dernier message transmis par la station a fait un tour complet : on analyse
 * l'accusÃ© de rÃ©ception pour savoir si le destinataire l'a correctement reÃ§u
 */
static void trace_acknowledged(const struct packet *ppack)
{
    fprintf(fp_out, "station %d (%d) : envoi Ã  station %d : message %s transmis : %s\n",
	    this_station, pid_station, ppack->m_dest,
            (ppack->m_token == TOK_ACK) ? "correctement" : "incorrectement",
	    ppack->m_message);
}

/* Un message valide transite juste Ã  travers la station */
static void trace_passing(const struct packet *ppack)
{
    fprintf(fp_out, "station %d (%d) : passage de paquet de station %d Ã  %d: %s\n",
	    this_station, pid_station,  ppack->m_orig, ppack->m_dest, ppack->m_message);
}

/* La station transmet un nouveau message */ 
static void trace_emitting(const struct packet *ppack)
{
    fprintf(fp_out, "station %d (%d) : Ã©mission d'un message pour station %d : %s\n",
	    this_station, pid_station, ppack->m_dest, ppack->m_message);
}

