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

    // TODO: init MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);     // the number of the process
    int local_toss_in_circle = 0;
    int global_toss_in_circle = 0;

    int name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &name_len);

    if (world_rank > 0)
    {   
        // TODO: handle workers
        int my_count = tosses/world_size;
        double x, y;
        unsigned seed = (unsigned)time(NULL) * (world_rank+1);
        
        local_toss_in_circle=0;
        for(int i=0; i<my_count; i++){
            x = (double)rand_r(&seed)/(double)RAND_MAX;
            y = (double)rand_r(&seed)/(double)RAND_MAX;

            if(x*x + y*y<=1.0)
                local_toss_in_circle++;
        }
        // printf("processor name: %s\n", processor_name);
        /*send result to master*/
        MPI_Send(&local_toss_in_circle, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: master
        int my_count = tosses/world_size;
        double x, y;
        unsigned seed = (unsigned)time(NULL) * (world_rank+1);
        
        local_toss_in_circle=0;
        for(int i=0; i<my_count; i++){
            x = (double)rand_r(&seed)/(double)RAND_MAX;
            y = (double)rand_r(&seed)/(double)RAND_MAX;

            if(x*x + y*y<=1.0)
                local_toss_in_circle++;
        }
        global_toss_in_circle += local_toss_in_circle;

        MPI_Status status;
        for(int i=world_size-1; i>=1; i--){
            MPI_Recv(&local_toss_in_circle, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            global_toss_in_circle += local_toss_in_circle;
        }
    }

    if (world_rank == 0)
    {
        // TODO: process PI result
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
