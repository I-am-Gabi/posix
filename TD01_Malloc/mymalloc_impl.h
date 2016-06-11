#ifndef _MYMALLOC_IMPL_H_
#define _MYMALLOC_IMPL_H_
/*! 
 * \file mymalloc_impl.h
 * \brief Définition des structures de données utilisées par l'implémentation.
 *
 * \author Jean-Paul Rigault
 */

/*! \brief Entête d'un bloc.
 *
 * \note Dans cette version simple, on ne tient pas compte
 * des contraintes d'alignement des adresses mémoire. De toutes façons la
 * structure suivante devrait être bien alignée sur la plupart des machines
 */
struct Header
{
    unsigned int sizenh;   //!< Taille totale (utile + header de gestion) du bloc en nombre d'entêtes
    struct Header *next;   //!< Pointeur sur le bloc suivant dans la liste libre
};

/*! \brief Taille de l'entête.
 *
 * C'est l'unité d'allocation de l'espace mémoire, exprimée en nombre de
 * caractères. Toutes les autres tailles sont exprimées en multiple de \c
 * HEADER_SZ et sont nommées avec le sufffixe \c NH (nombre de "headers")
 */
#define HEADER_SZ sizeof(struct Header)

/*!  \brief Taille minimale (en nombre de headers) de mémoire demandée au système.
 *
 * Lorsqu'on réclame de la mémoire au système grâce à l'appel de \c sbrk(), on
 * demande au moins un bloc de taille <tt>SYSTBLK_NH*HEADER_SZ</tt>.
 *
 * \note En général cette taille est un multiple de la taille de page du
 * système. Nous ne tenons pas compte de cette contrainte ici.
 */
#define MINSYSTBLOCK_NH 64

/*! Une fonction d'arrondi.
 *
 * <tt>NBLOCKS(sz, blksz)</tt> cherche combien de blocks de taille \a blksz sont
 * nécessaires au minimum pour contenir une information de taille \a sz. Bien
 * entendu, \a sz et \a blksz doivent être dans la même unité.
 *
 * \param sz la taille demandée
 * \param blocksz la taille de block
 * \return le nombre de blocs nécessaire pour contenir une information de taille \a sz
 */
#define NBLOCKS(sz, blocksz) (1 + ((sz) - 1) / (blocksz))

/* Instrumentation
 */
struct Instrum 
{
    int nsysblk;       	//!< Nombre total de blocs demandés à \c sbrk().
    int syssz;		//!< Taille totale demandée à \c sbrk().
    int nuserblk;	//!< Nombre total de blocs actuellement alloués pour l'utilisateur.
    int nfree;		//!< Nombre de blocs actuellement dans la liste libre.
    int nuserblkfrom;	//!< Nombre total de blocs demandés par l'utilisateur.
    int userszfrom;	//!< Taille totale en caractères demandéel'utilisateur.
    int nuserblkback;	//!< Nombre total de blocs rendus par l'utilisateur.
    int userszback;	//!< Taille totale en caractères rendue l'utilisateur.
    int nmergeblk;	//!< Nombre de fusions de blocks
};

//! La variable globale d'instrumentation
static struct Instrum instrum = {0, 0, 0, 0, 0, 0, 0, 0, 0};

#endif

