N1=1600  # N1 and N2 was found in lab2
N2=526999
Dn=$(echo "scale=0; ($N2 - $N1) / 10" | bc -l)
# TODO: find real
Nx=5  # Nx was found manually for this lab

FROM=$(echo "scale=0; $Nx / 2" | bc -l)
FROM=$(($FROM < $N1 ? $FROM : $N1))
TO=$N2

# $1: mode[MP|NOMP], $2: array size of program
measure_perf() {
    ."/lab4_$1" $2 100 | tail -n1 | awk '{ print $NF; }'
}

# $1: from, $2: to, $3: step, $4: output filename
mark3_task() {
    PERF_FILE=$4
    echo -n "N,MP,NOMP" >> $PERF_FILE
    echo >> $PERF_FILE
    for (( I=$1; I<=$2; I+=$3 )); do
        echo -n $I >> $PERF_FILE
        echo -n ",$(measure_perf MP $I)" >> $PERF_FILE
        echo -n ",$(measure_perf NOMP $I)" >> $PERF_FILE
        echo >> $PERF_FILE
    done
}

echo "task for mark 3 started..."
mark3_task $FROM $TO $Dn 100_loops.csv
sleep 10 # make PC some time to relax
echo "
    task for mark 3 done
    started task for mark 4...
"
#run_perf_suite $FROM $TO $Dn 10_loops.csv 10
#echo "task for mark 4 done"
