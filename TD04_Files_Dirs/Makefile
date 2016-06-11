#---------------------------------------------------------------------------------
# Primitives de POSIX.1 : Manipulations de fichiers et répertoires
#---------------------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Mars 2005
# $Id: Makefile,v 1.4 2008/04/21 18:08:09 jpr Exp $
# --------------------------------------------------------------------------------
# Makefile
#---------------------------------------------------------------------------------

# Compilateur et options
# ----------------------

CC = gcc
CFLAGS = -std=c99 -Wall -g 

# Règle par défaut pour construire foo.exe à partir d'un seul fichier foo.c
# -------------------------------------------------------------------------

%.exe : %.c
	$(CC) $(CFLAGS) -o $@ $<

# Répertoires et fichiers pour l'installation des sujets
# ------------------------------------------------------

INSTALL_DIR = ../../Sujets
TD_DIR = Files_Dirs
TD_LNK = TD04
INSTALL_FILES = 
INSTALL_ESSI = $$HOME/NET4/TD_TP/Système/ESSI_1/Sujets

PROGS = printenv.exe lsrec.exe mycp1.exe mycp2.exe 

#---------------------------------------------------------------------------------
# Cibles principales
#---------------------------------------------------------------------------------

# Exécutables et bibliothèques
# ----------------------------

all : $(PROGS)

# Nettoyages
# ----------

# Nettoyage léger : 
# conserve les bibliothèques et les exécutables
clean : all
	-rm -f \#* *.o 

# Nettoyage en profondeur : 
# détruit tous les fichiers générés, y compris les liens avec Cache
clobber full_clean :
	-rm -f \#* *.o $(PROGS) 
	-rm -rf Tests

#---------------------------------------------------------------------------------
# Dependances et règles pour les exécutables
#---------------------------------------------------------------------------------

lsrec.exe : lsrec.o util.o
	$(CC) -o lsrec.exe lsrec.o util.o

mycp1.exe : mycp1.o util.o
	$(CC) -o mycp1.exe mycp1.o util.o

mycp2.exe : mycp2.o util.o
	$(CC) -o mycp2.exe mycp2.o util.o

#---------------------------------------------------------------------------------
# Tests de lsrec et mycp[12]
#---------------------------------------------------------------------------------

# Tests fonctionnels
tests : all
	tst_lsrec.sh Tests
	tst_mycp1.sh Tests
	tst_mycp2.sh Tests

# Influence de la taille du buffer dans la copie
perf : all
	tst_perf.sh Tests ../mycp1.exe 100 200 500 1000 2000 5000 10000 50000 100000

#---------------------------------------------------------------------------------
# Dependances individuelles (obtenues par 'gcc -MM *.c')
#---------------------------------------------------------------------------------

util.o : util.h
lsrec.o mycp1.o mycp2.o : util.h

#---------------------------------------------------------------------------------
# Installations diverses (locale et répertoire étudiant de l'ESSI)
#---------------------------------------------------------------------------------

install : all 
	@echo "*** Rien à installer ***"

install_ESSI :
	make install INSTALL_DIR=$(INSTALL_ESSI)
