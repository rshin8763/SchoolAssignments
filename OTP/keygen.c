// Author: Ryan Shin
// 8/18/2017
// CS344- Assignment 4
// File: keygen.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Keygen creates a random string of characters from
// "ABCDEFGHIJKLMOPQRSTUVWXYZ " of length argv[1]
int main (int argc, char *argv[]){

    //Show usage error and format
    if (argc != 2){
        printf("USAGE: keygen number\n");
        exit (1);
    }

    int i, length;
    length = atoi(argv[1]);
    char key[length+1];
    char randC;

    //seed rng
    srand(time(NULL));


    //generate random string
    for (i=0; i<length; i++){
        randC = rand() % 27;
        if(randC == 26 ) randC = ' '; 
        else randC += 'A';
        key[i] = randC;
    }
    //terminate with null character
    key[length] = '\0';

    //print to stdout
    printf("%s\n", key);
} 
