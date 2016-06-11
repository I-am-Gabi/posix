/*!
 * \file low_cache.c
 *
 * \brief Implémentation du cache.
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: low_cache.c,v 1.1 2008/03/04 16:52:49 jpr Exp $
 */

#include <assert.h>

#include "low_cache.h"

/*! 
 * \ingroup low_cache_interface
 *
 * Recherche d'un bloc libre (cad invalide)
 *
 * Ici la situation est simple car le cache se remplit séquentiellement et
 * l'invalidation ne peut être que globale. Soit le pointeur \c pfree du cache est
 * non nul et il pointe sur le premier bloc libre du cache, soit il est nul et
 * le cache est plein. 
 *
 * Cette fonction est invoquée par les stratégies de gestion du cache.
 * 
 * \param pcache un pointeur sur le cache
 * \return un pointeur sur un bloc libre ou nul s'il n'y en a plus
*/
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache)
{
    struct Cache_Block_Header *pbh = pcache->pfree;

    if (pbh != NULL)
    {
        assert((pbh->flags & VALID) == 0);
            
        /* On fait pointer pfree sur le block suivant à moins que l'on soit
         * arrivé à la fin */
        if (++pcache->pfree >= pcache->headers + pcache->nblocks) pcache->pfree = NULL;   
    }

    return pbh;
}
