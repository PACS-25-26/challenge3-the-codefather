#include <mpi.h>
#include <omp.h>
#include <iostream>
#include "solver.hpp"
#include "params.hpp"


int main(int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    parameters p = parse_args(argc, argv);

    if (rank == 0) {
        std::cout << "--- Starting Laplace Solver ---\n"
                  << "Grid size (n): " << p.n << "\n"
                  << "MPI Ranks:     " << size << "\n"
                  << "Max Iters:     " << p.max_it << "\n"
                  << "Mode:          " << (p.mode == SERIAL ? "SERIAL" : "HYBRID") << "\n"
                  << "Boundary Conditions:          " << (p.bc_type == DIRICHLET ? "DIRICHLET" : (p.bc_type == NEUMANN ? "NEUMANN" : "ROBIN")) << "\n"
                  << "-------------------------------\n";
    }

    double final_error = 0.0;
    double start_time = MPI_Wtime();

    if (p.mode == SERIAL) {
        if (rank == 0) final_error = serial_solver(p);
    } 
    else if (p.mode == HYBRID) {
        final_error = hybrid_solver(p, rank, size);
    }

    double end_time = MPI_Wtime();

    if (rank == 0) {
        std::cout << ">> Total Execution Time: " << (end_time - start_time) << " seconds.\n\n";
    }

    MPI_Finalize();
    return 0;
}

