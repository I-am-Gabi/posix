/*!
 * \file mymalloc.c 
 * \brief Implémentation de l'allocateur de mémoire simple.
 *
 * \note On donne deux réalisations très légèrement différentes de \c
 * mymalloc() : la première utilise une seule boulce de recherche qui est
 * parcourue deux fois en cas d'allocation par \c sbrk() ; la seconde réalise
 * ce deuxième parcours par un appel récursif de \c myalloc() (qui ne sera
 * exécuté qu'une seule fois). Choisir entre les deux formes est une affaire de
 * goût...
 * 
 * \author Jean-Paul Rigault  
 */

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "mymalloc.h"
#include "mymalloc_impl.h"

/*! \brief La sentinelle de la liste des blocs libres.
 *
 * Les blocs libres sont chainés entre eux dans une liste circulaire simplement
 * chainée, rangée dans l'ordre des adresses machines croissantes. Les
 * chainages sont effectués par l'intermédiaire de l'entête de gestion des
 * blocs. Le début (et la fin, puisqu'elle est circulaire !) de cette liste est
 * indiqué par une "sentinelle", c'est-à-dire un bloc "bidon" qui ne correspond
 * pas à un véritable bloc de mémoire allouée. Cette sentinelle est toujours
 * présente, et est nommée ici \c base. Par commodité, le pointeur \c pfree contient
 * l'adresse de cette base (il reste constant).
 *
 * Au départ la liste de blocs libres est vide, donc \c base pointe sur elle-même.
 */
static struct Header base = {0, &base};	

//! Pointeur courant sur la liste des blocs libres.
 static struct Header *pfree = &base;	

//! Fonction interne 'allocation de mémoire système (definie plus loin)
 static void *system_alloc(unsigned int nh);

/*!
 * Ceci est la fonction principale d'allocation mémoire, un substitut de \c malloc().
 *
 * Elle reçoit la taille \a sz (en caractères) à allouer. Elle calcule le nombre
 * d'entêtes (\a nh) nécessaires pour allouer cette taille (plus une entête
 * servant à la gestion). Puis elle parcourt la liste libre pour trouver le
 * premier block de taille suffisante. Si elle trouve, elle découpe ce block
 * pour ne retenir que ce dont elle a besoin et recycle le reste dans la liste
 * libre. Si elle ne trouve pas elle appelle \a system_alloc() pour demander de
 * la mémoire supplémentaire au système, place la mémoire obtenue dans la liste
 * libre et continue sa recherche (qui sera alors immédiatement fructueuse).
 *
 * Le pointeur retourné pointe sur la première adresse "utile" (c'est-à-dire
 * utilisable pour les données de l'appelant) du block ainsi alloué.
 *
 * \param sz le nombre de caractères à allouer
 * \return un pointeur sur le début du bloc utilisable par l'appelant
 */
