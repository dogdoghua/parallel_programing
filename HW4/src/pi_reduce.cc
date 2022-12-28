#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);     // the number of the process
    // long long int recv_buffer[world_size];
    int toss_per_process = tosses/world_size;
    long long int global_toss_in_circle=0;

    long long int local_sum=0;
    double x, y;
    unsigned seed = (unsigned)time(NULL) * (world_rank+1);
    for(int j=0; j<toss_per_process; j++){
        x = (double)rand_r(&seed)/(double)RAND_MAX;
        y = (double)rand_r(&seed)/(double)RAND_MAX;
        if(x*x + y*y<=1.0)
            local_sum++;
    }
    // printf("local_sum %lld in rank %d\n", local_sum, world_rank);
    
    // TODO: use MPI_Reduce
    MPI_Reduce(&local_sum, &global_toss_in_circle, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        // for(int i=0; i<world_size; i++){
        //     printf("recv local count %lld\n", recv_buffer[i]);
        //     global_toss_in_circle +=recv_buffer[i];
        // }
        // TODO: PI result
        // printf("global sum=%lld\n", global_toss_in_circle);
        pi_result = 4.0*(double)global_toss_in_circle/(double)tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
