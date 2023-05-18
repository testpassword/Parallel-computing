N1=1600  # N1 and N2 was found in lab2
N2=526999
Dn=$(echo "scale=0; ($N2 - $N1) / 10" | bc -l)
Nx=4000  # Nx was found manually for this lab

FROM=$N1
TO=$N2


test_perf() {
    # $1: mode[MP|NOMP], $2: array size of program
    measure_perf() {
        ."/lab5_$1" $2 100 | tail -n1
    }
    PERF="perf.csv"
    echo -n "N,generate_MP,map_MP,merge_MP,sort_MP,reduce_MP,generate_NOMP,map_NOMP,merge_NOMP,sort_NOMP,reduce_NOMP" >> $PERF
    echo >> $PERF
    for (( I=$FROM; I<=$TO; I+=$Dn )); do
        echo -n $I >> $PERF
        echo -n ",$(measure_perf MP $I)" >> $PERF
        echo -n "," >> $PERF
        echo -n ",$(measure_perf NOMP $I)" >> $PERF
        echo >> $PERF
    done
}

echo "task for mark 3 and 4 started..."
test_perf
echo "perfomance results saved"
