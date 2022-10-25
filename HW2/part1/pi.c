#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#define ll long long 

ll int point_sum = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void *monteCarlo(void *param){
    srand(time(NULL));
    ll int toss = *(int*) param;
    ll point_per_thread=0;

    for(int i=0; i<toss; i++){
        pthread_mutex_lock(&lock);

        double x = (double)rand()/(double)RAND_MAX;
        double y = (double)rand()/(double)RAND_MAX;
        double dis = x*x + y*y;

        if(dis <= 1.0)
            point_per_thread ++;


        pthread_mutex_unlock(&lock);
    }

}

int main(int argc, char *argv[]){       // argv: inputs from command line, argc: numbers of argument

    // convert arguments to int or long long int
    int num_thread = atoi(argv[0]);
    ll int num_toss = atoi(argv[1]);
    ll int toss_per_thread = num_thread/num_toss;

    pthread_t threads[num_thread];
    // threadData t_data[num_thread];

    for(int i=0; i<num_thread; i++){
        t_data[i].toss_per_thread = toss_per_thread;
        pthread_create(&threads[i], NULL, monteCarlo, (void *) &toss_per_thread);
    }

    for(int i=0; i<num_thread; i++)
        pthread_join(threads[i]);


    pthread_mutex_destroy(&lock);

    return 0;
}