#---------------------------------------------------------------------------------
# Primitives de POSIX.1 : Siganux
#---------------------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Mars 2005
# $Id: Makefile,v 1.2 2005/04/12 17:51:59 jpr Exp $
# --------------------------------------------------------------------------------
# Makefile
#---------------------------------------------------------------------------------

# Compilateur et options
# ----------------------

CC = gcc
# Dans ces exercices, il est obligatoire d'éviter l'option -ansi de gcc, sinon
#	- changement de la sémantique de réception des signaux en cas 
#         d'interruption d'E/S par un signal
#	- nécessité de réarmement du handler
#       - sigaction inconnu
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=1

# Règle par défaut pour construire foo.exe à partir d'un seul fichier foo.c
# -------------------------------------------------------------------------

%.exe : %.c
	$(CC) $(CFLAGS) -o $@ $<

# Répertoires et fichiers pour l'installation des sujets
# ------------------------------------------------------

INSTALL_DIR = ../../Sujets
TD_DIR = Signals
TD_LNK = TD07
INSTALL_FILES = 
INSTALL_ESSI = $$HOME/NET4/TD_TP/Système/ESSI_1/Sujets

PROGS = tst_signal.exe tst_sigaction.exe tst_ignore.exe tst_children.exe \
	tst_exec_signal.exe tst_exec_signal-aux.exe \
	shell-system.exe station.exe ring.exe
EXTRA = 

#---------------------------------------------------------------------------------
# Cibles principales
#---------------------------------------------------------------------------------

# Exécutables et bibliothèques
# ----------------------------

all : $(PROGS) $(EXTRA)

# Nettoyages
# ----------

# Nettoyage léger : 
# conserve les exécutables
clean : all
	-rm -f \#* *.o foo

# Nettoyage en profondeur : 
# détruit tous les fichiers généré
clobber full_clean :
	-rm -f \#* *.o foo $(PROGS) 
	-rm -rf Tests
	-rm -rf *.dSYM

#---------------------------------------------------------------------------------
# Installations diverses (locale et répertoire étudiant de l'ESSI)
#---------------------------------------------------------------------------------

install : all 
	@echo "*** Rien à installer ***"

install_ESSI :
	make install INSTALL_DIR=$(INSTALL_ESSI)
