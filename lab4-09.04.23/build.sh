#!/bin/bash
gcc -O3 -Wall -o lab4_MP src/*.c -lm -fopenmp -pthread
gcc -O3 -Wall -o lab4_NOMP src/*.c -lm -pthread