#ifndef MYMALLOC_ALT
void *mymalloc(size_t sz)
{
    unsigned int nh; 		// taille nécessaire en nombre d'entêtes
    struct Header *ph;		// Pointeur sur le bloc libre courant
    struct Header *phprev;	// Pointeur sur le bloc libre précédent 

    // On doit allouer suffisamment d'entêtes pour contenir la taille sz plus 1
    // pour assurer la gestion
     
    nh = NBLOCKS(sz, HEADER_SZ) + 1;

    // printf("sz = %zu, HEADER_SZ = %zu, nh = %u\n", sz, HEADER_SZ, nh);

    // Instrumentation 
    ++instrum.nuserblk;		// un bloc utilisateur de plus 
    ++instrum.nuserblkfrom;
    instrum.userszfrom += nh * HEADER_SZ;	// taille utile pour l'utilisateur 

    // Parcours de la liste libre : on doit garder un pointeur sur le bloc
    // précédent
     
    for (phprev = pfree, ph = pfree->next;  ; phprev = ph, ph = ph->next)
    {
        if (ph->sizenh >= nh) 
        {
            // Le block courant est de taille suffisante 
            if (ph->sizenh == nh)
            {
                // Ici, c'est même *exactement* la taille nécessaire, on retire
                // ce block de la liste libre en le "shuntant"
                 
                phprev->next = ph->next;

                --instrum.nfree;	// un bloc libre de moins 
            }
            else
            {
                // Ici le block trouvé est trop grand, on le découpe, la partie
                // utile étant à la fin et le début étant recyclé dans la liste
                // libre
                 
                ph->sizenh -= nh;	// on prend ce dont on a besoin 
                ph += ph->sizenh; 	// on saute le début qui va rester libre 
                ph->sizenh = nh;  	// ph pointe maintenant sur l'entête de
                                        // gestion du bloc que l'on va retourner,
                                        // on ajuste la taille (la valeur du
                                        // pointeur next est indifférente 

                // Ici on n'a pas à mettre à jour nfree, car il reste un bloc
                // libre
                 
            }

            // Dans tous les cas de succès, on retourne un pointeur sur la
            // première adresse utile (c'est-à-dire juste après l'entête de
            // gestion) 
            return ph + 1;
        }

        // Le block libre courant n'est pas assez grand, ou alors il n'y  a
        // plus de bloc libre.
         

        if (ph == pfree)
        {
            // On a fait un tour complet de la liste libre sans rien
            // trouver. On demande donc assez de mémoire au système pour
            // allouer nos sizenh
             
            if ((ph = system_alloc(nh)) == NULL) 
            {
                // On ne peut plus rien obtenir 
                fprintf(stderr, "mymalloc: error: no more system memory\n");
                return NULL;
            }
            // On a pu allouer de la mémoire système et on peut donc continuer
            // notre parcours de la liste libre qui va maintenant, par
            // un bloc de taille suffisante.
            // 
            // Noter que system_alloc() retourne l'adresse du bloc à partir
            // duquel le parcours peut continuer.
             
        }

        // Continuation de la boucle de parcours des blocks libres 
    }
    /*NOTREACHED*/
}

#else // MYMALLOC_ALT
void *mymalloc(size_t sz)
{
    unsigned int nh; 		// taille nécessaire en nombre d'entêtes
    struct Header *ph;		// Pointeur sur le bloc libre courant
    struct Header *phprev;	// Pointeur sur le bloc libre précédent 

    // On doit allouer suffisamment d'entêtes pour contenir la taille sz plus 1
    // pour assurer la gestion
     
    nh = NBLOCKS(sz, HEADER_SZ) + 1;

    // printf("sz = %zu, HEADER_SZ = %zu, nh = %u\n", sz, HEADER_SZ, nh);

    // Parcours de la liste libre : on doit garder un pointeur sur le bloc
    // précédent ; on s'arre quand on a fait un tour complet
     
    for (phprev = pfree, ph = pfree->next; ph != pfree  ; phprev = ph, ph = ph->next)
    {
        if (ph->sizenh >= nh) 
        {
            // Le block courant est de taille suffisante 

            if (ph->sizenh == nh)
            {
                // Ici, c'est même *exactement* la taille nécessaire, on retire
                // ce block de la liste libre en le "shuntant"
                 
                phprev->next = ph->next;

                --instrum.nfree;	// un bloc libre de moins 
            }
            else
            {
                // Ici le block trouvé est trop grand, on le découpe, la partie
                // utile étant à la fin et le début étant recyclé dans la liste
                // libre
                 
                ph->sizenh -= nh;	// on prend ce dont on a besoin 
                ph += ph->sizenh; 	// on saute le début qui va rester libre 
                ph->sizenh = nh;  	// ph pointe maintenant sur l'entête de
                                        // gestion du bloc que l'on va retourner,
                                        // on ajuste la taille (la valeur du
                                        // pointeur next est indifférente) 

                // Ici on n'a pas à mettre à jour nfree, car il reste un bloc
                // libre
                 
            }

            // Instrumentation 
            ++instrum.nuserblk;		// un bloc utilisateur de plus 
            ++instrum.nuserblkfrom;
            instrum.userszfrom += nh * HEADER_SZ;	// taille utile pour l'utilisateur 

            // Dans tous les cas de succès, on retourne un pointeur sur la
            // première adresse utile (c'est-à-dire juste après l'entête de
            // gestion) 
             
            return ph + 1;
        }
    }

    // Ici on a parcouru toute la liste libre sans trouver de bloc de taille
    // suffisante.
     

    // On demande au système suffisamment de mémoire pour allouer notre
    // requête. Le bloc correspondant sera placé dans la liste libre 
     
    if ((ph = system_alloc(nh)) == NULL) 
    {
        // On ne peut plus rien obtenir 
        fprintf(stderr, "mymalloc: error: no more system memory\n");
        return NULL;
    }

    // On a pu obtenir de la mémoire.  On appelle donc à nouveau mymalloc() :
    // ce second appel va réussir puisque l'on vient d'injecter un bloc libre
    // de taille suffisante.
     
    return mymalloc(sz);
} 

