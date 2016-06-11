/* ------------------------------------------------------------------------------------
 * Un exemple d'implémentation du cache d'un fichier afin d'explorer
 * l'effet des algorithmes de gestion et de remplacement
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: main_Cache_List.c,v 1.2 2008/02/29 16:21:12 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Programme de test unitaire de Cache_List
 * ------------------------------------------------------------------------------------
 */

#include <stdio.h>

#include "cache.h"
#include "low_cache.h"
#include "cache_list.h"

#define FILE "foo"
#define NBLOCKS 3
#define NRECORDS 5
#define RECORDSZ 12

static void Cache_List_Print(const char *msg, struct Cache_List *list,
                             struct Cache *pcache)
{
    struct Cache_List *cell;
   
    fprintf(stderr, "lru_list : %s : [ ", msg);
    for (cell = list->next; cell != list; cell = cell->next)
    {
        fprintf(stderr, "%d ", cell->pheader - pcache->headers);
    }
    fprintf(stderr, "]\n");
}

int main()
{
    int i;
    struct Cache *pcache = Cache_Create(FILE, NBLOCKS, NRECORDS, RECORDSZ, 0);
    struct Cache_List *list = Cache_List_Create();

    for(i = 0; i < NBLOCKS; ++i)
    {
        Cache_List_Append(list, &pcache->headers[i]);
    }
    Cache_List_Print("Liste après append", list, pcache);
    printf("Liste empty ? %d\n", Cache_List_Is_Empty(list));

    Cache_List_Clear(list);
    Cache_List_Print("Liste après clear", list, pcache);
    printf("Liste empty ? %d\n", Cache_List_Is_Empty(list));

    for(i = 0; i < NBLOCKS; ++i)
    {
        Cache_List_Prepend(list, &pcache->headers[i]);
    }
    Cache_List_Print("Liste après prepend", list, pcache);

    for(i = 0; i < NBLOCKS; ++i)
    {
        Cache_List_Append(list, &pcache->headers[i]);
        Cache_List_Prepend(list, &pcache->headers[i]);
    }
    Cache_List_Print("Liste après append et prepend", list, pcache);

    Cache_List_Remove_First(list);
    Cache_List_Remove_Last(list);
    Cache_List_Print("Liste après remove first et last", list, pcache);

    Cache_List_Remove(list, &pcache->headers[2]);
    Cache_List_Print("Liste après remove 2", list, pcache);

    Cache_List_Move_To_End(list, &pcache->headers[0]);
    Cache_List_Move_To_End(list, &pcache->headers[1]);
    Cache_List_Print("Liste après move to end 0 et 1", list, pcache);

    Cache_List_Move_To_Begin(list, &pcache->headers[0]);
    Cache_List_Move_To_Begin(list, &pcache->headers[0]);
    Cache_List_Print("Liste après move to begin 0 et 0", list, pcache);
    

    Cache_List_Delete(list);

    return 0;

}


