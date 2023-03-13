#!/bin/bash
THREADS=(1 2 3 4)

build_seq_gcc() {
    gcc -O3 -Wall -Werror -o lab1-seq src/*.c -lm
}

# $1: threads count
build_par_gcc() { 
    gcc -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=$1 src/*.c -o lab1-par-$1 -lm
}

build_seq_clang() {
    clang -O3 -Wall -Werror -pedantic -Wextra -o lab1-seq src/*.c -lm   
}

# $1: threads count
build_par_clang() {
    clang -O3 -Wall -Werror -pedantic -Wextra -mllvm -polly -mllvm -polly-parallel -mllvm -polly-omp-backend=LLVM -mllvm -polly-num-threads=$1 src/*.c -o lab1-par-$1 -lm
}

clean_builds() {
    rm lab1-seq
    for I in ${THREADS[@]}; do
        rm lab1-par-$I
    done
}

# $1: array size of program
measure_perf_seq() {
    ./lab1-seq $1 | awk '{ print $NF; }'
}

# $1: array size of program, $2: threads
measure_perf_par() {
    ./lab1-par-$2 $1 | awk '{ print $NF; }'
}

# $1: max execution time
find_N() {
    N=0
    while : ; do
        (( N += 1000 ))
        EXEC_TIME=$(measure_perf_seq $N)
        if (( $(echo "$EXEC_TIME > $1" | bc -l) )); then
            break
        fi
    done
    echo $N
}

# $1: min iters, $2: max iters, $3: step, $4: output filename
run_perf_suite() {
    echo -n "N,seq" >> $4
    for I in ${THREADS[@]}; do
        echo -n ",par-$I" >> $4
    done
    echo >> $4
    for (( I=$1; I<=$2; I+=$3 )); do
        echo -n "$I," >> $4
        echo -n "$(measure_perf_seq $I)" >> $4
        for J in ${THREADS[@]}; do
            echo -n ",$(measure_perf_par $I $J)" >> $4
        done
        echo >> $4
    done
}

# $1: data filename
draw_plot() {
    NAME="${1%%.*}"
    gnuplot -e "
        set title '$NAME autoparallel perfomance';
        set ylabel 'time (ms)';
        set xlabel 'array size';
        set key autotitle columnheader;
        set terminal jpeg size 1280,720;
        set datafile separator ',';
        set output '$NAME.jpeg';
        plot '$1' using 1:2 with lines, '$1' using 1:3 with lines, '$1' using 1:4 with lines, '$1' using 1:5 with lines, '$1' using 1:6 with lines;
    "
}

# $1: build_seq function, $2: build_par function, $3 filepattern
test_for_compiler() {
    ($1)
    for I in ${THREADS[@]}; do
        ($2 $I)
    done
    N1=$(find_N 10)  # 0.01s == 10ms
    echo "N1 = $N1 for $3 found"
    N2=$(find_N 5000)  # 5s == 5000ms
    echo "N1 = $N2 for $3 found"
    DNf=$(echo "($N2 - $N1) / 10" | bc -l)
    DN=$(printf '%.f' $DNf)
    run_perf_suite $N1 $N2 $DN $3.csv
    draw_plot $3.csv   
    clean_builds
}


test_for_compiler build_seq_gcc build_par_gcc gcc
echo "gcc test done"
test_for_compiler build_seq_clang build_par_clang clang
echo "clang test done"
