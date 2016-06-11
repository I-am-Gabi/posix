#---------------------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#---------------------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: Makefile,v 1.3 2008/04/14 15:15:49 jpr Exp $
#---------------------------------------------------------------------------------
# Makefile
#---------------------------------------------------------------------------------

# Compilateur et options
# ----------------------

# On supprime l'option -pedantic qui provoque des warnings de conversion 
# interdite entre void * et pointeurs sur fonctions en ISO C lors de 
# l'utilisation de dlsym().
#
# En revanche, on ajoute l'option -fpic (ou -fPIC) pour créer les .o des
# bibliothèques partagées. Cette option n'est pas utile pour les bibliothèques
# statiques, mais elle n'est pas nuisible non plus.

CC = gcc
CFLAGS = -std=c99 -g -fpic 

# Répertoires et fichiers pour l'installation des sujets
# ------------------------------------------

INSTALL_DIR = ../../Sujets
TD_DIR = Shared_Lib
TD_LNK = TD03
INSTALL_FILES = $(wildcard *.c *.h)
INSTALL_ESSI = $$HOME/SFE_JPR/TD_TP/Système/ESSI_1/Sujets

# Répertoire du projet Cache dont celui-ci dépend
# -----------------------------------------------

CACHE_DIR=../Cache
CACHE_SRC_HEADER = cache.h low_cache.h strategy.h cache_list.h random.h
CACHE_SRC_BASE = cache.c low_cache.c cache_list.c main_Cache.c 
CACHE_SRC_STRATEGY = RAND_strategy.c FIFO_strategy.c LRU_strategy.c NUR_strategy.c

CACHE_SRC = $(CACHE_SRC_HEADER) $(CACHE_SRC_BASE) $(CACHE_SRC_STRATEGY)
CACHE_LNK = $(addprefix $(CACHE_DIR)/,$(CACHE_SRC))

# Exécutables à construire
# ------------------------

NONLIB_PROGS =	 tst_Cache_RAND.exe tst_Cache_FIFO.exe \
	         tst_Cache_LRU.exe tst_Cache_NUR.exe

STATIC_PROGS = 	 tst_Cache_RAND-static.exe tst_Cache_FIFO-static.exe \
	         tst_Cache_LRU-static.exe tst_Cache_NUR-static.exe

DYNAMIC_PROGS =	 tst_Cache_RAND-dynamic.exe tst_Cache_FIFO-dynamic.exe \
	         tst_Cache_LRU-dynamic.exe tst_Cache_NUR-dynamic.exe

LOADABLE_PROGS = tst_Cache-loadable.exe 

PROGS =		 $(NONLIB_PROGS) $(STATIC_PROGS) $(DYNAMIC_PROGS) $(LOADABLE_PROGS)

# Bibliothèques à construire
# --------------------------

STATIC_LIBS =    libCache_RAND-static.a libCache_FIFO-static.a \
		 libCache_LRU-static.a libCache_NUR-static.a

DYNAMIC_LIBS =   libCache_RAND-dynamic.so libCache_FIFO-dynamic.so \
		 libCache_LRU-dynamic.so libCache_NUR-dynamic.so

LOADABLE_LIBS =  libStrategy-RAND.so  libStrategy-FIFO.so  \
		 libStrategy-LRU.so  libStrategy-NUR.so 

LIBS =		 $(STATIC_LIBS) $(DYNAMIC_LIBS) $(LOADABLE_LIBS)

#---------------------------------------------------------------------------------
# Cibles principales
#---------------------------------------------------------------------------------

# Exécutables et bibliothèques
# ----------------------------

all : links $(LIBS) $(PROGS)

# Nettoyages
# ----------

# Nettoyage léger : 
# conserve les bibliothèques et les exécutables
clean : all
	-rm -f \#* *.o foo

# Nettoyage en profondeur : 
# détruit tous les fichiers générés, y compris les liens avec Cache
clobber full_clean :
	-rm -f \#* *.o foo
	-rm -f $(PROGS) $(LIBS)
	-rm $(CACHE_SRC)
	-rm .links

#---------------------------------------------------------------------------------
# Exécutables construits sans bibliothèques 
#---------------------------------------------------------------------------------

# Règle par défaut pour les exécutables
# 	%' sert de "joker" dans la règle
#	$@ est le nom de la cible (partie gauche de la règle)
#	$^ est la liste des dépendances (partie droite de la règle)
# Noter que cache_list.o n'est pas utile pour toutes les stratégies, 
# mais ici, on veut une règle unique...

tst_Cache_%.exe : main_Cache.o cache.o low_cache.o %_strategy.o cache_list.o
	$(CC) -o $@ $^

#---------------------------------------------------------------------------------
# Exécutables construits avec bibliothèques statiques
#---------------------------------------------------------------------------------

# Règle par défaut pour les exécutables

tst_Cache_%-static.exe : main_Cache.o libCache_%-static.a 
	$(CC) -o $@ $^

# Bibliothèques statiques (règle par défaut)

libCache_%-static.a : cache.o low_cache.o %_strategy.o cache_list.o
	ar r $@ $^
	ranlib $@  

#---------------------------------------------------------------------------------
# Exécutables construits avec bibliothèques dynamiques chargeables automatiquement
#---------------------------------------------------------------------------------

# Règle par défaut pour les exécutables ('%' sert de "joker" dans la règle)

tst_Cache_%-dynamic.exe : main_Cache.o libCache_%-dynamic.so 
	$(CC) -o $@ $^


# Bibliothèques dynamique (règle par défaut) : on y met de toutes façons
# cache_list.o bien que ce soit inutile pour RAND et NUR, mais cela rend plus
# facile l'écriture d'une règle unique et cela ne coute pas vraiment.

