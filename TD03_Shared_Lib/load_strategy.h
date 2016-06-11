/* ------------------------------------------------------------------------------------
 * Un exemple d'implémentation du cache d'un fichier afin d'explorer
 * l'effet des algorithmes de gestion et de remplacement
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: load_strategy.h,v 1.2 2008/04/14 15:15:49 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Interface avec une bibliothèque de stratégie chargée dynamiquement
 * ------------------------------------------------------------------------------------
 */

#ifndef _LOAD_STRATEGY_H_
#define _LOAD_STRATEGY_H_

/* ------------------------------------------------------------------------------------
 *  Définition du type énuméré désignant les fonctions d'interface
 * ------------------------------------------------------------------------------------
*/

enum Strategy_Function
{
    STRATEGY_CREATE,		/* == 0 */
    STRATEGY_CLOSE,
    STRATEGY_INVALIDATE,
    STRATEGY_REPLACE,
    STRATEGY_READ,
    STRATEGY_WRITE,
    STRATEGY_NAME
};

static const int N_STRATEGY_FUNCTIONS = STRATEGY_NAME + 1;

/* ------------------------------------------------------------------------------------
 *  Définition du tableau de correspondance entre le numéro de la fonction et un 
 * pointeur sur son code
 * ------------------------------------------------------------------------------------
*/

/* La structure de correspondance elle-même */

typedef void (*ANY_PTR_FUNC)();

struct Strategy_Interface
{
    char *fname;	/* le nom de la fonction */
    ANY_PTR_FUNC fptr;	/* un pointeur sur son code : on prend un pointeur sur
                         * fonction --- de type queconque --- pour éviter un
                         * warning */
};

/* Le tableau de correspondance (statique, car défini dans un .h */
struct Strategy_Interface strategy_functions[] = 
{
    {"Strategy_Create", NULL},		/* indice STRATEGY_CREATE */
    {"Strategy_Close", NULL},
    {"Strategy_Invalidate", NULL},
    {"Strategy_Replace_Block", NULL},
    {"Strategy_Read", NULL},
    {"Strategy_Write", NULL},
    {"Strategy_Name", NULL},
};

/* ------------------------------------------------------------------------------------
 *  Définition des types de fonctions d'interface 
 * ------------------------------------------------------------------------------------
*/

/* Ceci est le type  Strategy_Create()
 * 	struct Cache * --> void *
 */
typedef void *(*STRATEGY_CREATE_TYPE)(struct Cache *);

/* Ceci est le type  Strategy_Close() et Strategy_Invalidate()
 * 	struct Cache * --> void
 */
typedef void (*STRATEGY_CLOSE_INVALIDATE_TYPE)(struct Cache *);

/* Ceci est le type de Strategy_Replace_Block()
 * 	struct Cache * --> struct Cache_Block_Header *
 */
typedef struct Cache_Block_Header *(*STRATEGY_REPLACE_TYPE)(struct Cache *);

/* Ceci est le type de Strategy_Read() et Strategy_Write()
 * 	struct Cache * x struct Cache_Block_Header * --> void
 */
typedef void (*STRATEGY_READ_WRITE_TYPE)(struct Cache *, struct Cache_Block_Header *);

/* Ceci est le type de Strategy_Name
 * 	void --> char *
 */
typedef char *(*STRATEGY_NAME_TYPE)(void);

/* ------------------------------------------------------------------------------------
 * Macro de transtypage des fonctions de la stratégie
 * Convertit le pointeur sur fonction d'indice strategy_func (un enum Strategy_Func)
 * en type
 * ------------------------------------------------------------------------------------
*/

#define STRATEGY_FUNC(type, sfunc) ((type)(strategy_functions[sfunc].fptr))

/* ------------------------------------------------------------------------------------
 * Nom de la bibliothèque de stratégie (juste le préfixe)
 * ------------------------------------------------------------------------------------
*/

/* Préfixe du nom */
static const char *libname_prefix = "libStrategy-" ; 
static const char *libname_suffix = ".so" ; 

/* Longueur totale du nom : ceci devrait suffire */
#define NLIBNAME 50

/* ------------------------------------------------------------------------------------
 * Nom de la stratégie
 * ------------------------------------------------------------------------------------
*/

extern char *The_Strategy;

#endif /* _LOAD_STRATEGY_H_ */
