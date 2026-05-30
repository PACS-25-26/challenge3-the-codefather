#include <mpi.h>
#include <omp.h>
#include <iostream>
#include "solver.hpp"

//--------------------------------
//     Type of Parallelization
//-------------------------------
enum Parallel_mode{MPI,OMP,HYBRID};

int main(int argc,char **argv){

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

    //------------------------
    // Possible MPI call
    //------------------------

    int rank = 0;
    int size = 1;

    if(m == MPI || m == HYBRID){
        MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        // cannot initialize here rank and size because i need them also after
    }

    //---------------------
    // something for OMP
    //---------------------

    if(m == OMP || m == HYBRID)
    {
        //....
    }

    //-----------------------
    //        SOLVING
    //----------------------

    if(m == MPI)
    mpi_solver(n,tol,max_it,rank,size);

    if(m == OMP)
    omp_solver(n,tol,max_it);

    if(m == HYBRID)
    hybrid_solver(n,tol,max_it,rank,size);

    // -----------------------------
    //        Finalizing MPI 
    // -----------------------------
    if (m == MPI || m == HYBRID)
        MPI_Finalize();

    return 0;
}