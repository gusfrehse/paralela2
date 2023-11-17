#!/bin/bash

tries=10

for n in 1 2 3 4
do
    printf "$n"
    for i in tries
    do
        /usr/bin/time -f %e -o tempo mpirun -n $n ./tsp < tsp.in > /dev/null
        TEMPO=$(cat tempo)
        printf ", $TEMPO"
    done
    printf "\n"
done