#endif // MYMALLOC_ALT

/*!
 * Ceci est la fonction de libération mémoire, un substitut de \c free().
 *
 * Le paramètre \a ptr est un pointeur sur la première adresse utile du bloc
 * (i.e., celle qui suit l'entête de gestion). Bien entendu \a ptr doit avoir une
 * valeur retournée par un précédent appel de \c mymalloc(). Libérer plusieurs
 * fois le même bloc peut être catastrophique.
 *
 * On parcourt la liste de blocs libres pour trouver le premier (rappelons-nous
 * que la liste des blocs libres est triée par adresses croissantes) dont
 * l'adresse est supérieure à celle du bloc que l'on libère. Si les deux blocs
 * sont adjacents (pour les adresses machines), on constitue un gros bloc libre
 * par fusion. De même si le bloc à libérer est immédiatement précédé (toujours
 * pour les adresses machines) d'un bloc libre, on fusionne également. Dans
 * tous les cas le bloc ainsi obtenu est remis dans la liste libre.
 *
 * \param ptr l'adresse du bloc à libérer (il s'agit de l'adresse "utile", celle
 * transmise à l'utilisateur)
 *
 * \note Cette fonction ne vérifie pas que le bloc libéré a été effectivement
 * obtenu par un appel de mymalloc(). Elle se contente de considerer le début
 * du bloc comme un header et de placer ce bloc à sa bonne position
 * (c'est-à-dire en conservant la liste libre ordonnée par adresse
 * croissante). Ce comportement est voulu : d'une part, la vérification d'un
 * malloc précédent peut être couteuse ; d'autre part, cette fonction peut donc
 * être utilisée pour placer n'importe quel bloc (muni d'un header correct)
 * dans la liste libre... par exemple un bloc alloué par system_alloc().
 */
