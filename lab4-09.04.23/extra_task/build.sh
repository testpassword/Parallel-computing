# Индивидуальное задание: проверить, есть ли разница при использвовании множества '#pragma omp parallel for' vs '#pragma omp parallel sections'

gcc -O3 -Wall -o extra *.c -lm -fopenmp
chmod +x extra