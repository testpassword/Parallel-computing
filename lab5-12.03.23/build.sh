#!/bin/bash
gcc -O3 -Wall -o lab5_MP src/*.c -lm -fopenmp -pthread
gcc -O3 -Wall -o lab5_NOMP src/*.c -lm -pthread