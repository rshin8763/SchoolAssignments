/*********************************************************************
 ** Program Filename:  dynamicArray.c
 ** Author: Ryan Shin 
 ** Date: 4/23/17
 ** Description: Dynamic Array implmentation.
 ** Input: none
 ** Output: none
 *********************************************************************/
#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(){
    // Array of structs
    struct s {
        float value;
    } Array[4];

    omp_set_num_threads( NUMT );

    int someBigNumber = 100000000;

    //get time0
    double time0 = omp_get_wtime( );

    #pragma omp parallel for 
    for( int i = 0; i < 4; i++ ) {
        
        float privateArrayValue = Array[i].value;

        for( int j = 0; j < someBigNumber; j++ )
        {
            privateArrayValue += 2.;
        }

        Array[i].value = privateArrayValue;

    }
    //get time1
    double time1 = omp_get_wtime( );

    // calculate performance
    printf("%8.2lf Mega Calculations/Sec\n", 4*someBigNumber/1000000./(time1-time0));

    return 0;
}
