#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int my_rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        printf("A correct input format: mpiexec -np < number_of_processes > ./task_2.out < N >\n");
        MPI_Finalize();
        return 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);  // make sure that all processes are ready to perform an algorithm
    double start = MPI_Wtime();

    int N = atoi(argv[1]);  // the total number of summands
    int n_first, n_last;  // limits of each process' work

    int N_1 = N / size;  // the number of summands for each process (maybe, except for the last one)
    if (my_rank != size - 1) {  // all processes, except for the last one, get an equal job
        n_first = my_rank * N_1 + 1;
        n_last = (my_rank + 1) * N_1;
    }

    else {  // the last process takes on all the other summands
        n_first = N_1 * (size - 1) + 1;
        n_last = N;
    }

    int n;  // a variable-counter
    double sum = 0.;  // a sum, calculated by each process
    for (n = n_first; n <= n_last; n++)
        sum += 1. / n;

    if (my_rank != 0)
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, 127, MPI_COMM_WORLD);
    if (my_rank == 0) {
        double tmp;
        int k;
        for (k = 1; k < size; k++) {
            MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 127, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += tmp;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);  // we try to make all processes finish their jobs simultaneously
    double end = MPI_Wtime();

    if (my_rank == 0)  // a zeroth process makes a conclusion
        printf("The final result: %lf; it took %lf seconds to calculate it.\n", sum, end - start);

    MPI_Finalize();
    return 0;
}