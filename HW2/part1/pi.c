#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#define ll long long 



int main(int argc, char *argv[]){       // argv: inputs from command line, argc: numbers of argument

    srand(time(NULL));
    
    // convert arguments to int or long long int
    int num_thread = atoi(argv[0]);
    ll int num_toss = atoi(argv[1]);
    ll int toss_per_thread = num_thread/num_toss;

    return 0;
}