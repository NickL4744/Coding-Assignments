#include <mpi.h>
#include <math.h>

int main(int argc, char** argv) {
    int my_rank, comm_sz;
    int n = 100;
    int* local_data;
    int* recv_data;
    int i, j, k, step;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    local_data = (int*) malloc(n/comm_sz * sizeof(int));
    recv_data = (int*) malloc(n/comm_sz * sizeof(int));
    
    for (i = 0; i < n/comm_sz; i++) {
        local_data[i] = my_rank * n/comm_sz + i + 1;
    }
    
    for (step = 1; step < comm_sz; step *= 2) {
        if (my_rank % (2*step) == 0) {
            MPI_Recv(recv_data, n/comm_sz, MPI_INT, my_rank+step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (i = 0; i < n/comm_sz; i++) {
                local_data[i] += recv_data[i];
            }
        } else if (my_rank % step == 0) {
            MPI_Send(local_data, n/comm_sz, MPI_INT, my_rank-step, 0, MPI_COMM_WORLD);
        }
    }
    
    if (my_rank == 0) {
        int global_sum = 0;
        for (i = 0; i < n/comm_sz; i++) {
            global_sum += local_data[i];
        }
        printf("Global sum = %d\n", global_sum);
    }
    
    free(local_data);
    free(recv_data);
    MPI_Finalize();
    return 0;
}