#include <mpi.h>
#include <omp.h>
#include <iostream>
#include "solver.hpp"
#include "params.hpp"


int main(int argc, char **argv) {
    // 1. Initialize MPI
    // (For Hybrid OpenMP/MPI, MPI_Init_thread is safer, but MPI_Init works for basic setups)
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 2. Parse arguments
    parameters p = parse_args(argc, argv);

    if (rank == 0) {
        std::cout << "--- Starting Laplace Solver ---\n"
                  << "Grid size (n): " << p.n << "\n"
                  << "MPI Ranks:     " << size << "\n"
                  << "Max Iters:     " << p.max_it << "\n"
                  << "-------------------------------\n";
    }

    // 3. Run the unified solver
    // The solver will handle the math, print the iterations, and export the VTK file.
    double final_error = hybrid_solver(p, rank, size);

    // 4. Clean up
    MPI_Finalize();
    return 0;
}

//--------------------------------
//     Type of Parallelization
//-------------------------------
//enum Parallel_mode{MPI,OMP,HYBRID};

    // OMP: si controlla tramite variabile d'ambiente OMP_NUM_THREADS
    // MPI: si controlla da quante istanze lanci con mpirun -np X
    // Non serve un enum mod — è già implicito nel lancio
    /*

    hybrid_solver(p, rank, size);  // unica funzione, gestisce tutto

    MPI_Finalize();
    return 0;
        

        //questo dovrebbe bastare

        // bash:
        
            # Seriale
            ./solver --n 128

            # Solo MPI
            mpirun -np 4 ./solver --n 128

            # Solo OMP
            OMP_NUM_THREADS=4 ./solver --n 128

            # Hybrid
            OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 128
        
    */


 /*

    spostato tutto in un file separato params (più comodo)
    //-----------------------------
    //       Reading input for:
    //          -   n : matrix size    
    //          -   tol: tollerance
    //          -   max_it: max iteration
    //          -   mode: Parallel_mode
    //------------------------------------

    // starting with default value:

    int n= 32;//or 33 if we count also ghost row...
    double tol=1e-5;
    int max_it=1000;
    Parallel_mode m = HYBRID;

    //possible reading terms:

    // knowing the command line:
    // mpirun -np 4 ./solver --n 256 --tol 1e-6 --max_it 50000 --mode MPI   
    // for examples
    for(int i=0;i<argc;i++){
        std::string str= argv[i];
        if(str == "--n") n = std::stoi(argv[++i]); //first insert then update
        if(str == "--tol") tol = std::stof(argv[++i]); 
        if(str == "--max_it") max_it = std::stoi(argv[++i]);
        if(str == "--mode"){
            std::string mode =argv[++i];
            if(mode == "MPI") m = MPI;
            if(mode == "OMP") m = OMP;
            if(mode == "HYBRID") m = HYBRID;
        }
    }

    */