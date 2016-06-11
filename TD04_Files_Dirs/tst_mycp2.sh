#! /bin/sh -v
#---------------------------------------------------------------------------------
# Primitives de POSIX.1 : Manipulations de fichiers et répertoires
#---------------------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Mars 2005
# $Id: tst_mycp2.sh,v 1.1 2005/03/25 16:17:45 jpr Exp $
# --------------------------------------------------------------------------------
# Test de la copie de fichiers avec options -v, -i et -r.
#---------------------------------------------------------------------------------

#
# Répertoire et commande de test
Tests=${1-Tests}
mycp=${2-../mycp2.exe}

# Tests communs à mycp1 et mycp2 (non régression)
#------------------------------------------------

tst_mycp1.sh $Tests ../mycp2.exe

# Tests spécifiques à mycp2 
#--------------------------

cd $Tests

# Copies
$mycp -i foo bar
$mycp -v foo bar
$mycp -v -i foo bar
$mycp -r foobar barfoo FOO FOOBAR
$mycp -r -i -v foobar barfoo FOO FOOBAR

exit 0
