N1=1600  # N1 and N2 was found in lab2
N2=526999
Dn=$(echo "scale=0; ($N2 - $N1) / 10" | bc -l)
Nx=4000  # Nx was found manually for this lab

FROM=$(echo "scale=0; $Nx / 2" | bc -l)
FROM=$(($FROM < $N1 ? $FROM : $N1))
TO=$N2

# $1: output filename
mark3_task() {
    # $1: mode[MP|NOMP], $2: array size of program
    measure_perf() {
        ."/lab4_$1" $2 100 | tail -n1 | awk '{ print $NF; }'
    }
    PERF=$1
    echo -n "N,MP,NOMP" >> $PERF
    echo >> $PERF
    for (( I=$FROM; I<=$TO; I+=$Dn )); do
        echo -n $I >> $PERF
        echo -n ",$(measure_perf MP $I)" >> $PERF
        echo -n ",$(measure_perf NOMP $I)" >> $PERF
        echo >> $PERF
    done
}

# $1: output filename
mark4_task() {
    # $1: mode[MP|NOMP], $2: array size of program
    measure_perf() {
        ."/lab4_$1" $2 10 | tail -n1
    }
    PERF=$1
    echo -n "N,MP,NOMP" >> $PERF
    echo >> $PERF
    for (( I=$FROM; I<=$TO; I+=$Dn )); do
        echo -n $I >> $PERF
        echo -n ",$(measure_perf MP $I)" >> $PERF
        echo -n ",$(measure_perf NOMP $I)" >> $PERF
        echo >> $PERF
    done
    python3 confidence_interval.py $PERF
}

echo "task for mark 3 started..."
mark3_task 100_loops.csv
sleep 10 # make PC some time to relax
echo "
    task for mark 3 done
    started task for mark 4...
"
mark4_task 10_loops.csv
echo "task for mark 4 done"
