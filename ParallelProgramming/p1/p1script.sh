#!/bin/bash

echo "RESULTS" > results.out
echo "=======" >> results.out

for t in 1 2 4 16
do
    for n in 16 32 64 128 256 512 1024 2048
    do
        echo NUMT=$t
        echo NUMNODES=$n
        /usr/local/common/gcc-7.3.0/bin/g++ -DNUMT=$t -DNUMNODES=$n p1.cpp -o p1 -lm -fopenmp
        ./p1 >> results.out
    done

    echo >> results.out
done
