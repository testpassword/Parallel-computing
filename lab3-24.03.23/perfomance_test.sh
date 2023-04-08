#!/bin/bash

# N1 and N2 was found in lab2
N1=1600
N2=526999
DNf=$(echo "($N2 - $N1) / 10" | bc -l)
DN=$(printf '%.f' $DNf)
SCHEDULES=("static" "dynamic" "guided")
CHUNK_SIZES=(1 8 16 32)

# $1: func which will be called inside loop
program_args_iterator() {
    for S in ${SCHEDULES[@]}; do
        for CS in ${CHUNK_SIZES[@]}; do
            $1
        done
    done
}

# $1: output_filename
write_header() {
    echo -n ",$S-$CS" >> $1
}

# $1: array size of program
measure_perf() {
    ./lab3-$S-$CS $1 | awk '{ print $NF; }'
}

# $1: from, $2: to, $3: step, $4: output filename
run_perf_suite() {
    PERF=$4
    echo -n "N" >> $PERF
    anon_write_header() {
        write_header $PERF
    }
    program_args_iterator anon_write_header
    echo -n ",auto" >> $PERF
    echo >> $PERF
    for (( I=$1; I<=$2; I+=$3 )); do
        echo -n $I >> $PERF
        anon_measure_perf() { 
            echo -n ",$(measure_perf $I)" >> $PERF
        }
        program_args_iterator anon_measure_perf
        echo -n ",$(./lab3-auto $I | awk '{ print $NF; }')" >> $PERF
        echo >> $PERF
    done
}


echo "task for mark 3 started..."
run_perf_suite $N1 $N2 $DN perf_N1_N2.csv
sleep 10 # make PC some time to relax
echo "
    task for mark 3 done
    started task for mark 4...
"
run_perf_suite 1 $N1 1 perf_1_N1.csv
echo "task for mark 4 done"