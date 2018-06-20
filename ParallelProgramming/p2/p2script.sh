#!/bin/bash

echo "RESULTS" > results.out
echo "=======" >> results.out

for t in 1 2 4 8 16 32
do
    for c in 0 1
    do
        for s in 0 1
        do
            echo COARSE=$c
            echo STATIC=$c
            /usr/local/common/gcc-7.3.0/bin/g++ -DCOARSE=$c -DSTATIC=$s -DNUMTHREADS=$t p2.cpp -o p2 -lm -fopenmp
            ./p2 >> results.out
        done
    done
    #newline
    echo >> results.out
done
