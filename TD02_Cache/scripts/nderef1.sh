#! /bin/sh

#------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: nderef1.sh,v 1.2 2008/02/29 16:21:12 jpr Exp $
#------------------------------------------------------------------
# Script de test de l'effet de la période de déréférençage dans 
# la stratégie NUR  en fonction de la taille du cache :
# effet des grandes valeurs de nderef
#------------------------------------------------------------------

file=$1; shift
rm -f $file.gp $file

for nd in 5 10 20 50 100 150 200 250 300 350 400 500 700 1000 1500
do
    /bin/echo -n $nd
    for r in 1000 500 300 200 100 50 20 10
    do
        ../tst_Cache_NUR.exe -r $r -t 5 -S -d $nd |\
 				awk '$1 ~ /hits/ {printf " %.2f ", $2}'
    done
    echo
done > $file

cat > $file.gp <<EOF
set style data linespoints
set xlabel "Période de déréférençage (nombre d'accès)"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet de la période de déréférençage (test 5)" font "Helvetica-Bold,18" at 150,60
set encoding 
set terminal postscript eps color
set output "$file.eps"
plot "$file" using 1:2 t "r=1000", "$file" using 1:3 t "r=500", "$file" using 1:4 t "r=300", "$file" using 1:5 

gnuplot $file.gp
