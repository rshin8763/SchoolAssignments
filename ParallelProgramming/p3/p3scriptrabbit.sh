#!/bin/bash

echo "RESULTS" > results.out
echo "=======" >> results.out

echo "FIX 1" >> results.out

for t in 1 2 4 
do
    echo THREADS=$t >> results.out
    for n in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
    do
        icpc -mmic -DNUM=$n -DNUMT=$t p3fix1.cpp -o p3fix1 -lm -openmp -no-vec -align -qopt-report=3 -qopt-report-phase=vec
        micnativeloadex ./p3fix1 >> results.out
    done
done

#newline
echo >> results.out

echo "FIX 2" >> results.out

for t in 1 2 4 
do
    echo THREADS=$t >> results.out
    icpc -mmic -DNUM=$n -DNUMT=$t p3fix2.cpp -o p3fix2 -lm -openmp -no-vec -align -qopt-report=3 -qopt-report-phase=vec
    micnativeloadex ./p3fix2 >> results.out
done

#newline
echo >> results.out
