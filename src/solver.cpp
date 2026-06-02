#include "solver.hpp"
#include "jacobi.hpp"

#include <mpi.h>
#include <omp.h>

//------------------------
//        MPI Solver
//------------------------
double mpi_solver(parameters p, int rank, int size){
    
    int n=p.n;
    //------row division in ranks----------------
    int inner_row=n-2;
    int loc_row=inner_row/size;

    //in caso di numero di righe non multiplo di size:
    // int extra_row = inner_row%size //--->quanti rank avranno una riga in più 
    /*
    if(rank < extra_row)
        loc_row++*/

    int first_row=1+rank*loc_row;// + (rank < extra_row ? rank : extra_row)
    int last_row =first_row+loc_row;
    
    // adding ghost row for jacobi solving (need i-1 and i+1 row)
    int ghost_row_top = (rank > 0) ? 1:0;
    int ghost_row_bottom = (rank < n-1) ? 1:0;
    //-------------------------------------------

    Matrix_Sol M(n,first_row-ghost_row_top,last_row+ghost_row_bottom);

    //----------solving iteration-----------------

    int it=0;
    double global_err=100.0;
    while(it<p.max_it && global_err > p.tol){
        ++it;

        //serve mandare e ricevere le righe ghost dai rank adiacenti;
        
        if(rank > 0){
            //sending to previous rank and recieving from previous rank
            //non sono sicuro di dove puntano 
            MPI_Sendrecv(M.row_ptr(1),   n, MPI_DOUBLE, rank-1, 0,
                         M.row_ptr(0),         n, MPI_DOUBLE, rank-1, 1,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if(rank<size-1){
            // same as before with last row (ghost one)
            //controllo anche qui se gli indici sono corretti
            MPI_Sendrecv(M.row_ptr(loc_row-1),  n, MPI_DOUBLE, rank+1, 1,
                         M.row_ptr(loc_row),     n, MPI_DOUBLE, rank+1, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        double err=Jacobi_update(M,1,n-1,p.f);// non considero la prima e l'ultima riga perchè boundary
        
         
        MPI_Allreduce(&err, &global_err, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    }

    return L2_err(M, p.u_ex);

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