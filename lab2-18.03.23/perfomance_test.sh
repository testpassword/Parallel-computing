# N1 and N2 was found in lab1
N1=1600
N2=526999
DNf=$(echo "($N2 - $N1) / 10" | bc -l)
DN=$(printf '%.f' $DNf)


# $1: array size of program, $2: threads
measure_perf_par() {
    ./lab2 $1 $2 | awk '{ print $NF; }'
}

# $1: data filename, $2: from_cores, $3: to_cores
draw_plot() {
    # todo: последную строку сконкотенировать
    NAME="${1%%.*}"
    for (( I=$(( $2+1 )); I<=$3; I++)); do
		TEMPLATE+="'$1' using 1:$I with lines"
		if (( $I == $3 )); then
			TEMPLATE+=";"
		else
			TEMPLATE+=", "
		fi
	done
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

# $1: min iters, $2: max iters, $3: step, $4: output filename, $5: from_cores, $6 to_cores
run_perf_suite() {
    echo -n "N" >> $4
    for (( I=$5; I<=$6; I++ )); do
        echo -n ",$I" >> $4
    done
    echo >> $4
    for (( I=$1; I<=$2; I+=$3 )); do
        echo -n "$I" >> $4
        for ((J=$5; J<=$6; J++)); do
            echo -n ",$(measure_perf_par $I $J)" >> $4
        done
        echo >> $4
    done
}

mark3_task() {
    run_perf_suite $N1 $N2 $DN amd_fw3.csv 1 16
    draw_plot amd_fw3.csv 1 16
}

mark4_task() {
    run_perf_suite $N2 $N2 $DN amd_fw4.csv 16 32
    # ease to draw with Excel
}


echo "mark3_task() started..."
mark3_task
sleep 10  # make PC some time to relax
echo "
    mark3_task() done
    started mark4_task()...
"
mark4_task