void myfree(void *ptr)
{
    struct Header *ptrh;	// Le début du bloc correspondant à l'adresse utile ptr
    struct Header *ph;		// Pointeur sur le bloc libre courant 
    struct Header *phprev;	// Pointeur sur le bloc libre précédent

    // ptr étant l'adresse utile, le début du bloc commence un entête avant 
    ptrh = (struct Header *)ptr - 1;

    ++instrum.nuserblkback; // on rend ce block
    instrum.userszback += ptrh->sizenh * HEADER_SZ; // on rend cette taille
    
    // Parcours de la liste libre afin de trouver le bloc juste suivant. 
    for (phprev = pfree, ph = pfree->next;  ; phprev = ph, ph = ph->next)
    {
        // Soit on fait un tour complet sans rien trouver (p == pfree), soit le
        // bloc courant suit immédiatement le bloc à libérer
         
        if (ph == pfree || ph > ptrh) break;
    }

    /* Ici, deux cas sont possibles :
     *
     * Soit on a trouvé un bloc libre dont l'adresse suit celle du bloc à
     * libérer. Dans ce cas ph pointe sur ce bloc libre et phprev pointe sur le
     * dernier bloc libre dont l'adresse précède celle du bloc à libérer (ou
     * bien phprev == pfree, auquel cas cela signifie que le bloc à libérer va
     * se placer en tête de la liste libre).
     *
     * Soit on n'a pas trouvé de tel bloc et alors ph == pfree et phprev pointe
     * sur le dernier bloc libre de la liste (ou phprev == pfree si la liste en
     * question est vide). Dans ce cas le bloc à libérer doit devenir le
     * dernier de la liste.
     *
     * On voit donc que *dans tous les cas* les deux pointeurs phprev et ph
     * "encadrent" la position où l'on doit placer le bloc à libérer dans la
     * liste.
     */

    // Instrumentation 
    ++instrum.nfree;			// un bloc libre de plus 
    --instrum.nuserblk;			// un bloc utilisateur de moins 
    
    // On essaie de fusionner avec le bloc suivant 
    if (ptrh + ptrh->sizenh == ph) 
    {
        // Le bloc à libérer et le bloc libre qui suit sont adjacents 
        ptrh->sizenh += ph->sizenh;	// somme des taille 
        ptrh->next = ph->next;		// maintien de la liste libre 

        --instrum.nfree;	// un bloc libre de moins 
        ++instrum.nmergeblk;	// un bloc fusionné
    }
    else 
    {
        // Pas de fusion avec le bloc libre qui suit 
        ptrh->next = ph;		// maintien de la liste libre 
    }

    // On essaie de fusionner avec le bloc précédent 
    if (phprev + phprev->sizenh == ptrh)
    {
         // Le bloc à libérer et le bloc libre qui précède sont adjacents 
        phprev->sizenh += ptrh->sizenh; 	// somme des taille 
        phprev->next = ptrh->next;		// maintien de la liste libre 

        --instrum.nfree;	// un bloc libre de moins     
        ++instrum.nmergeblk;	// un bloc fusionné
    }
    else 
    {
         // Pas de fusion avec le bloc libre qui précède 
        phprev->next = ptrh;		// maintien de la liste libre 
    }
}

/*!
 * Cette fonction demande effectivement de la mémoire au systèmes, grâce à \c sbrk()).
 *
 * Le paramètre est le nombre d'entêtes. Cette fonction se contente d'ajouter
 * un bloc libre de la taille demandée dans la liste des blocs libres. Elle
 * retourne un pointeur à partir duquel le parcours de la liste libre peut
 * continuer dans \c mymalloc().
 *
 * \param nh le nombre d'entêtes à allouer
 * \return le pointeur sur la liste libre à partir duquel continuer la
 * recherche ou nul si on ne peut plus obtenbir de mémoire
 *
 * \note En cas de succès cette fonction retourne en fait toujours le début de
 * la liste libre (\c pfree). En effet nous ne savons pas où le nouveau bloc se
 * place dans cette liste et il faut donc repartir du début pour ne pas briser
 * la stratégie de "first fit".
*/
static void *system_alloc(unsigned int nh)
{
    extern void *sbrk(); 	// Under some Linusses, this is not accessible in unistd.h 

    struct Header *phnew;	// pointeur sur la nouvelle zone mémoire 

    // On alloue toujours un multiple de MINSYSTBLOCK_NH
     
    unsigned int nsysblock = NBLOCKS(nh, MINSYSTBLOCK_NH);

    // Demande de mémoire au système (l'argument de sbrk() est en nombre de
    // caractères)
     
    int nhnew = nsysblock * MINSYSTBLOCK_NH; // nombre de headers
    size_t sz_to_alloc = nhnew * HEADER_SZ;
    phnew = (struct Header *)sbrk(sz_to_alloc);
    if ((intptr_t)phnew < 0) return NULL; 	// plus de mémoire 

    // Mise à jour des informations de gestion du nouveau bloc 
    phnew->sizenh = nhnew;

    // Instrumentation

    ++instrum.nsysblk;		// un bloc système de plus 
    instrum.syssz += sz_to_alloc;
    ++instrum.nuserblk;	// c'est comme si il y avait un bloc utilisateur de plus 
    instrum.userszfrom += sz_to_alloc;
    ++instrum.nuserblkfrom;

    /* On doit placer le nouveau bloc dans la liste de blocs libres, à
     * l'emplacement correspondant à son adresse machine. Le plus simple est de
     * faire comme s'il s'agissait d'un bloc qu'on "libère" et donc d'appeler
     * myfree() qui le placera à l'endroit correct de la liste libre.
     *
     * Mais attention ! La fonction myfree() doit être invoquée avec l'adresse
     * "utile" du bloc, c'est-à-dire l'adresse qui suit l'entête de gestion
     * (d'où le + 1 qui suit)
     */
    myfree(phnew + 1);
 
    // Nous n'avons aucune garantie sur la position de l'adresse du nouveau
    // bloc (phnew) par rapport à celle des blocs déjà existants. Il faut donc
    // que le parcours de la liste libre reparte du début.
     
    return pfree;    
}

