#!/bin/bash

tries=10

cmd="mpirun -n $n ./tsp"

for n in 1 2 3 4
do
    printf "$n"
    for i in {0..10}
    do
        /usr/bin/time -f %e -o tempo $cmd < tsp.in > /dev/null
        TEMPO=$(cat ./tempo)
        rm ./tempo
        printf ", $TEMPO"
    done
    printf "\n"
done

