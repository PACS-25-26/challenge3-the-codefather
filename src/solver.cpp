#include "solver.hpp"
#include "jacobi.hpp"
#include "io.hpp"

#include <mpi.h>
#include <omp.h>

//----------------------------
//          SERIAL solver
//-----------------------------
double serial_solver(parameters p) {
    // Force OpenMP to use exactly 1 thread
    omp_set_num_threads(1); 
    
    int n = p.n;
    double h = 1.0 / (n - 1);
    
    Matrix_Sol M(n, -1, n); // Allocate n internal rows, +2 for ghosts (0 and n-1)
    init_boundaries(M, p, 0, 1);

    int it = 0;
    double global_err = p.tol + 1.0;

    while (it < p.max_it && global_err > p.tol) {
        ++it;
        double err = Jacobi_update(M, 1, n - 1, p.f);
        update_boundaries(M, p, 0, 1);
        global_err = std::sqrt(err * h);
    }

    double l2_error = std::sqrt(L2_err(M, p.u_ex) * h);
    std::cout << "[SERIAL] Iters: " << it << " | L2 Error: " << l2_error << "\n";
    
    export_to_vtk(M, p, 0, 1, n - 2);
    return l2_error;
}

//----------------------------
//              OMP solver
//-----------------------------
double omp_solver(parameters p) {
    // Relies on environment variable OMP_NUM_THREADS (handled in run script)
    int n = p.n;
    double h = 1.0 / (n - 1);
    
    Matrix_Sol M(n, -1, n);
    init_boundaries(M, p, 0, 1);

    int it = 0;
    double global_err = p.tol + 1.0;

    while (it < p.max_it && global_err > p.tol) {
        ++it;
        double err = Jacobi_update(M, 1, n - 1, p.f);
        update_boundaries(M, p, 0, 1);
        global_err = std::sqrt(err * h);
    }

    double l2_error = std::sqrt(L2_err(M, p.u_ex) * h);
    std::cout << "[OMP] Iters: " << it << " | L2 Error: " << l2_error << "\n";
    
    export_to_vtk(M, p, 0, 1, n - 2);
    return l2_error;
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

    //Initialize the non-homogeneous boundary functions!
    init_boundaries(M, p, rank, size);

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

        //Dynamically recalculate Neumann/Robin boundaries!
        update_boundaries(M, p, rank, size);

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
        //Each processor checks its own local convergence criterion; then the local information
        //is exchanged among all ranks. Convergence is reached if all ranks satisfy the stopping
        //criterion"
    } else {
        global_l2 = local_l2;
    }
    double total_l2_error = std::sqrt(global_l2 * h);

    if (rank == 0) {
        std::cout << "[HYBRID] Iters: " << it << " | Final Global L2 Error: " << total_l2_error << "\n";
    }

    // Right before the solver finishes, export the data!
    export_to_vtk_3d(M, p, rank, size, num_owned);


    return total_l2_error;

}

