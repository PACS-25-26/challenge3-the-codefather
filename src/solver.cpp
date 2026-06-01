#include "solver.hpp"
#include "jacobi.hpp"

#include <mpi.h>
#include <omp.h>

//------------------------
//        MPI Solver
//------------------------
double mpi_solver(parameters p, int rank, int size){
    
    //......................................

}

//----------------------------
//              OMP solver
//-----------------------------

double omp_solver(parameters p){

}

//----------------------------
//          HYBRID solver
//-----------------------------

double hybrid_solver(parameters p, int rank, int size){

    //...........................
    
}

// just testing if it works

double serial_solver(parameters p){

    int n=p.n;
    Matrix_Sol M(n,0,n);
    int it = 0;
    double err = 100.0;
    while(it<p.max_it && err > p.tol){
        ++it;
        err=Jacobi_update(M,1,n-1,p.f);// non considero la prima e l'ultima riga perchè boundary
    }

    //final parto for plotting?
    return L2_err(M,p.u_ex);

}