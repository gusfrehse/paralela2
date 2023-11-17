#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ROOT 0

#include <mpi.h>


int main(int argc, char **argv)
{
    int my_rank, n_procs;
    //printf("hello world: %d\n", my_rank);
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    int *buff = malloc(sizeof(int) *  100);

    if (my_rank == ROOT) {
        for (int i = 0; i < 100; i++) {
            buff[i] = i * 10;
        }
    } else {
        for (int i = 0; i < 100; i++) {
            buff[i] = my_rank;
        }
    }

    MPI_Bcast(buff, 100, MPI_INT, ROOT, MPI_COMM_WORLD);

    fprintf(stderr, "my_rank: %d buff[30]: %d\n", my_rank, buff[30]);

    MPI_Finalize();
    return 0;
}
