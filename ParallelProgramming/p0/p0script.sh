#!/bin/bash

for t in 1 4
do
    echo NUMT=$t
    /usr/local/common/gcc-7.3.0/bin/g++ -DNUMT=$t p0.cpp -o p0 -lm -fopenmp
    ./p0
done

   
