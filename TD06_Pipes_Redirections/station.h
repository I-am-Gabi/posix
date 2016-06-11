/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: station.h,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * R�seau en anneau � jeton (token ring) : structure de donn�es de la station
 * ---------------------------------------------------------------------------------
 */

#ifndef _STATION_H_
#define _STATION_H_

// Structure des messages
// ----------------------

// Taille maximale des donn�es d'un message 
#define MSG_LEN 100

// Longueur maximale d'une ligne au terminal
#define MAX_LINE 100		

// �tats d'un jeton
enum token 
{
    TOK_FREE,	// le jeton est libre, aucun message n'est attach� 
    TOK_ACK,	// le paquet est un accus� de r�ception
    TOK_BUSY	// le paquet contient un message en cours de transmission
};

// Le paquet
struct packet 
{
    enum token m_token;		// le jeton
    short m_orig;		// le num�ro de la station �mettrice
    short m_dest;		// le num�ro de la station destinatrice 
    char m_message[MSG_LEN];	// les donne�s (i.e., le message lui-m�me)
};

// Taille de l'ensemble du paquet
const int SIZE_PACKET = sizeof(struct packet);	

#endif
