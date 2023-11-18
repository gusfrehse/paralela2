#include <stdio.h>

#include <mpi.h>

int main(int argc, char **argv)
{
    int my_rank;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    fprintf(stderr, "%d: before first barrier\n", my_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    fprintf(stderr, "%d: after first barrier and before second\n", my_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    fprintf(stderr, "%d: after second barrier\n", my_rank);

    MPI_Finalize();
    return 0;
}
