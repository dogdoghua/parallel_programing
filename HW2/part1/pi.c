#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#define ll long long 

unsigned ll int circle_sum = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct threadData{
    ll int toss;
    ll int start;
}threadData;

void *monteCarlo(void *param){
    threadData t_data = *(threadData *)param;
    unsigned int seed = time(NULL);
    // ll int toss = *(int*) t_data.toss;
    ll t_circle_sum=0;

    for(ll int i=t_data.start; i<(t_data.toss + t_data.start); i++){

        double x = (double)rand_r(&seed)/(double)RAND_MAX;
        double y = (double)rand_r(&seed)/(double)RAND_MAX;

        if(x*x + y*y <= 1.0)
            t_circle_sum++;

        
    }
    pthread_mutex_lock(&lock);
    
    circle_sum += t_circle_sum;
    // printf("---------\n%lld\n", circle_sum);

    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);

}

int main(int argc, char **argv){       // argv: inputs from command line, argc: numbers of argument
    double exe_time=0.0;
    clock_t begin = clock();

    // convert arguments to int or long long int
    int num_thread = atoi(argv[1]);
    ll int num_toss = atoi(argv[2]);
    ll int toss_per_thread = num_toss/num_thread;

    pthread_t* threads;
    threads = (pthread_t*)malloc(num_thread * sizeof(pthread_t));

    threadData t_data[num_thread];

    int state;
    // printf("number of thread%d number of toss:%lld, toss per thread%lld\n", num_thread, num_toss, toss_per_thread);
    for(int i=0; i<num_thread; i++){

        t_data[i].toss = toss_per_thread;
        t_data[i].start = i*toss_per_thread;
        // printf("create thread %d\n", i);
        state = pthread_create(&threads[i], NULL, monteCarlo, (void *) &t_data[i]);

        if(state){
            // printf("create error!\n");
            exit(-1);
        }
    }

    for(int i=0; i<num_thread; i++)
        pthread_join(threads[i], NULL);


    pthread_mutex_destroy(&lock);
    free(threads);
    clock_t end = clock();
    exe_time += (double)(end-begin) / CLOCKS_PER_SEC;
    // printf("total sum:%lld total toss:%lld\n", circle_sum, num_toss);
    printf("execution time is %fs\n", exe_time);
    printf("%7f\n", 4.0*(double)circle_sum/ (double)num_toss);

    return 0;
}