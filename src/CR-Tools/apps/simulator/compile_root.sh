#!/bin/bash
echo -e "\n\n\n\n\n\n\n\n\n"

numfiles=$(echo $(($# - 1)))
filenames=$(echo $@ | cut -d ' ' -f-$numfiles)
echo compiling using:
echo g++ -c FILE $(root-config --cflags) -Wall -O -fPIC
echo ...
while [ $# -gt 1 ]
do
    echo compiling $1
    g++ -c $1 $(root-config --cflags) -Wall -O -fPIC
    shift
    echo -e "\n\n\n"
done
testing=$(echo $filenames | sed -e 's/.cc/.o/g')
echo
echo linking using:
echo g++ $testing -o $1 $(root-config --glibs) -Wall-O -fPIC 
echo ...
g++ $testing -o $1 $(root-config --glibs) -Wall-O -fPIC 