libCache_%-dynamic.so : cache.o low_cache.o %_strategy.o cache_list.o
	$(CC) -shared -Wl,-soname,$@ $^ -o $@

#---------------------------------------------------------------------------------
# Exécutables construits avec bibliothèques chargeables dynamiquement
#---------------------------------------------------------------------------------

# Il n'y a ici plus qu'un seul exécutable
# Pour l'option --export-dynamic, voir 'man dlopen' man ld'

# NOTE : --export-dynamic est supposé être synonyme de -rdynamic,mais sur mon
# Linux, cela ne fonctionne pas (à l'exécution) (Mandrake Linux 10.1)

tst_Cache-loadable.exe : main_Cache-loadable.o cache.o load_strategy.o
	$(CC) -rdynamic  -o tst_Cache-loadable.exe \
			 main_Cache-loadable.o cache.o low_cache.o load_strategy.o -ldl

# Bibliothèques dynamiques de gestion des stratégies

libStrategy-%.so : %_strategy.o cache_list.o
	$(CC) -shared -Wl,-soname,$@ $^ -o $@

#---------------------------------------------------------------------------------
# Etablissement des liens avec le répertoire Cache
# Ceci doit être fait avant de commencer à travailler
#---------------------------------------------------------------------------------

links : .links

.links : 
	for f in $(CACHE_LNK) ; do \
		ln -s $$f ; \
	done
	date > .links

#---------------------------------------------------------------------------------
# Dependances individuelles (obtenues par 'gcc -MM *.c')
#---------------------------------------------------------------------------------

cache.o: cache.c cache.h low_cache.h strategy.h
cache_list.o: cache_list.c cache_list.h
FIFO_strategy.o: FIFO_strategy.c strategy.h low_cache.h cache.h \
  cache_list.h
load_strategy.o: load_strategy.c strategy.h load_strategy.h
low_cache.o: low_cache.c low_cache.h cache.h
LRU_strategy.o: LRU_strategy.c strategy.h low_cache.h cache.h \
  cache_list.h
main_Cache.o: main_Cache.c cache.h strategy.h random.h
main_Cache-loadable.o: main_Cache-loadable.c cache.h strategy.h random.h
NUR_strategy.o: NUR_strategy.c strategy.h low_cache.h cache.h
RAND_strategy.o: RAND_strategy.c strategy.h low_cache.h cache.h random.h

#---------------------------------------------------------------------------------
# Tests
#---------------------------------------------------------------------------------

tests : all regress sizes search_list

# Test de non régression) par rapport au projet Cache

regress : 
	cd $(CACHE_DIR); make simul
	@echo "*** Début des tests de non régression"
	@echo "*** Il ne doit y avoir aucun autre message jusqu'à la fin des tests" 
	@for strat in RAND FIFO NUR LRU ; do \
		tst_Cache_$$strat-static.exe | \
					diff - $(CACHE_DIR)/$${strat}_default.out ; \
		tst_Cache_$$strat-dynamic.exe | \
					diff - $(CACHE_DIR)/$${strat}_default.out ; \
		tst_Cache-loadable.exe $$strat  | \
					diff - $(CACHE_DIR)/$${strat}_default.out ; \
	done
	@echo "*** Fin des tests de non régression"

# Taille des exécutables
# 
#	- Il n'y a pas de différences significatives entre les versions statiques 
#	  (avec ou sans bibliothèques)
#	- Les exécutables qui utilisent des bibliothèques statiques (*-static.exe) 
#         sont sensiblement plus gros (~30 %) que leur contrepartie dynamique 
#	  (*-dynamic.exe).
#	- Ironiquement, le plus gros des exécutables est celui qui correspond au 
#         "tout dynamique" ! Cela est du au fait que la bibliothèques libdl.so 
#         (qui, elle, est liée *statiquement*), est assez grosse ! Evidemment,
#         cet effet paradoxal disparait dès que les bibliothèques chargées 
#         dynamiquement sont très grosses.

sizes : 
	@echo "*** Taille des exécutables"
	size *.exe 

# Modules chargés
#
#	- Les exécutable liés sans bibliothèque comportent le code de cache_list.o,
#	  puisque leur règle de compilation par défaut est aveugle.
#	- En revanche, pour les exécutables liés avec une librairie statique, 
#	  seuls ceux aui en ont réellement besoin comportent une copie de cache_list.o.
#	- Pour les exécutables liés avec une bibliothèque partagée (édition de liens 
#         statique ou dynamique), il n'y a évidemment pas de copie de cache_list.o
#         dans le fichier exécutable lui-même.

search_list : 
	@echo "*** Exécutables contenant les fonctions de cache_list "
	@-for f in  *.exe ; do \
		nm $$f | grep -q Cache_List  && echo $$f ; \
	done

#---------------------------------------------------------------------------------
# Installations diverses (locale et répertoire étudiant de l'ESSI)
#---------------------------------------------------------------------------------

install : 
	$(MAKE) links
	$(MAKE) all 
	-mkdir $(INSTALL_DIR)
	chmod a+rx $(INSTALL_DIR)	
	-mkdir $(INSTALL_DIR)/$(TD_DIR)
	cp $(INSTALL_FILES) $(INSTALL_DIR)/$(TD_DIR)
	-cp Makefile.install $(INSTALL_DIR)/$(TD_DIR)/Makefile
	chmod a+rx $(INSTALL_DIR)/$(TD_DIR)
	chmod -R a+r $(INSTALL_DIR)/$(TD_DIR)	
	-cd $(INSTALL_DIR); rm -f $(TD_LNK); ln -s $(TD_DIR) $(TD_LNK)

install_ESSI :
	make install INSTALL_DIR=$(INSTALL_ESSI)
