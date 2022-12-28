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
    
    int local_toss_in_circle = 0, global_toss_in_circle = 0;
    int toss_per_process = tosses/world_size;

    
    global_toss_in_circle = local_toss_in_circle;

    if (world_rank > 0)
    {
        // TODO: MPI workers
        double x, y;
        unsigned seed = (unsigned)time(NULL) * (world_rank+1);
        for(int j=0; j<toss_per_process; j++){
            x = (double)rand_r(&seed)/(double)RAND_MAX;
            y = (double)rand_r(&seed)/(double)RAND_MAX;
            if(x*x + y*y<=1.0)
                local_toss_in_circle++;
        }
        MPI_Send(&local_toss_in_circle, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        double x, y;
        unsigned seed = (unsigned)time(NULL) * (world_rank+1);
        for(int j=0; j<toss_per_process; j++){
            x = (double)rand_r(&seed)/(double)RAND_MAX;
            y = (double)rand_r(&seed)/(double)RAND_MAX;
            if(x*x + y*y<=1.0)
                local_toss_in_circle++;
        }
        global_toss_in_circle += local_toss_in_circle;

        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        MPI_Request requests[world_size-1];
        MPI_Status status[world_size-1];
        int recv_buffer[world_size];

        for(int i=1; i<world_size; i++){
            MPI_Irecv(&recv_buffer[i-1], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &requests[i-1]);
        }
        MPI_Waitall(world_size-1, requests, status);
        for(int i=1; i<world_size; i++){
            global_toss_in_circle += recv_buffer[i-1];
        }

    }

    if (world_rank == 0)
    {
        // TODO: PI result
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
