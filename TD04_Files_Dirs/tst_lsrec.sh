#! /bin/sh -v
#---------------------------------------------------------------------------------
# Primitives de POSIX.1 : Manipulations de fichiers et répertoires
#---------------------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Mars 2005
# $Id: tst_lsrec.sh,v 1.1 2005/03/25 16:17:45 jpr Exp $
# --------------------------------------------------------------------------------
# Test de lsrec (liste récursive)
#---------------------------------------------------------------------------------

# Mise en place des répertoires de test
set_tst_dir.sh Tests

echo "Test de lsrec.exe dans le répertoire courant"
lsrec.exe
