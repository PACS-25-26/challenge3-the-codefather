#include "solver.hpp"
#include "jacobi.hpp"
#include "io.hpp"

#include <mpi.h>
#include <omp.h>

//------------------------
//        MPI Solver
//------------------------
/*
double mpi_solver(parameters p, int rank, int size){
    
    int n=p.n;
    //------row division in ranks----------------
    int inner_row=n-2;
    int loc_row=inner_row/size;

    //in caso di numero di righe non multiplo di size:
    int extra_row = inner_row%size; //--->quanti rank avranno una riga in più 
    
    if (rank < extra_row)
        loc_row++;
    int first_row=1+rank*loc_row + (rank < extra_row ? rank : extra_row);
    int last_row =first_row+loc_row;
    
    // adding ghost row for jacobi solving (need i-1 and i+1 row)
    int ghost_row_top = (rank > 0) ? 1:0;
    int ghost_row_bottom = (rank < size-1) ? 1:0;
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
            //non sono sicuro di dove puntano righe in ordine sono 0: ghost 1:prima riga 2: seconda ----  devo inviare la riga 1 al rank precedente e da lui ricevere la riga 0 
            MPI_Sendrecv(M.row_ptr(1), n, MPI_DOUBLE, rank-1, 0,
                 M.row_ptr(0), n, MPI_DOUBLE, rank-1, 1,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if(rank<size-1){
            // same as before with last row (ghost one)
            //controllo anche qui se gli indici sono corretti. in fondo abbiamo righe: loc_row: ultima riga effettiva e loc_row+1: bottom ghost ------ qui devo inviare la riga loc_row e ricevere quella ghost 
            MPI_Sendrecv(M.row_ptr(loc_row), n, MPI_DOUBLE, rank+1, 1,
                 M.row_ptr(loc_row + 1), n, MPI_DOUBLE, rank+1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        double err=Jacobi_update(M,1,loc_row+1,p.f);// non considero la prima e l'ultima riga perchè boundary
        
         
        MPI_Allreduce(&err, &global_err, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    }

    return L2_err(M, p.u_ex);

}
    */

//----------------------------
//              OMP solver
//-----------------------------

double omp_solver(parameters p){

    // identica al seriale 
    int n=p.n;
    Matrix_Sol M(n,0,n);
    int it = 0;

    double err = 100.0;
    
    while(it<p.max_it && err > p.tol){
        ++it;
        err=Jacobi_update(M,1,n-1,p.f); // unica differenza è in jacobi su come agisce
    }

    //final parto for plotting?
    return L2_err(M,p.u_ex);

}

//----------------------------
//          HYBRID solver
//-----------------------------

double hybrid_solver(parameters p, int rank, int size) {
    int n = p.n;
    double h = 1.0 / (n - 1);

    // 1. Correct Load Balancing (without overwriting shared base variables)
    int total_internal_rows = n - 2;
    int base_loc_rows = total_internal_rows / size;
    int extra_row = total_internal_rows % size;

    int start_row_global = 1 + rank * base_loc_rows + std::min(rank, extra_row);
    int num_owned = base_loc_rows + (rank < extra_row ? 1 : 0);
    int end_row_global = start_row_global + num_owned;

    // Uniform memory allocation: Allocates exactly num_owned + 2 rows for EVERY rank.
    // Local row 0 is top ghost/boundary. Local row (num_owned + 1) is bottom ghost/boundary.
    Matrix_Sol M(n, start_row_global - 1, end_row_global + 1);

    int it = 0;
    double global_err = p.tol + 1.0;

    while (it < p.max_it && global_err > p.tol) {
        ++it;

        // 2. Clean, Uniform MPI Ghost Cell Exchange
        if (rank > 0) {
            // Send our first real row (1), receive into top ghost cell (0)
            MPI_Sendrecv(M.row_ptr(1), n, MPI_DOUBLE, rank - 1, 0,
                         M.row_ptr(0), n, MPI_DOUBLE, rank - 1, 1,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            // Send our last real row (num_owned), receive into bottom ghost cell (num_owned + 1)
            MPI_Sendrecv(M.row_ptr(num_owned), n, MPI_DOUBLE, rank + 1, 1,
                         M.row_ptr(num_owned + 1), n, MPI_DOUBLE, rank + 1, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // 3. Update local row space (always 1 to num_owned + 1 because bounds are exclusive)
        double local_sum = Jacobi_update(M, 1, num_owned + 1, p.f);

        // 4. Mathematically Correct Parallel Reduction
        double global_sum = 0.0;
        if (size > 1) {
            MPI_Allreduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        } else {
            global_sum = local_sum;
        }
        global_err = std::sqrt(global_sum * h);
    }

    // 5. Global L2 error check against analytical solution
    double local_l2 = L2_err(M, p.u_ex);
    double global_l2 = 0.0;
    if (size > 1) {
        MPI_Allreduce(&local_l2, &global_l2, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD); 
        // I think should be max not mpi sum as " Each processor checks its own local convergence criterion; then the local information
        //is exchanged among all ranks. Convergence is reached if all ranks satisfy the stopping
        //criterion"
    } else {
        global_l2 = local_l2;
    }
    double total_l2_error = std::sqrt(global_l2 * h);

    if (rank == 0) {
        std::cout << ">> Iterations: " << it << " | Final Global L2 Error: " << total_l2_error << "\n";
    }

    // Right before the solver finishes, export the data!
    export_to_vtk(M, p, rank, size, num_owned);


    return total_l2_error;

}



// just testing if it works

double serial_solver(parameters p){

    int n=p.n;
    Matrix_Sol M(n,0,n);
    long unsigned int it = 0;

    double err = 100.0;

    while(it<p.max_it && err > p.tol){
        ++it;
        err=Jacobi_update(M,1,n-1,p.f);// non considero la prima e l'ultima riga perchè boundary
    }

    //final parto for plotting?
    return L2_err(M,p.u_ex);

}