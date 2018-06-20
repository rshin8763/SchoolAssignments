#include <omp.h>
#include <stdio.h>
#include <math.h>

#define NUMTRIES 5


int main(){

    #ifndef _OPENMP
    fprintf(stderr, "OpenMP is not supported here.\n");
    return 1;
    #endif

    int startArraySize = 1024;
    int r = 4;
    int maxArraySize = 1024*1024*32;

    for (int s = startArraySize; s < maxArraySize; s *= r ){

    double maxMegaMults = 0.;
    double sumMegaMults = 0.;

    float A[s];
    float B[s];
    float C[s];

    for(int t= 0; t < NUMTRIES; t++) {
        double time0 = omp_get_wtime();
        #pragma omp parallel for
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
    printf("NON-SIMD Array Mult size: %d\n", s);
    printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
    printf("Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults);

    }
    return 0;
}

