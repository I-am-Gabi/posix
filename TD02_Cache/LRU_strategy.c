/*!
 * \file LRU_strategy.c
 *
 * \brief  Stratégie de remplacement LRU (<em>Least Recently Used</em>).
 *
 * On gère une liste des blocs du cache. Chaque fois qu'un bloc est utilisé
 * (alloué, lu ou modifié) il est placé dans cette liste, à la fin. Le bloc le
 * moins récemment utilisé se trouve donc au début de la liste. C'est lui qui
 * sera remplacé lorsqu'on ne trouvera plus aucun bloc libre.
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: LRU_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include "strategy.h"
#include "low_cache.h"
#include "cache_list.h"

//! Transforme le pointeur sur la stratégie en un pointeur sur la liste LRU
#define LRU_LIST(pcache) ((struct Cache_List *)((pcache)->pstrategy))


/*!
 * LRU : On alloue et initialise la liste LRU.
 */
void *Strategy_Create(struct Cache *pcache) 
{
    /* Création et initialisation de la liste LRU */
    return Cache_List_Create();
}

/*! 
 * LRU : On se débarasse de la liste LRU.
 */
void Strategy_Close(struct Cache *pcache)
{
    Cache_List_Delete(LRU_LIST(pcache));
}

/*!
 * LRU : On vide la liste LRU.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
    Cache_List_Clear(LRU_LIST(pcache));
}

/*!
 * LRU : On prend le premier bloc invalide. S'il n'y en a plus on prend le
 * premier bloc de la liste LRU c'est-à-dire le bloc le plus anciennement
 * utilisé. Comme ce bloc devient le plus récent, on le passe en fin de liste.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    struct Cache_Block_Header *pbh;
    struct Cache_List *lru_list = LRU_LIST(pcache);

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL)
    {
        /* Les blocs invalides ne sont pas dans la liste ; mettons-le en queue */
        Cache_List_Append(lru_list, pbh);  
        return pbh;
    }

    /* Sinon on prend le premier bloc de la liste LRU et on le déplace à la fin
     * de la liste */
    pbh = Cache_List_Remove_First(lru_list);
    Cache_List_Append(lru_list, pbh);

    return pbh;    
}

/*! 
 * LRU : Le bloc référencé devient le bloc le plus récent et passe donc en queue de
 * la liste LRU.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
    Cache_List_Move_To_End(LRU_LIST(pcache), pbh);
} 
 
/*! 
 * LRU : Le bloc référencé devient le bloc le plus récent et passe donc en queue de
 * la liste LRU.
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    Cache_List_Move_To_End(LRU_LIST(pcache), pbh);
} 

char *Strategy_Name()
{
    return "LRU";
}
