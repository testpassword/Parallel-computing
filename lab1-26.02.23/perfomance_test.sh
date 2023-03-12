#!/bin/bash

function build_seq {
    gcc -O3 -Wall -Werror -o lab1-seq src/*.c -lm
}

# $1: threads count
function build_par { 
    gcc -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=$1 src/*.c -o lab1-par-$1 -lm
}

# $1: max execution time
function find_N {
    N=0
    while : ; do
        ((N++))
        EXEC_TIME=$(./lab1-seq $N | awk '{ print $NF; }')
        if (( $(echo "$EXEC_TIME > $1" | bc -l) )); then
            break
        fi
    done
    echo $N
}

build_seq
for (( I=1; I <= 4; ++I)); do  
    build_par $I
done
N1=$(find_N 10)  # 0.01s == 10ms
N2=$(find_N 5000)  # 5s == 5000ms
DN=$(echo "($N2 - $N1) / 10" | bc -l | xargs printf '%.f' $1)
echo $DN
