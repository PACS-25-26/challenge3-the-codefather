#include <mpi.h>
#include <omp.h>
#include <iostream>
#include "solver.hpp"
#include "params.hpp"

//--------------------------------
//     Type of Parallelization
//-------------------------------
//enum Parallel_mode{MPI,OMP,HYBRID};

int main(int argc,char **argv){

   

    parameters p=parse_args(argc,argv);
    
    //------------------------
    // Possible MPI call
    //------------------------

    int rank = 0;
    int size = 1;

    if( p.mode == HYBRID){
        MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        // cannot initialize here rank and size because i need them also after
    }

    //---------------------
    // something for OMP
    //---------------------

    if(p.mode == OMP || p.mode == HYBRID)
    {
        //....
    }

    //-----------------------
    //        SOLVING
    //----------------------

    if(p.mode == SERIAL)
    serial_solver(p);

    if(p.mode == OMP)
    omp_solver(p);

    if(p.mode == HYBRID)
    hybrid_solver(p,rank,size);

    // -----------------------------
    //        Finalizing MPI 
    // -----------------------------
    if ( p.mode == HYBRID)
        MPI_Finalize();


        //per ora ho tenuto divisione tra mpi omp e ibrido poi per ottimizzare si può tenere solo il caso ibrido che gestisce tutto in teoria:
        /*
        MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    parameters p = parse_args(argc, argv);

    // OMP: si controlla tramite variabile d'ambiente OMP_NUM_THREADS
    // MPI: si controlla da quante istanze lanci con mpirun -np X
    // Non serve un enum mode — è già implicito nel lancio

    hybrid_solver(p, rank, size);  // unica funzione, gestisce tutto

    MPI_Finalize();
    return 0;
        */

        //questo dovrebbe bastare

        // bash:
        /*
            # Seriale
            ./solver --n 128

            # Solo MPI
            mpirun -np 4 ./solver --n 128

            # Solo OMP
            OMP_NUM_THREADS=4 ./solver --n 128

            # Hybrid
            OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 128
        */

    return 0;
}



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