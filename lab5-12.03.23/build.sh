#!/bin/bash
gcc -O3 -Wall -o lab5_MP src/*.c -w -lm -fopenmp -pthread
gcc -O3 -Wall -o lab5_NOMP src/*.c -w -lm -pthread