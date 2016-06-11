#! /bin/sh -v
#---------------------------------------------------------------------------------
# Primitives de POSIX.1 : Manipulations de fichiers et répertoires
#---------------------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Mars 2005
# $Id: set_tst_dir.sh,v 1.1 2005/03/25 16:17:45 jpr Exp $
# --------------------------------------------------------------------------------
# Mise en place du répertoire de test pour tst_mycp[12].sh
#---------------------------------------------------------------------------------

# Répertoire de test
Tests=${1-Tests}

# Création du répertoire de test
if [ -d $Tests ]
then
    rm -rf $Tests
fi
mkdir $Tests

# Mise en place de fichiers et de répertoires
cp util.h $Tests/foo
cp util.c $Tests/bar
mkdir $Tests/FOO
cp util.h $Tests/FOO/foo
mkdir $Tests/FOO/BAR
cp util.h $Tests/FOO/BAR/foo
cp util.c $Tests/FOO/BAR/bar

# Affichage du contenu (avec le vrai ls)
ls -lR $Tests

exit 0


