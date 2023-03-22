CORES=16

# $1: array size of program, $2: threads
measure_perf_par() {
    ./lab2 $1 $2 | awk '{ print $NF; }'
}

# $1: data filename
draw_plot() {
    # todo: последную строку сконкотенировать
    NAME="${1%%.*}"
    gnuplot -e "
        set title 'AMD Framewave perfomance';
        set ylabel 'time (ms)';
        set xlabel 'array size';
        set key autotitle columnheader;
        set terminal jpeg size 1280,720;
        set datafile separator ',';
        set output '$NAME.jpeg';
        plot '$1' using 1:2 with lines, '$1' using 1:3 with lines, '$1' using 1:4 with lines, '$1' using 1:5 with lines, '$1' using 1:6 with lines;
    "
}

# $1: min iters, $2: max iters, $3: step, $4: output filename
run_perf_suite() {
    echo -n "N" >> $4
    for ((I=1; I<=CORES; I++)); do
        echo -n ",$I" >> $4
    done
    echo >> $4
    # for (( I=$1; I<=$2; I+=$3 )); do
    #     echo -n "$I," >> $4
    #     for ((J=1; J<=CORES; J++)); do
    #         echo -n ",$(measure_perf_par $I $J)" >> $4
    #     done
    #     echo >> $4
    # done
}

N1=1  # found in lab1
N2=1  # found in lab1
DNf=$(echo "($N2 - $N1) / 10" | bc -l)
DN=$(printf '%.f' $DNf)
run_perf_suite $N1 $N2 $DN amd_fw.csv
#draw_plot amd_fw.csv   