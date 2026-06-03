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
        for (int j = 0; j < n; ++j) {
            double xi = (global_row_start + i) * h;
            double yj = j * h;

            double diff = M.at(i, j) - u_ex(xi, yj);
            l2e += diff * diff;
        }
    }

    return l2e;
}