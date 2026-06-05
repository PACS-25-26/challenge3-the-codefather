#include "jacobi.hpp"
#include <omp.h>

double Jacobi_update(Matrix_Sol& M, int row_lo, int row_hi,
                     const std::function<double(double,double)>& f) 
{
    const double h = M.get_h();
    const int n = M.get_n();
    const int global_row_start = M.get_row_start();
    double iter_err = 0.0;

    // Maximizes shared-memory execution efficiency across threads
    #pragma omp parallel for reduction(+:iter_err) collapse(2)
    for (int i = row_lo; i < row_hi; ++i) {
        for (int j = 1; j < n - 1; ++j) {
            int iglo = global_row_start + i;
            
            double Uk = 0.25 * (M.at(i - 1, j) + M.at(i + 1, j) + 
                                M.at(i, j - 1) + M.at(i, j + 1) + 
                                f(iglo * h, j * h) * h * h);
            
            M.at_new(i, j) = Uk;

            double diff = Uk - M.at(i, j);
            iter_err += diff * diff; // Return raw squared sum for global MPI reduction
        }
    }

    M.swap_buffers();
    return iter_err; 
}

double L2_err(Matrix_Sol& M, const std::function<double(double,double)>& u_ex) {


    const double h = M.get_h();
    const int n = M.get_n();
    const int global_row_start = M.get_row_start();
    const int num_owned = M.local_rows() - 2; 
    double l2e = 0.0;

    #pragma omp parallel for reduction(+:l2e) collapse(2)
    for (int i = 1; i <= num_owned; ++i) {
        for (int j = 1; j < n-1; ++j) {
            double xi = (global_row_start + i) * h;
            double yj = j * h;

            double diff = M.at(i, j) - u_ex(xi, yj);
            l2e += diff * diff;
        }
    }

    return l2e;
}

void init_boundaries(Matrix_Sol& M, const parameters& p, int rank, int size) {
    if (p.bc_type != DIRICHLET) return;
    double h = M.get_h();
    int n = M.get_n();
    int global_start = M.get_row_start();
    int num_owned = M.local_rows() - 2;

    // Apply Left and Right borders for all ranks
    #pragma omp parallel for
    for (int i = 1; i <= num_owned; ++i) {
        double y = (global_start + i) * h;
        // Left boundary (j = 0)
        M.at(i, 0) = p.g(0.0, y);
        M.at_new(i, 0) = p.g(0.0, y);
        // Right boundary (j = n - 1)
        M.at(i, n - 1) = p.g(1.0, y);
        M.at_new(i, n - 1) = p.g(1.0, y);
    }

    // Apply Top border (only if you own the top global row)
    if (rank == 0) {
        #pragma omp parallel for
        for (int j = 0; j < n; ++j) {
            double x = j * h;
            M.at(0, j) = p.g(x, 0.0);
            M.at_new(0, j) = p.g(x, 0.0);
        }
    }

    // Apply Bottom border (only if you own the bottom global row)
    if (rank == size - 1) {
        #pragma omp parallel for
        for (int j = 0; j < n; ++j) {
            double x = j * h;
            M.at(num_owned + 1, j) = p.g(x, 1.0);
            M.at_new(num_owned + 1, j) = p.g(x, 1.0);
        }
    }
}


void update_boundaries(Matrix_Sol& M, const parameters& p, int rank, int size) {
    // Dirichlet boundaries do not change during Jacobi iterations
    if (p.bc_type == DIRICHLET) return;

    double h = M.get_h();
    int n = M.get_n();
    int global_start = M.get_row_start();
    int num_owned = M.local_rows() - 2;

    #pragma omp parallel for
    for (int i = 1; i <= num_owned; ++i) {
        double y = (global_start + i) * h;
        
        // Update Left boundary (uses internal node at j=1)
        double a_left = p.alpha(0.0, y);
        M.at(i, 0) = (M.at(i, 1) + h * p.g(0.0, y)) / (1.0 + h * a_left);

        // Update Right boundary (uses internal node at j=n-2)
        double a_right = p.alpha(1.0, y);
        M.at(i, n - 1) = (M.at(i, n - 2) + h * p.g(1.0, y)) / (1.0 + h * a_right);
    }

    // Top Neumann/Robin Update
    if (rank == 0) {
        #pragma omp parallel for
        for (int j = 0; j < n; ++j) {
            double x = j * h;
            double a_top = p.alpha(x, 0.0);
            M.at(0, j) = (M.at(1, j) + h * p.g(x, 0.0)) / (1.0 + h * a_top);
        }
    }

    // Bottom Neumann/Robin Update
    if (rank == size - 1) {
        #pragma omp parallel for
        for (int j = 0; j < n; ++j) {
            double x = j * h;
            double a_bot = p.alpha(x, 1.0);
            M.at(num_owned + 1, j) = (M.at(num_owned, j) + h * p.g(x, 1.0)) / (1.0 + h * a_bot);
        }
    }
}












