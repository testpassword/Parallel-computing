#!/bin/bash

SCHEDULES=("static" "dynamic" "guided")
CHUNK_SIZES=(1 8 16 32)
FILES_TO_EXTENED=("FloatArray.c" "lab3.c")

# $1 executable name
compile() {
    gcc -O3 -Wall -o $1 src/*.c -lm -fopenmp
}

compile lab3-auto
for F in ${FILES_TO_EXTENED[@]}; do
    mv "src/$F" "$F:orig"
    for S in ${SCHEDULES[@]}; do
        for CS in ${CHUNK_SIZES[@]}; do
            TEMPLATE="
            #define SCHEDULE_TYPE $S
            #define CHUNK_SIZE $CS
            "
            echo "$TEMPLATE" | cat - "$F:orig"  > "src/$F"
            compile lab3-$S-$CS
        done
    done
    rm "$F:orig"
done
