#ifndef _MYMALLOC_H_
#define _MYMALLOC_H_
/*!
 * \file mymalloc.h 
 * \brief Interface de l'allocateur de mémoire simple.
 * 
 * \author Jean-Paul Rigault
 */

//! Allocation d'une zone de taille \a sz octets.
extern void *mymalloc(size_t sz);

//! Libération du bloc de mémoire commençant à l'adress (utile) \a ptr.
extern void myfree(void *ptr);

//!  Instrumentation.
extern void mymalloc_instrum();

#endif

