#include <stdio.h>
#include <string.h>

#define STD_TAG 0

#include <mpi.h>


int main(int argc, char **argv)
{
    int my_rank, n_procs;
    MPI_Status status;
    char msg[100];
    printf("hello world\n");
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    if (my_rank != 0) {
        sprintf(msg, "rank: %d", my_rank);
        MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, 0, STD_TAG, MPI_COMM_WORLD);
    } else {
        for (int i = 1; i < n_procs; i++) {
            MPI_Recv(msg, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Proc %d: %s \n", status.MPI_SOURCE, msg);
        }
    }

    MPI_Finalize();
    return 0;
}
