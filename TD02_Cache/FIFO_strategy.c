/*!
 * \file FIFO_strategy.c
 *
 * \brief  Stratégie de remplacement FIFO (<em>First In First Out</em>).
 *
 * On gère une liste des blocs du cache. Chaque fois qu'un bloc est alloué ou
 * remplacé il est placé dans cette liste, à la fin. Le bloc le plus ancien se
 * trouve donc au début de la liste. C'est lui qui sera remplacé lorsqu'on ne
 * trouvera plus aucun bloc libre.
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: FIFO_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include "strategy.h"
#include "low_cache.h"
#include "cache_list.h" 

//! Transforme le pointeur sur la stratégie en un pointeur sur la liste FIFO
#define FIFO_LIST(pcache) ((struct Cache_List *)((pcache)->pstrategy))

/*!
 * FIFO : On alloue et initialise la liste FIFO.
 */
void *Strategy_Create(struct Cache *pcache) 
{
    /* Création et initialisation de la liste FIFO */
    return Cache_List_Create();
}

/*!
 * FIFO : On se débarasse de la liste FIFO.
 */
void Strategy_Close(struct Cache *pcache)
{
    Cache_List_Delete(FIFO_LIST(pcache));
}

/*!
 * FIFO : On vide la liste FIFO.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
    Cache_List_Clear(FIFO_LIST(pcache));
}

/*!
 * FIFO : On prend le premier bloc invalide. S'il n'y en a plus on prend le premier
 * bloc de la liste FIFO, c'est-à-dire le bloc le plus ancien. Comme ce bloc
 * devient le plus récent, on le passe en fin de liste.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    struct Cache_Block_Header *pbh;
    struct Cache_List *fifo_list = FIFO_LIST(pcache);

     /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL)
    {
        /* Les blocs invalides ne sont pas dans la liste ; mettons y celui-ci */
        Cache_List_Append(fifo_list, pbh);
        return pbh;
    }

    /* Sinon on prend le premier bloc de la liste FIFO et on le déplace à la
     * fin de la liste
     */
    pbh = Cache_List_Remove_First(fifo_list);
    Cache_List_Append(fifo_list, pbh);

    return pbh;    
}

/*!
 * FIFO : Ici on ne fait rien, seul l'age de la page compte, pas la fréquence des accès.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
}  

/*!
 * FIFO : Ici on ne fait rien, seul l'age de la page compte, pas la fréquence des accès.
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
} 

char *Strategy_Name()
{
    return "FIFO";
}
