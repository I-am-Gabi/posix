#! /bin/sh

#------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: nderef.sh,v 1.2 2008/02/29 16:21:12 jpr Exp $
#------------------------------------------------------------------
# Script de test de l'effet de la période de dérérençage dans 
# la stratégie NUR 
#------------------------------------------------------------------

file=$1; shift
rm -f $file.gp $file

for nd in $*
do
    /bin/echo -n $nd 
    ../tst_Cache_NUR.exe -r 50 -t 5 -S -d $nd |\
 				awk '$1 ~ /hits/ {printf " %.2f\n", $2}'
done > $file

ed - << EOF
a
set style data linespoints
set xlabel "Période de déréférençage (nombre d'accès)"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet de la période de déréférençage (test 5)" font "Helvetica-Bold,18" at 150,60
set encoding utf8
set terminal postscript eps color
set output "$file.eps"
plot "$file" t "NUR"
.
w $file.gp
q
EOF

gnuplot $file.gp
