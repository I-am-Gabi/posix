/* ------------------------------------------------------------------
 * Un exemple d'implémentation du cache d'un fichier afin d'explorer
 * l'effet des algorithmes de gestion et de remplacement
 * ------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: NUR_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 * ------------------------------------------------------------------
 * Stratégie NUR (Not Used Recently)
 * ------------------------------------------------------------------
 */

/*!
 * \file LRU_strategy.c
 *
 * \brief  Stratégie de remplacement NUR (<em>Not Used Recently</em>).
 *
 * 
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: NUR_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include "strategy.h"
#include "low_cache.h"

/*!
 * Flag propre à NUR indiquant que le bloc a été référencé dans la dernière tranche de temps.
 * 
 * On utilise directement le champ flags de la structure Cache_Block_Header
 * (voir low_cache.h).
*/
#define REFER 0x4

//! Valeur par défaut de la période de mise à zéro des bits de référence.
/*!
 * Dans un vrai système ceci serait fait avec une certaine période temporelle
 * (tranche de temps). Ici nous effectuons cette opérations tous les \c nderef
 * accès.
 */
#define NDEREF 100

//! Valeur de vérité d'un entier
#define BOOLVAL(v) ((v) ? 1 : 0)

//! Prend 2 bits m et r et en fait un entier m*2 + r  
#define MAKE_RM(r, m) ((BOOLVAL(r)<<1) | BOOLVAL(m))

//! Structure de données propre à la stratégie NUR.
/*!
 * On remet le bit de référence (\c REFER) à 0 tous les \c nderef accès ;
 * cptderf est le compteur qui décompte le nombre d'accès restant à effectuer
 * avant le prochain déréférençage.
 */
struct NUR_Strategy
{
    unsigned nderef;	//!< Période de déréférençage 
    unsigned cptderef;	//!< Compteur de déréférençage
};
#define NUR_STRATEGY(pcache) ((struct NUR_Strategy *)(pcache)->pstrategy)

//! Fonction auxiliaire effectuant le déréférençage.
static void Dereference_If_Needed(struct Cache *pcache);

/*!
 * NUR : On alloue la structure de données propre à la stratégie et on initialise \c nderef.
 */ 
void *Strategy_Create(struct Cache *pcache) 
{
    struct NUR_Strategy *pstrat = malloc(sizeof(struct NUR_Strategy));

    pstrat->nderef = pcache->nderef;
    pstrat->cptderef = pcache->nderef;

    return pstrat;
}

/*!
 * NUR : On se débarasse de structure de données propre à la stratégie.
 */ 
void Strategy_Close(struct Cache *pcache)
{
    free(pcache->pstrategy);
}

/*!
 * NUR : On force la remise à zéro des bits de référence. Pour cela on appelle
 * simplement Dereference_If_Needed() avec un compteur \c cptderef à 1.
 */
void Strategy_Invalidate(struct Cache *pcache) 
{
    struct NUR_Strategy *pstrat = NUR_STRATEGY(pcache);

    if (pstrat->nderef != 0) 
    {
        pstrat->cptderef = 1;
        Dereference_If_Needed(pcache);
    }   
}

/*!
 * NUR : On prend le premier bloc invalide. S'il n'y en a plus on ordonne les blocs à
 * l'aide des bits REFER et MODIF et on prend le "meilleur" bloc à remplacer :
 * de préférence non référencé et non modifié dans la dernière tranche de
 * temps, puis non référencé et modifié, puis référencé et non modifié, puis en
 * désespoir de cause référencé et modifié.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    int ib;
    int min;
    struct Cache_Block_Header *pbh_save = NULL;
    struct  Cache_Block_Header *pbh;
    int rm;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) return pbh;

    for (min = MAKE_RM(1, 1) + 1, ib = 0; ib < pcache->nblocks; ib++)
    {
	pbh = &pcache->headers[ib];
	
	/* On construit un nombre binaire rm avec le bit de modification
         * et le bit de référence, et on cherche le bloc avec la valeur de rm
         * minimale */
	rm = MAKE_RM(pbh->flags & REFER, pbh->flags & MODIF);
	if (rm == 0) return pbh; /* pas la peine de chercher plus loin */
	else if (rm < min) 
	{
	    min = rm;
	    pbh_save = pbh;
	}	
    }
    return pbh_save;    
}

/*!
 * NUR : On déréférence si nécessaire puis on marque le bloc courant comme référencé. 
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
    Dereference_If_Needed(pcache);
    pbh->flags |= REFER;
}
 
/*!
 * NUR : On déréférence si nécessaire puis on marque le bloc courant comme référencé. 
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    Dereference_If_Needed(pcache);
    pbh->flags |= REFER;
} 

char *Strategy_Name()
{
    return "NUR";
}


/*! 
 * On remet à 0 le bit de référence (\c REFER) tous les deref accès.
 */
static void Dereference_If_Needed(struct Cache *pcache)
{
    struct NUR_Strategy *pstrat = NUR_STRATEGY(pcache);
    int ib;

    /* On déréférence tous les deref accès ; si deref est 0 on ne déréférence jamais */
    if (pstrat->nderef == 0 || --pstrat->cptderef > 0) return;

    /* C'est le moment : on remet à 0 tous les bits de déférence */
    for (ib = 0; ib < pcache->nblocks; ib++) pcache->headers[ib].flags &= ~REFER;

    /* On réarme le compteur et on met à jour l'instrumentation */
    pstrat->cptderef = pstrat->nderef;
    ++pcache->instrument.n_deref;
}

