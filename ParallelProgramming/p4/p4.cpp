#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <omp.h>
#include <ctime>

#define TABLEPRINT 1

//STATE 
int    NowYear;        // 2017 - 2022
int    NowMonth;        // 0 - 11

float    NowPrecip;        // inches of rain per month
float    NowTemp;        // temperature this month
float    NowHeight;        // grain height in inches
int     NowNumPresents; 
int    NowNumDeer;        // number of deer in the current population


// Time steps
const float GRAIN_GROWS_PER_MONTH =        8.0;
const float ONE_DEER_EATS_PER_MONTH =        0.5;

const float AVG_PRECIP_PER_MONTH =        6.0;    // average
const float AMP_PRECIP_PER_MONTH =        6.0;    // plus or minus
const float RANDOM_PRECIP =            2.0;    // plus or minus noise

const float AVG_TEMP =                50.0;    // average
const float AMP_TEMP =                20.0;    // plus or minus
const float RANDOM_TEMP =            10.0;    // plus or minus noise

const float MIDTEMP =                40.0;
const float MIDPRECIP =                10.0;

// Function Prototypes
void GrainDeer();
void Grain();
void Watcher();
void MyAgent();
float SQR( float x );
float Ranf( unsigned int *seedp,  float low, float high );
int Ranf( unsigned int *seedp, int ilow, int ihigh );
void Print();

int main () {
    //Seed 
    srand(time(0));

    // starting values
    // starting date and time:
    NowMonth =    0;
    NowYear  = 2017;

    // starting state (feel free to change this if you want):
    NowNumDeer = 1;
    NowHeight =  1.;


    omp_set_num_threads(4);    // same as # of sections
#pragma omp parallel sections
    {
#pragma omp section
        {
            GrainDeer( );
        }

#pragma omp section
        {
            Grain( );
        }

#pragma omp section
        {
            Watcher( );
        }

#pragma omp section
        {
            MyAgent( );    // your own
        }
    }
    return 0;
}

void GrainDeer(){
    while( NowYear < 2023 ) {
        int tempNowNumDeer = NowNumDeer;

        if(NowHeight > NowNumDeer){
            tempNowNumDeer++;
        } else if (NowHeight < NowNumDeer){
            tempNowNumDeer--;
        }

        if (NowMonth == 11){
            int chosenDeer = NowNumPresents/20;
            tempNowNumDeer -= chosenDeer;
        }
        if( tempNowNumDeer < 0. ) tempNowNumDeer = 0.;


        // DoneComputing barrier:
#pragma omp barrier

        NowNumDeer = tempNowNumDeer;

        // DoneAssigning barrier:
#pragma omp barrier

        // DonePrinting barrier:
#pragma omp barrier
    }
}

void Grain(){
    while( NowYear < 2023 ) {
        //Calculate Grain height
        float tempNowHeight = NowHeight;
        float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
        float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );


        tempNowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
        tempNowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        if( tempNowHeight < 0. ) tempNowHeight = 0.;

        // DoneComputing barrier:
#pragma omp barrier

        NowHeight = tempNowHeight;

        // DoneAssigning barrier:
#pragma omp barrier

        // DonePrinting barrier:
#pragma omp barrier
    }
}

void Watcher(){
    while( NowYear < 2023 ) {
        // DoneComputing barrier:
#pragma omp barrier
        // DoneAssigning barrier:
#pragma omp barrier

        //Print State
        Print();

        //Temperature and Precipitation calculations
        float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

        float temp = AVG_TEMP - AMP_TEMP * cos( ang );
        unsigned int seed = rand()%10000;
        NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
        NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
        if( NowPrecip < 0. ) NowPrecip = 0.;

        NowMonth++; 
        if (NowMonth > 11) {
            NowMonth = 0;
            NowYear++;
        }
        // DonePrinting barrier:
#pragma omp barrier

    }
}

void MyAgent(){
    while( NowYear < 2023 ) {

        // Santa starts preparing presents in June
        // which will determine how many deer he needs
        int tempNowNumPresents = NowNumPresents;
        if (NowMonth > 5){
            tempNowNumPresents += rand()%25;
        }

        // If its December
        if (NowMonth == 11) {
            tempNowNumPresents = 0;
        }

        // DoneComputing barrier:
#pragma omp barrier
        NowNumPresents = tempNowNumPresents;

        // DoneAssigning barrier:
#pragma omp barrier

        // DonePrinting barrier:
#pragma omp barrier
    }
}

// Utility Functions
float SQR( float x ) {
    return x*x;
}

float Ranf( unsigned int *seedp,  float low, float high ) {
    float r = (float) rand_r( seedp );              // 0 - RAND_MAX

    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int Ranf( unsigned int *seedp, int ilow, int ihigh ) {
    float low = (float)ilow;
    float high = (float)ihigh + 0.9999f;
    return (int)(  Ranf(seedp, low,high) );
}
void Print() {
    //Convert Temp to Celsius
    float convertedTemp = (NowTemp-32)*5./9.;
    //Convert inches to cm
    float convertedHeight = NowHeight*2.54;
    float convertedPrecip = NowPrecip*2.54;

    if (TABLEPRINT == 1){
        printf("%8.2lf\t%8.2lf\t%8d\t%8.2lf%8d\t\n", convertedTemp, convertedPrecip, NowNumDeer, convertedHeight, NowNumPresents);
    } else {
        printf("YEAR %d MONTH %d\n", NowYear, NowMonth);
        printf("TEMP %f PRECIP %f", convertedTemp, convertedPrecip);
        printf("DEER %d GRAIN %f PRESENTS %d\n", NowNumDeer, convertedHeight, NowNumPresents);
    }
}
