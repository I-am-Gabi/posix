/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: station.c,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Réseau en anneau à jeton (token ring) : une station de l'anneau
 * ---------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "station.h"

/* Variables globales
 * ------------------
 */
static int this_station;	/* Numéro de cette station */

/* Déclarations en avant
 * --------------------
 * Ces fonctions tracent l'opération qu'elles effectuent;
 */

/* Demande de message à transmettre */
bool message_to_transmit(struct packet *ppack);
	
/* Réception d'un message */
static void trace_received(const struct packet *ppack); 
   
/* Réception accusé de réception */
static void trace_acknowledged(const struct packet *ppack);

/* Propagation simple */
static void trace_passing(const struct packet *ppack);     

/* Émission d'un message */
static void trace_emitting(const struct packet *ppack);    

/* Programme principal
 * -------------------
 *
 * La station est connectée à ses deux voisines par l'intermédiaire de 'stdin'
 * (0) et 'stdout' (1); elle est également capable de lire et d'écrire sur le
 * terminal.
 * 									
 * L'algorithme est le suivant:
 * 									
 * 	1. la station se bloque en lecture de paquet sur son entrée standard.
 * 									
 *	2. lorsque la lecture se débloque et que le jeton est libre, la station
 *	demande sur le terminal si il y a un message à envoyer et, si oui, à
 *	quelle station et quel est son contenu. Dans l'affiramtive elle compose
 *	le paquet et l'envoit sur l'anneau; sinon, elle propage le paquet avec
 *	le jeton libre sur l'anneau.
 *	
 * 	3. si le jeton n'est pas libre, c'est qu'il y a des données valides qui
 * 	suivent. Trois cas sont alors possibles :
 * 									
 *	    3.1. La station est destinatrice des données : elle les traite
 *	    (ici elle se contente de les afficher), marque qu'elle les a recues
 *	    correctement (dans le champ accusé de réception du paquet) et
 *	    retransmet le paquet sur le réseau pour qu'il puisse revenir à la
 *	    station émettrice.
 * 									
 * 	    3.2. La station est l'émettrice du paquet : ceci signifie que le
 * 	    message a fait un tour complet.  La station peut alors déterminer
 * 	    par l'analyse  de l'accusé de réception si le destinataire a
 * 	    bien recu le message. Dans tous les cas, la station doit libérer
 * 	    le jeton, et transmettre un paquet à jeton libre sur l'anneau.
 * 									
 *	    3.3. Aucun des 2 cas précédents : le paquet ne concerne pas la
 *	    station qui le propage tel quel sur l'anneau.
 * 									
 * C'est la station 0 qui injecte le premier paquet (avec un jeton libre) sur
 * le réseau.
 *
 * Noter que le dialogue avec l'utilisateur est écrit de façon à éviter les
 * erreurs de débordement de tableaux et de chaines ("buffer overflow"); En
 * particulier on évite le recours à des fonctions comme gets() et scanf().
 */ 
int main(int argc, char *argv[])
{
    struct packet current_packet;	/* Le paquet courant */
    int n;
        
    /* Initialisation du numéro de la station courante  */
    if (argc != 2)
    {
        fprintf(stderr, "Usage : station.exe numéro\n");
        exit(1);
    }
    this_station = atoi(argv[1]);
     
    /* La station 0 lance la ronde */
    if (this_station == 0)
    {
	current_packet.m_token = TOK_FREE;	/* jeton libre */
	write(1, &current_packet, SIZE_PACKET);
    }
    
    /* Boucle infinie de gestion du réseau et des paquets */
    while(1) 
    {
	/* La station se bloque, en attente de paquet sur stdin */
	n = read(0, &current_packet, SIZE_PACKET);
 
	/* Un paquet est arrivé */
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
                 * Attention: il faut traiter correctement le cas où c'est un
                 * message à nous-mêmes qui a fait un tout complet !
                 */
		if (current_packet.m_dest == this_station) 
		{
		    /* ce paquet nous est destiné */
		    trace_received(&current_packet);
		    current_packet.m_token = TOK_ACK; /* accusé de réception */
		}
		if (current_packet.m_orig == this_station) {
		    /* Nous avons émis ce message et il a fait un tour complet */
		    trace_acknowledged(&current_packet);
		    current_packet.m_token = TOK_FREE;	/* libère le jeton */
		}
	    }
	}
	else if (message_to_transmit(&current_packet))
	{
	    /* le jeton est libre et nous voulons transmettre */
	    trace_emitting(&current_packet);
	}
	
	/* Dans tous les cas on entretient le réseau en propageant le paquet */

	write(1, &current_packet, SIZE_PACKET);
    }
    /*NOTREACHED*/
}

