#!/bin/bash

SCHEDULES=("static" "dynamic" "guided")
CHUNK_SIZES=(1 8 16 32)
FILES_TO_EXTENED=("FloatArray.c" "lab3.c")

for F in ${FILES_TO_EXTENED[@]}; do
    mv "src/$F" "$F:orig"
    for S in ${SCHEDULES[@]}; do
        for CS in ${CHUNK_SIZES[@]}; do
            TEMPLATE="
            #define SCHEDULE_TYPE $S
            #define CHUNK_SIZE $CS
            "
            echo "$TEMPLATE" | cat - "$F:orig"  > "src/$F"
            gcc -O3 -Wall -Werror -o lab3-$S-$CS src/*.c -lm -fopenmp
        done
    done
    rm "$F:orig"
done
