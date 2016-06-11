#------------------------------------------------------------------
# Un exemple d'implémentation de malloc()/free()
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: Makefile,v 1.4 2008/03/04 16:54:33 jpr Exp $
#------------------------------------------------------------------
# Makefile
#------------------------------------------------------------------

# Compilateur et options
CC = gcc 
CFLAGS = -g -std=c99  -Wall

# Documentation
DOXYGEN = doxygen

# Répertoires pour l'installation des sujets
INSTALL_DIR = ../../Sujets
TD_DIR = Malloc
TD_LNK = TD01
INSTALL_FILES = main_mymalloc1.c main_mymalloc2.c mymalloc.h

INSTALL_ESSI = $$HOME/NET4/TD_TP/Systeme/ESSI_1/Sujets

#------------------------------------------------------------------
# Cibles principales
#------------------------------------------------------------------

all : tst_mymalloc1.exe tst_mymalloc2.exe tst_mymalloc1-alt.exe

tst_mymalloc1.exe : main_mymalloc1.o mymalloc.o
	$(CC) -o tst_mymalloc1.exe main_mymalloc1.o mymalloc.o

tst_mymalloc2.exe : main_mymalloc2.o mymalloc.o
	$(CC) -o tst_mymalloc2.exe main_mymalloc2.o mymalloc.o

tst_mymalloc1-alt.exe : main_mymalloc1.o mymalloc-alt.o
	$(CC) -o tst_mymalloc1-alt.exe main_mymalloc1.o mymalloc-alt.o

doc : doc_made

clean : all
	-rm *.o

clobber full_clean :
	-rm *.o *.exe 
	-rm -rf doc_made doc

#------------------------------------------------------------------
# Dépendances spécifiques
#------------------------------------------------------------------

doc_made : $(wildcard *.[ch] *.dox)
	$(DOXYGEN) Doxyfile
	cat /dev/null > doc_made

mymalloc.o : mymalloc.h mymalloc_impl.h
main_mymalloc1.o main_mymalloc2.o : mymalloc.h

mymalloc-alt.o : mymalloc.h mymalloc_impl.h
	$(CC) $(CFLAGS) -c -o mymalloc-alt.o -DMYMALLOC_ALT mymalloc.c

#------------------------------------------------------------------
# Installations diverses (locale et répertoire étudiant de l'ESSI)
#------------------------------------------------------------------

install :
	-mkdir $(INSTALL_DIR)
	chmod a+rx $(INSTALL_DIR)	
	-mkdir $(INSTALL_DIR)/$(TD_DIR)
	cp $(INSTALL_FILES) $(INSTALL_DIR)/$(TD_DIR)
	-cp Makefile.install $(INSTALL_DIR)/$(TD_DIR)/Makefile
	-for f in *.install; do \
	    cp $$f $(INSTALL_DIR)/$(TD_DIR)/`basename $$f .install`; \
	done 
	chmod a+rx $(INSTALL_DIR)/$(TD_DIR)
	chmod -R a+r $(INSTALL_DIR)/$(TD_DIR)
	-cd $(INSTALL_DIR); rm -f $(TD_LNK); ln -s $(TD_DIR) $(TD_LNK)

install_ESSI :
	make install INSTALL_DIR=$(INSTALL_ESSI)
