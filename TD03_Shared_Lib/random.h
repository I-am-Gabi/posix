/* ------------------------------------------------------------------------------------
 * Tirer au hard un nombre entier 
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: random.h,v 1.1.1.1 2005/03/22 11:44:19 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Voir 'man 3 rand'
 * ------------------------------------------------------------------------------------
 */

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdlib.h>

/* RANDOM(m, n) tire un nombre au hasard dans l'intervalle [m,n[. */
#define RANDOM(m, n) ((m)+(unsigned int)((double)((n) - (m)) * rand() / (RAND_MAX + 1.0)))

#endif /* _RANDOM_H_ */ 
