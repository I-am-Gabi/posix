/* ------------------------------------------------------------------
 * Un exemple d'implémentation du cache d'un fichier afin d'explorer
 * l'effet des algorithmes de gestion et de remplacement
 * ------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: cache_list.c,v 1.2 2008/02/29 16:21:12 jpr Exp $
 * ------------------------------------------------------------------
 * Une liste simple de blocs du cache : spécification
 * ------------------------------------------------------------------
 */

#include <stdlib.h>

#include "cache_list.h"

/* Fonction de création et d'initialisation
 * ----------------------------------------
 * Elle met en place la liste vide, une simple sentinelle.
 */
struct Cache_List *Cache_List_Create()
{
   /* On alloue la première cellule
      (inutilisée sauf comme sentinelle) de la liste */
    struct Cache_List *list = malloc(sizeof(struct Cache_List));
    
    list->pheader = NULL;
    list->next = list->prev = list;

    return list;
}

/* Fonction de destruction
 * -----------------------
 */
void Cache_List_Delete(struct Cache_List *list)
{
    Cache_List_Clear(list);
    free(list);
}

/* Insertion d'un élément à la fin de la liste
 * -------------------------------------------
 * Une cellule conteant le pointeur pbh est allouée comme dernière cellule de la liste
 */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    struct Cache_List *new_cell = malloc(sizeof(struct Cache_List));
    
    new_cell->pheader = pbh;
    new_cell->next = list;
    new_cell->prev = list->prev;
    list->prev->next = new_cell;
    list->prev = new_cell;

    /* cas de la liste initialement vide */
    if (list->next == list) list->next = new_cell; 
}

/* Insertion d'un élément au début de la liste
 * -------------------------------------------
 * Une cellule conteant le pointeur pbh est allouée comme première cellule de la liste
 */
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    struct Cache_List *new_cell = malloc(sizeof(struct Cache_List));
    
    new_cell->pheader = pbh;
    new_cell->next = list->next;
    new_cell->prev = list;
    list->next->prev = new_cell;
    list->next = new_cell;

    /* cas de la liste initialement vide */
    if (list->prev == list) list->prev = new_cell;
}

/* Retrait du premier élément de la liste
 * --------------------------------------
 */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list)
{
    struct Cache_List *old_cell;
    struct Cache_Block_Header *pbh;

    old_cell = list->next;
    if (old_cell == list) return NULL;
    old_cell->prev->next = old_cell->next;
    old_cell->next->prev = old_cell->prev;

    pbh = old_cell->pheader;
    free(old_cell);
    return pbh;
}

/* Retrait du dernier élément de la liste
 * --------------------------------------
 */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list)
{
    struct Cache_List *old_cell;
    struct Cache_Block_Header *pbh;

    old_cell = list->prev;
    if (old_cell == list) return NULL;
    old_cell->prev->next = old_cell->next;
    old_cell->next->prev = old_cell->prev;

    pbh = old_cell->pheader;
    free(old_cell);
    return pbh;
}

/* Retrait d'un élément de la liste
 * --------------------------------
 * On recherche le premier élément contenant pbh et on le retire de la
 * liste. On retourne pbh (ou NULL si cette valeur n'était pas présente dans
 * la liste).
 */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,
                                             struct Cache_Block_Header *pbh)
{
    struct Cache_List *old_cell;

    for (old_cell = list->next; old_cell != list; old_cell = old_cell->next)
    {
        if (old_cell->pheader == pbh)
        {
            if (old_cell == list) return NULL; /* au cas où... mais a priori impossible */
            old_cell->prev->next = old_cell->next;
            old_cell->next->prev = old_cell->prev; 
            free(old_cell);
            return pbh;
        }  
    }

    return NULL;   
}

/* Retour à l'état de liste vide
 * -----------------------------
 */
void Cache_List_Clear(struct Cache_List *list)
{
    while (list->next != list)
        (void)Cache_List_Remove_First(list);
}

/* Test de liste vide
 * ------------------
 */
int Cache_List_Is_Empty(struct Cache_List *list)
{
    return list == list->next;
}


/* Transfert d'un élément à la fin de la liste
 * -------------------------------------------
 * On recherche le premier élément contenant pbh et on le transfère à la fin de la
 * liste. Si pbh n'est pas dans la liste, il est quand même placé à la fin. 
 */
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    struct Cache_Block_Header *pbh1;

    if (list->prev->pheader == pbh)
        return;
    pbh1 = Cache_List_Remove(list, pbh);
    if (pbh1 == NULL) pbh1 = pbh;
    Cache_List_Append(list, pbh1);
}

/* Transfert d'un élément au début de la liste
 * -------------------------------------------
 * On recherche le premier élément contenant pbh et on le transfèreau début de la
 * liste. Si pbh n'est pas dans la liste, il est quand même placé au début.
 */
void Cache_List_Move_To_Begin(struct Cache_List *list,
                              struct Cache_Block_Header *pbh)
{
    struct Cache_Block_Header *pbh1;

    if (list->next->pheader == pbh)
        return;
    pbh1 = Cache_List_Remove(list, pbh);
    if (pbh1 == NULL) pbh1 = pbh;
    Cache_List_Prepend(list, pbh1);
}

