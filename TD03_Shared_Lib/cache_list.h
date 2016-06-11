/* ------------------------------------------------------------------
 * Un exemple d'implémentation du cache d'un fichier afin d'explorer
 * l'effet des algorithmes de gestion et de remplacement
 * ------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: cache_list.h,v 1.2 2008/02/29 16:21:12 jpr Exp $
 * ------------------------------------------------------------------
 * Une liste simple de blocs du cache : spécification
 * ------------------------------------------------------------------
 */

#ifndef _CACHE_LIST_
#define _CACHE_LIST_

/* La cellule de liste doublement chainée */
struct Cache_List
{
    struct Cache_Block_Header *pheader;	/* information */
    struct Cache_List *next;		/* chainage avant */
    struct Cache_List *prev;		/* chainage arrière */

};

/* Création et destruction */
struct Cache_List *Cache_List_Create();
void Cache_List_Delete(struct Cache_List *list);

/* Insertion d'un élément au début et à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh);
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh);

/* Retrait du premier, du dernier ou d'un quelconque élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list);
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list);
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,
                                             struct Cache_Block_Header *pbh);

/* Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list);

/* Test de liste vide */
int Cache_List_Is_Empty(struct Cache_List *list);

/* Transférer un élément à la fin ou au début */
void Cache_List_Move_To_End(struct Cache_List *list,
                            struct Cache_Block_Header *pbh);
void Cache_List_Move_To_Begin(struct Cache_List *list,
                              struct Cache_Block_Header *pbh);

#endif /* CACHE_LIST_ */
