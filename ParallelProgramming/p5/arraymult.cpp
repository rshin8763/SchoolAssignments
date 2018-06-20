#include <omp.h>
#include <stdio.h>
#include <math.h>
#include "simd.p5.h"

#define NUMTRIES 5
#define MAXARRAYSIZE 32000000
#define DATAPRINT 1

float A[MAXARRAYSIZE];
float B[MAXARRAYSIZE];
float C[MAXARRAYSIZE];

int main(){

    #ifndef _OPENMP
    fprintf(stderr, "OpenMP is not supported here.\n");
    return 1;
    #endif

    int startArraySize = 1000;
    int r = 2;


    if(!DATAPRINT)
    printf("size\tnonsimd\tnonsimd+reduction\tsimd\tsimd+reduction\n");
    for (int s = startArraySize; s <= MAXARRAYSIZE; s *= r ){

        //Nonsimd
        
        double maxMegaMults = 0.;
        double sumMegaMults = 0.;

        if (!DATAPRINT){
            printf("ARRAY SIZE: %d\n", s);
        } else {
            printf("%10d\t",s);

        }


        for(int t= 0; t < NUMTRIES; t++) {
            double time0 = omp_get_wtime();
            for (int i = 0; i < s; i++) {
                C[i] = A[i] * B[i];
            }
            double time1 = omp_get_wtime();
            double megaMults = (double)s/(time1-time0)/1000000.;
            sumMegaMults += megaMults;
            if(megaMults > maxMegaMults)
                maxMegaMults = megaMults;
        }

        double avgMegaMults = sumMegaMults/(double)NUMTRIES;
        if (!DATAPRINT){
            printf("NON-SIMD Array Mult\n", s);
            printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
            printf("Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults);
        } else{
            printf("%8.2lf\t", maxMegaMults);
        }

        //Nonsimd reduction
        maxMegaMults = 0.;
        sumMegaMults = 0.;

        double sum = 0; 

        for(int t= 0; t < NUMTRIES; t++) {
            double time0 = omp_get_wtime();
            for (int i = 0; i < s; i++) {
                sum += A[i] * B[i];
            }
            double time1 = omp_get_wtime();
            double megaMults = (double)s/(time1-time0)/1000000.;
            sumMegaMults += megaMults;
            if(megaMults > maxMegaMults)
                maxMegaMults = megaMults;
        }

        avgMegaMults = sumMegaMults/(double)NUMTRIES;
        if (!DATAPRINT){
            printf("NON-SIMD Array Mult + reduction\n", s);
            printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
            printf("Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults);
        } else{
            printf("%8.2lf\t", maxMegaMults);
        }

        // SIMD array mult
        maxMegaMults = 0.;
        sumMegaMults = 0.;

        for(int t= 0; t < NUMTRIES; t++) {
            double time0 = omp_get_wtime();
            SimdMul(A, B, C, s);
            double time1 = omp_get_wtime();
            double megaMults = (double)s/(time1-time0)/1000000.;
            sumMegaMults += megaMults;
            if(megaMults > maxMegaMults)
                maxMegaMults = megaMults;
        }
        avgMegaMults = sumMegaMults/(double)NUMTRIES;
        if (!DATAPRINT){
            printf("SIMD Array Mult\n", s);
            printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
            printf("Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults);
        } else{
            printf("%8.2lf\t", maxMegaMults);
        }

        // SIMD reduction
        maxMegaMults = 0.;
        sumMegaMults = 0.;

        for(int t= 0; t < NUMTRIES; t++) {
            double time0 = omp_get_wtime();
            SimdMulSum(A, B, s);
            double time1 = omp_get_wtime();
            double megaMults = (double)s/(time1-time0)/1000000.;
            sumMegaMults += megaMults;
            if(megaMults > maxMegaMults)
                maxMegaMults = megaMults;
        }
        avgMegaMults = sumMegaMults/(double)NUMTRIES;
        if (!DATAPRINT){
            printf("SIMD Array Mult + reduction\n", s);
            printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
            printf("Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults);
        } else{
            printf("%8.2lf\n", maxMegaMults);
        }
    }
    return 0;
}

