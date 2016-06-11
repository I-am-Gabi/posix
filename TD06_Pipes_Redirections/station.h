/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: station.h,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Réseau en anneau à jeton (token ring) : structure de données de la station
 * ---------------------------------------------------------------------------------
 */

#ifndef _STATION_H_
#define _STATION_H_

// Structure des messages
// ----------------------

// Taille maximale des données d'un message 
#define MSG_LEN 100

// Longueur maximale d'une ligne au terminal
#define MAX_LINE 100		

// États d'un jeton
enum token 
{
    TOK_FREE,	// le jeton est libre, aucun message n'est attaché 
    TOK_ACK,	// le paquet est un accusé de réception
    TOK_BUSY	// le paquet contient un message en cours de transmission
};

// Le paquet
struct packet 
{
    enum token m_token;		// le jeton
    short m_orig;		// le numéro de la station émettrice
    short m_dest;		// le numéro de la station destinatrice 
    char m_message[MSG_LEN];	// les donneés (i.e., le message lui-même)
};

// Taille de l'ensemble du paquet
const int SIZE_PACKET = sizeof(struct packet);	

#endif
