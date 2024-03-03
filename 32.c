#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

// Function declarations
void e(int error);
void Get_input(unsigned long long int* number_of_tosses, int my_rank, MPI_Comm comm);
unsigned long long int Monte_carlo(unsigned long long int local_number_of_tosses, unsigned int seed);

int main(int argc, char *argv[]) {
    // Variable declarations
    unsigned long long int number_of_tosses;
    unsigned long long int local_number_of_tosses;
    unsigned long long int number_in_circle;
    unsigned long long int local_number_in_circle;

    double pi_estimate;
    int my_rank;
    int comm_sz;
    MPI_Comm comm;

    // Initialize MPI environment
    e(MPI_Init(&argc, &argv));
    comm = MPI_COMM_WORLD;
    e(MPI_Comm_size(comm, &comm_sz));
    e(MPI_Comm_rank(comm, &my_rank));

    // Call function to get user input
    Get_input(&number_of_tosses, my_rank, comm);

    // Determine the number of tosses per process
    local_number_of_tosses = number_of_tosses / comm_sz;
    number_of_tosses = local_number_of_tosses * comm_sz;

    // Set a seed for random number generation based on rank
    unsigned int seed = (unsigned int)(my_rank + 1);

    // Perform Monte Carlo simulation
    local_number_in_circle = Monte_carlo(local_number_of_tosses, seed);

    // Reduce results from all processes
    e(MPI_Reduce(&local_number_in_circle, &number_in_circle, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, comm));

    // Print the final result on process 0
    if (my_rank == 0) {
        pi_estimate = 4 * ((double) number_in_circle) / ((double) number_of_tosses);
        printf("Number of Tosses: %llu\nNumber in Circle: %llu\nPi is approx: %f\n", number_of_tosses, number_in_circle, pi_estimate);
    }

    // Finalize MPI environment
    MPI_Finalize();
    return 0;
}

// Function to handle MPI errors
void e(int error) {
    if (error != MPI_SUCCESS) {
        fprintf(stderr, "Error starting MPI program, Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, error);
        MPI_Finalize();
        exit(1);
    }
}

// Function to get input from the user
void Get_input(unsigned long long int* number_of_tosses, int my_rank, MPI_Comm comm) {
    if (my_rank == 0) {
        printf("Count: ");
        scanf("%llu", number_of_tosses);
    }
    e(MPI_Bcast(number_of_tosses, 1, MPI_UNSIGNED_LONG_LONG, 0, comm));
}

// Function to perform Monte Carlo simulation
unsigned long long int Monte_carlo(unsigned long long int local_number_of_tosses, unsigned int seed) {
    unsigned long long int result = 0;
    double radius = 1.0;
    unsigned long long int i;
    srand(seed);
    for (i = 0; i < local_number_of_tosses; i++) {
        double x = ((double) rand()) / ((double) RAND_MAX) * radius * 2;
        double y = ((double) rand()) / ((double) RAND_MAX) * radius * 2;
        // Calculate distance from the center
        double dist = sqrt(x * x + y * y);
        // Check if the point is inside the circle
        if (dist <= 1.0) result++;
    }
    return result;
}