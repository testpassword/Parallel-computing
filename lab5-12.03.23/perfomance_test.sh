N1=1600  # N1 and N2 was found in lab2
N2=526999
Dn=$(echo "scale=0; ($N2 - $N1) / 10" | bc -l)

FROM=$N1
TO=$N2

test_perf() {
    MODE=$1
    echo "test $MODE perf"
    CSV="perf_$MODE.csv"
    echo -n "N,generate,map,merge,sort,reduce,total" >> $CSV
    echo >> $CSV
    for (( I=$FROM; I<=$TO; I+=$Dn )); do
        echo "run $I"
        echo -n "$I,$(."/lab5_$MODE" $I 100 | tail -n1)" >> $CSV
        if [ "$MODE" == "NOMP" ]; then
            echo "sleep..."  # without sleep, Segmentation fault will be raised. I probably leaked memory, and with the help of sleep, the OS cleans up the memory for me
            sleep 3m
        fi
        echo >> $CSV
    done
}

test_perf MP
test_perf PTH
