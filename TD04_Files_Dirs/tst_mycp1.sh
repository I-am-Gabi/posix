#! /bin/sh -v
#---------------------------------------------------------------------------------
# Primitives de POSIX.1 : Manipulations de fichiers et répertoires
#---------------------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Mars 2005
# $Id: tst_mycp1.sh,v 1.1 2005/03/25 16:17:45 jpr Exp $
# --------------------------------------------------------------------------------
# Test de la copie simple de fichiers
#---------------------------------------------------------------------------------

# Répertoire et commande de test
Tests=${1-Tests}
mycp=${2-../mycp1.exe}

# Mise en place des répertoires de test
set_tst_dir.sh $Tests

# Tests communs à mycp1 et mycp2
#-------------------------------

cd $Tests

# Copies
$mycp foo foobar
$mycp bar barfoo
$mycp foo bar
$mycp foo FOO/foo1
$mycp foobar barfoo FOO

ls -lR .

# Copies impossibles
$mycp FOO BAR
$mycp foobar barfoo FOOBAR

exit 0
