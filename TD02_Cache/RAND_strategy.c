/*!
 * \file RAND_strategy.c
 *
 * \brief  Stratégie de remplacement au hasard..
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: RAND_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "random.h"

/*!
 * RAND : Rien à faire ici.
 */
void *Strategy_Create(struct Cache *pcache) 
{
    return NULL;
}

/*!
 * RAND : Rien à faire ici.
 */
void Strategy_Close(struct Cache *pcache)
{
}

/*!
 * RAND : Rien à faire ici.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
}

/*! 
 * RAND : On prend le premier bloc invalide. S'il n'y en a plus, on prend un bloc au hasard.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    int ib;
    struct Cache_Block_Header *pbh;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) return pbh;

    /* Sinon on tire un numéro de bloc au hasard */
    ib = RANDOM(0, pcache->nblocks);
    return &pcache->headers[ib];
}


/*!
 * RAND : Rien à faire ici.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
}  

/*!
 * RAND : Rien à faire ici.
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
} 

char *Strategy_Name()
{
    return "RAND";
}
