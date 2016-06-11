#------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: Makefile,v 1.5 2008/03/04 16:52:49 jpr Exp $
#------------------------------------------------------------------
# Makefile principale
#------------------------------------------------------------------

# Compilateur et options

CC = gcc #-m32
CFLAGS = -std=c99 -pedantic -Wall -g 

# Documentation

DOXYGEN = doxygen

# Répertoires pour l'installation des sujets

INSTALL_DIR = ../../Sujets
TD_DIR = Cache
TD_LNK = TD02
INSTALL_FILES = cache.h cache_list.h low_cache.h random.h strategy.h \
		RAND_strategy.c main_Cache.c \
		main_Cache.o cache.o low_cache.o cache_list.o \
		*_default.out \
		scripts/plot.sh

INSTALL_ESSI = $$HOME/NET4/TD_TP/Système/ESSI_1/Sujets

# Exécutables à construire

PROGS = tst_Cache_RAND.exe tst_Cache_FIFO.exe tst_Cache_LRU.exe tst_Cache_NUR.exe

#------------------------------------------------------------------
# Cibles principales
#------------------------------------------------------------------

# Exécutables

all : $(PROGS)

tst_Cache_RAND.exe : main_Cache.o cache.o low_cache.o RAND_strategy.o cache_list.o
	$(CC) -o tst_Cache_RAND.exe main_Cache.o cache.o \
	                        low_cache.o RAND_strategy.o cache_list.o

tst_Cache_FIFO.exe : main_Cache.o cache.o low_cache.o FIFO_strategy.o cache_list.o
	$(CC) -o tst_Cache_FIFO.exe main_Cache.o cache.o  \
                                low_cache.o FIFO_strategy.o cache_list.o

tst_Cache_LRU.exe : main_Cache.o cache.o low_cache.o LRU_strategy.o cache_list.o
	$(CC) -o tst_Cache_LRU.exe main_Cache.o cache.o \
                                low_cache.o LRU_strategy.o cache_list.o

tst_Cache_NUR.exe : main_Cache.o cache.o low_cache.o NUR_strategy.o
	$(CC) -o tst_Cache_NUR.exe main_Cache.o cache.o low_cache.o NUR_strategy.o

# Documentation

doc : doc_made

# Nettoyages

clean : all
	-rm -f \#* *.o *.out *.backup.fm foo

clobber full_clean :
	-rm -f \#* *.o *.out *.backup.fm foo 
	-rm -f $(PROGS) tst_Cache_List.exe
	-rm -rf Plots
	-rm -rf doc_made doc

#------------------------------------------------------------------
# Dependances individuelles
#------------------------------------------------------------------

doc_made : $(wildcard *.[ch] *.dox)
	$(DOXYGEN) Doxyfile
	cat /dev/null > doc_made

cache.o: cache.h low_cache.h strategy.h
low_cache.o: low_cache.h cache.h
cache_list.o: cache_list.h
make FIFO_strategy.o: strategy.h low_cache.h cache.h cache_list.h 
LRU_strategy.o: strategy.h low_cache.h cache.h cache_list.h 
main_Cache.o: main_Cache.c cache.h strategy.h random.h
NUR_strategy.o: low_cache.h cache.h 
RAND_strategy.o: strategy.h low_cache.h cache.h random.h

#------------------------------------------------------------------
# Installations diverses (locale et répertoire étudiant de l'ESSI)
#------------------------------------------------------------------

install : all simul
	-mkdir $(INSTALL_DIR)
	chmod a+rx $(INSTALL_DIR)	
	-mkdir $(INSTALL_DIR)/$(TD_DIR)
	-mkdir $(INSTALL_DIR)/$(TD_DIR)/Plots
	cp $(INSTALL_FILES) $(INSTALL_DIR)/$(TD_DIR)
	-cp Makefile.install $(INSTALL_DIR)/$(TD_DIR)/Makefile
	chmod a+rx $(INSTALL_DIR)/$(TD_DIR)
	chmod -R a+r $(INSTALL_DIR)/$(TD_DIR)
	chmod a-w $(INSTALL_DIR)/$(TD_DIR)/*.o $(INSTALL_DIR)/$(TD_DIR)/*.out
	-cd $(INSTALL_DIR); rm -f $(TD_LNK); ln -s $(TD_DIR) $(TD_LNK)

install_ESSI :
	make install INSTALL_DIR=$(INSTALL_ESSI)

#------------------------------------------------------------------
# Tests unitaires
#------------------------------------------------------------------

tests : tst_Cache_List.exe

tst_Cache_List.exe : main_Cache_List.o cache_list.o cache.o
	$(CC) -o tst_Cache_List.exe main_Cache_List.o cache_list.o \
		cache.o low_cache.o FIFO_strategy.o

main_Cache_List.o : cache.h low_cache.h cache_list.h

#------------------------------------------------------------------
# Simulation du cache et tracé des courbes
#------------------------------------------------------------------

%_default.out : tst_Cache_%.exe
	$< > $@

simul : RAND_default.out FIFO_default.out LRU_default.out NUR_default.out

plots : all
	-mkdir Plots
	cp Makefile.plots Plots/Makefile
	cd Plots; make
