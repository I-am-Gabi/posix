#!/usr/bin/env bash

echo "-------- building ---------"
cmake .
make

cd bin/
echo

echo "run cp | [without arguments] | exemple1.txt -> exemple2.txt"
./cp_exe exemple1.txt exemple2.txt

echo "run cp | [without arguments] | exemple1.txt exemple2.txt exemple3.txt -> exemple1_dir"
./cp_exe exemple1.txt exemple2.txt exemple3.txt example_dir/

echo "run cp | -i | exemple1.txt -> exemple2.txt"
./cp_exe -i exemple1.txt exemple2.txt

echo "run cp | -i | exemple1.txt exemple2.txt exemple3.txt -> exemple1_dir"
./cp_exe -i exemple1.txt exemple2.txt exemple3.txt example_dir/

echo "run cp | -v | exemple1.txt -> exemple2.txt"
./cp_exe -v exemple1.txt exemple2.txt

echo "run cp | -v | exemple1.txt exemple2.txt exemple3.txt -> exemple1_dir"
./cp_exe -v exemple1.txt exemple2.txt exemple3.txt example_dir/

echo "run cp | -i -v | exemple1.txt -> exemple2.txt"
./cp_exe -i -v exemple1.txt exemple2.txt

echo "run cp | -i -v | exemple1.txt exemple2.txt exemple3.txt -> exemple1_dir"
./cp_exe -i -v exemple1.txt exemple2.txt exemple3.txt example_dir/

echo "run cp | -r | exemple1_dir exemple1_dir"
./cp_exe -r exemple1_dir/ exemple2_dir/

echo "run cp | -r -v | exemple1_dir exemple1_dir"
./cp_exe -r -v exemple1_dir/ exemple2_dir/