#!/bin/bash

tries=10


for p in 1 2 3 4
do
    echo $p
    for n in 10 11 12 13 14 15 16 17 18 19 20
    do
        printf "$n"
        cmd="mpirun -n $p ./tsp"
        cmd="echo $p"
        for i in {0..10}
        do
            /usr/bin/time -f %e -o tempo $cmd < tsp$n.in > /dev/null
            TEMPO=$(cat ./tempo)
            rm ./tempo
            printf ", $TEMPO"
        done
        printf "\n"
    done
done
