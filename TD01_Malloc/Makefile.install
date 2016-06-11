#------------------------------------------------------------------------------
# Exemple de Makefile pour les TDs de programmation système
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: Makefile.install,v 1.3 2008/11/23 17:46:53 jpr Exp $
#------------------------------------------------------------------------------
# Nous prennons ici l'exemple du TDO1 (Malloc).
#
# On suppose que le programme à construire (tst_mymalloc.exe) est obtenu à
# partir de deux programmes sources en C ; main_mymalloc.c (qui contient le
# programme de test) et mymalloc.c (qui contient le code du module à développer
# (implémentation de mymalloc() et myfree().
# 
# mymalloc.c inclut les deux fichiers d'en-tête mymalloc_impl.h et mymalloc.h,
# alors que main_mymalloc.c n'inclut que le second.
#
# L'utilisation est la suivante :
#	make		construit la cible principale
#	make clean	efface la cible et tous les fichiers temporaires
#			(pour repartir à 0)
#------------------------------------------------------------------------------

#------------------------------------------------------------------
# Variables
#------------------------------------------------------------------

# Compilateur
CC = gcc 

# Options de compilation : -g pour débugger, compatibilité ANSI et niveau de
# warning maximum 
CFLAGS = -g --std=c99 -Wall

#------------------------------------------------------------------
# Cibles principales
#------------------------------------------------------------------

all : tst_mymalloc1.exe tst_mymalloc2.exe

tst_mymalloc1.exe : main_mymalloc1.o mymalloc.o
	$(CC) -o tst_mymalloc1.exe main_mymalloc1.o mymalloc.o

tst_mymalloc2.exe : main_mymalloc2.o mymalloc.o
	$(CC) -o tst_mymalloc2.exe main_mymalloc2.o mymalloc.o

clean :
	-rm *.o *.exe

#------------------------------------------------------------------
# Dépendances spécifiques
#------------------------------------------------------------------

mymalloc.o : mymalloc.h mymalloc_impl.h
main_mymalloc1.o : mymalloc.h
main_mymalloc2.o : mymalloc.h
