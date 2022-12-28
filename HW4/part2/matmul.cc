#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <mpi.h>

#define MASTER 0
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr, int **a_mat_ptr, int**b_mat_ptr){
    int world_size, world_rank;
    int *ptr;
    
	//init MPI
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if(world_rank == MASTER){
        scanf("%d %d %d", n_ptr, m_ptr, l_ptr);

        int N=*n_ptr, M=*m_ptr, L=*l_ptr;
        // initial matrix size
        *a_mat_ptr = (int*)malloc(sizeof(int) *N *M);
        *b_mat_ptr = (int*)malloc(sizeof(int) *M *L);

        // get matrix number
        for(int n=0; n<N; n++){
            for(int m=0; m<M; m++){
                ptr = *a_mat_ptr + n*M + m;
                scanf("%d", ptr);
            }
        }

        for(int m=0; m<M; m++){
            for(int l=0; l<L; l++){
                ptr = *b_mat_ptr + m*L + l;
                scanf("%d", ptr);
            }
        }
    }
}

void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){
	//init MPI
    int world_size, world_rank;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int num_tasks,
        task_id,
        num_workers,
        source, dest, mes_type,
        rows_send,
        ave_rows, extra, offset;
    
    int N, M, L;
    num_workers = world_size-1;
    
    /*--------master task--------*/

    if(world_rank == MASTER){
        int *ans;
        ans = (int*)malloc(sizeof(int) *n *l);
        ave_rows = n/num_workers;
        extra = n%num_workers;
        offset = 0;
        mes_type = FROM_MASTER;

        /*send task to workers*/
        for(dest=1; dest<=num_workers; dest++){
            MPI_Send(&n, 1, MPI_INT, dest, mes_type, MPI_COMM_WORLD);
            MPI_Send(&m, 1, MPI_INT, dest, mes_type, MPI_COMM_WORLD);
            MPI_Send(&l, 1, MPI_INT, dest, mes_type, MPI_COMM_WORLD);

            rows_send = (dest <= extra) ? ave_rows+1 : ave_rows;
            MPI_Send(&offset, 1, MPI_INT, dest, mes_type, MPI_COMM_WORLD);
            MPI_Send(&rows_send, 1, MPI_INT, dest, mes_type, MPI_COMM_WORLD);
            MPI_Send(&a_mat[offset*m], rows_send*m, MPI_INT, dest, mes_type, MPI_COMM_WORLD);
            MPI_Send(&b_mat[0], m*l, MPI_INT, dest, mes_type, MPI_COMM_WORLD);

            offset += rows_send;
        }

        /*receive result form workers*/
        mes_type = FROM_WORKER;
        for(int i=1; i<=num_workers; i++){
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, mes_type, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows_send, 1, MPI_INT, source, mes_type, MPI_COMM_WORLD, &status);
            MPI_Recv(&ans[offset * l], rows_send*l, MPI_INT, source, mes_type, MPI_COMM_WORLD, &status);
        }

        /*print result matrix*/
        for(int i=0; i<n; i++){
            for(int j=0; j<l; j++){
                printf("%d ", ans[i*l + j]);
                // if(j!=l-1) printf(" ");
            }
            printf("\n");
        }
        free(ans);

    }

    /*--------workers task--------*/
    if(world_rank > MASTER){
        mes_type = FROM_MASTER;
        MPI_Recv(&N, 1, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD, &status);
        MPI_Recv(&M, 1, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD, &status);
        MPI_Recv(&L, 1, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD, &status);

        int *a = (int*)malloc(sizeof(int) *N *M);
        int *b = (int*)malloc(sizeof(int) *M *L);
        int *c = (int*)malloc(sizeof(int) *N *L);

        MPI_Recv(&offset, 1, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows_send, 1, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD, &status);
        MPI_Recv(&a[0], rows_send*M, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD, &status);
        MPI_Recv(&b[0], M*L, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD, &status);

        for(int k=0; k<L; k++){
            for(int r=0; r<rows_send; r++){
                c[r*L + k] = 0;

                for(int j=0; j<M; j++){
                    c[r*L + k] += a[r*M + j] * b[j*L + k]; 
                }
            }
        }

        mes_type = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD);
        MPI_Send(&rows_send, 1, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD);
        MPI_Send(&c[0], rows_send*L, MPI_INT, MASTER, mes_type, MPI_COMM_WORLD);
        
        free(a);
        free(b);
        free(c);
    }
}


// Remember to release your allocated memory
void destruct_matrices(int *a_mat, int *b_mat){
    int world_size, world_rank;

	//init MPI
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    if(world_rank == MASTER){
        free(a_mat);
        free(b_mat);
    }
}