/*!
 * Cette fonction imprime les résultats de l'instrumentation :
 *
 * <dl>
 * <dt>\a sysblk</dt>	
 *    <dd>nombre de blocks système obtenus pas sbrk() [ne décroit pas]</dd>
 * <dt>\a syssz</dt>	
 *    <dd>taille totale (en caractères) obtenue par sbrk() [ne décroit pas]</dd> 
 * <dt>\a  userblk</dt>	
 *    <dd>nombre de blocks couramment alloués par l'appelant (incrémenté
 *		de 1 par mymalloc(), décrémenté de 1 par myfree())</dd>
 * <dt>\a totalsz</dt>	
 *    <dd>taille totale (en caractères) allouée pour l'appelant (c'est la
 *		taille totale des blocs précédents</dd>
 * <dt>\a nfree</dt>	
 *    <dd>nombre de blocs actuellement dans la liste libre</dd>
 * </dl>
 */
void mymalloc_instrum()
{
    fprintf(stderr, "Instrumentation : paramètres de configuration\n");
    fprintf(stderr, "---------------------------------------------\n");
    fprintf(stderr, "\tTaille du header [bytes] : HEADER_SZ = %zu\n", HEADER_SZ);
    fprintf(stderr, "\tTaille du bloc de sbrk() [bytes]: MINSYSTBLOCK  = %zu\n",
            MINSYSTBLOCK_NH * HEADER_SZ);

    fprintf(stderr, "Instrumentation : statistiques d'exécution\n");
    fprintf(stderr, "------------------------------------------\n");
    fprintf(stderr, "\tNombre de blocs demandés à sbrk() : %d\n", instrum.nsysblk);
    fprintf(stderr, "\tTaille totale demandée à sbrk() : %d\n", instrum.syssz);

    fprintf(stderr, "\tNombre total de blocks alloués à l'utilisateur : %d\n", instrum.nuserblkfrom);
    fprintf(stderr, "\tTaille totale allouée à l'utilisateur [bytes] : %d\n", instrum.userszfrom);

    fprintf(stderr, "\tNombre total de blocs rendus par l'utilisateur : %d\n", instrum.nuserblkback);
    fprintf(stderr, "\tTaille totale rendue par l'utilisateur [bytes] : %d\n", instrum.userszback);

    fprintf(stderr, "\tNombre de fusion de blocks : %d\n", instrum.nmergeblk);

    fprintf(stderr, "\tNombre courant de blocs alloués à l'utilisateur : %d\n", instrum.nuserblk);
    fprintf(stderr, "\tNombre courant de blocs dans la liste libre : %d\n", instrum.nfree);
}