/* Demande du message à transmettre au terminal		
 * ---------------------------------------------
 * Le packet transmis en paramètre est modifié par la fonction lorsqu'il 
 * y a un message à transmettre. Sinon, il est inchangé.								
 */
bool message_to_transmit(struct packet *ppack)
{
    char msg[MSG_LEN];
    char line[MAX_LINE];
    assert(MSG_LEN <= MAX_LINE);
    char *line_read;
    int ndest;    


    fprintf(stderr, "ring: lancement de la station %d\n", this_station);

    // Ouverture du terminal de contrôle pour dialogue utilisateur
    FILE *fp_tty = fopen("/dev/tty", "a+");
    if (fp_tty == NULL) 
    {
	fprintf(stderr, "Erreur station %d: malaise avec le terminal\n",
                this_station);
	exit(1);
    }  
    setbuf(fp_tty, NULL);   /* évite la bufferisation en sortie */
    
    
    fprintf(fp_tty, "station %d : avez-vous un message à transmettre ? [y/n] ",
            this_station);
    line_read = fgets(line, MAX_LINE, fp_tty);
    assert(line_read != NULL);

    switch (line[0]) 
    {
    case 'y':
    case 'Y':
    case 'o':
    case 'O':

        // Lecture du message à envoyer
	fprintf(fp_tty, "station %d : lequel? ", this_station);
	// fscanf(fp_tty, "%s", msg);
        line_read = fgets(line, MAX_LINE, fp_tty);
        assert(line_read != NULL);
        sscanf(line, "%s", msg);

        // Lecture du numéro de la station destinatrice
	fprintf(fp_tty, "station %d : à quelle station ? ", this_station);
	line_read = fgets(line, MAX_LINE, fp_tty);
        assert(line_read != NULL);
        sscanf(line, "%d", &ndest);

        // Préparation du packet
        strcpy(ppack->m_message, msg);
	ppack->m_token = TOK_BUSY;		/* on prend le jeton */
        ppack->m_dest = ndest;
        ppack->m_orig = this_station;

        fclose(fp_tty);
	return true;

    default:

        fclose(fp_tty);
        return false;
    }
    /*NOTREACHED*/
}

/* Fonctions de trace				
 * ------------------
 */

/* La station vient de recevoir un message valide qui lui était adressé */
static void trace_received(const struct packet *ppack)
{
    fprintf(stderr, "station %d : reçu de station %d : %s\n",
	    this_station, ppack->m_orig, ppack->m_message);
}

/* Le dernier message transmis par la station a fait un tour complet : on analyse
 * l'accusé de réception pour savoir si le destinataire l'a correctement reçu
 */
static void trace_acknowledged(const struct packet *ppack)
{
    fprintf(stderr, "station %d : envoi à station %d : message %s transmis : %s\n",
	    this_station, ppack->m_dest,
            (ppack->m_token == TOK_ACK) ? "correctement" : "incorrectement",
	    ppack->m_message);
}

/* Un message valide transite juste à travers la station */
static void trace_passing(const struct packet *ppack)
{
    fprintf(stderr, "station %d : passage de paquet de station %d à %d: %s\n",
	    this_station, ppack->m_orig, ppack->m_dest, ppack->m_message);
}

/* La station transmet un nouveau message */ 
static void trace_emitting(const struct packet *ppack)
{
    fprintf(stderr, "station %d : émission d'un message pour station %d: %s\n",
	    this_station, ppack->m_dest, ppack->m_message);
}

