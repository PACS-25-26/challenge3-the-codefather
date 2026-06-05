#ifndef JACOBI_HPP
#define JACOBI_HPP

#include <cmath>
#include <functional>
#include <vector>
#include <omp.h>
#include <stdexcept>
#include "params.hpp"

class Matrix_Sol{

    private:

    int n; //dimension;
    //for parallelization;
    int row_start;
    int row_end;

    double h; // discretization 1/(n-1)

    std::vector<double> U; //solution at iter k;
    std::vector<double> U_new; // new iteration solution

    public:

    // constructor
    Matrix_Sol(int n,int row_start,int row_end) : n(n) ,row_start(row_start),row_end(row_end),U((row_end - row_start) * n, 0.0),
          U_new((row_end - row_start) * n, 0.0)
          {
            if (n <= 1)
            throw std::invalid_argument("n must be greater than 1");

            h = 1.0 / (n - 1);

          }

    void swap_buffers() 
        { 
            std::swap(U, U_new); 
        }

    // getter and setter:
    double& at(int local_row, int col) 
        {
         return U[local_row * n + col];
        }
    double  at(int local_row, int col) const 
        {
             return U[local_row * n + col]; 
        }
    double& at_new(int local_row, int col) 
        { 
             return U_new[local_row * n + col]; 
        }
    double at_new(int local_row, int col) const 
        { 
             return U_new[local_row * n + col]; 
        }

    int local_rows() const { return row_end - row_start; }
    
    int get_n()         const { return n; }
    int get_row_start() const { return row_start; }
    int get_row_end()   const { return row_end; }
    double get_h()      const { return h; }
   
    //altre possibili get o set per MPI...

    double* row_ptr(int local_row)     { return &U[local_row * n]; }

};

// main function:

double Jacobi_update(Matrix_Sol& M, int row_lo, int row_hi, const std::function<double(double,double)>& f);

//to compute L2 error against exact solution (for testing purposes, not used in main solver loop)
double L2_err(Matrix_Sol& M,const std::function<double(double,double)>& u_ex);

// Sets the initial boundary values based on p.g(x,y)
void init_boundaries(Matrix_Sol& M, const parameters& p, int rank, int size);

// Updates the boundaries dynamically for Neumann/Robin
void update_boundaries(Matrix_Sol& M, const parameters& p, int rank, int size);


#endif //JACOBI_HPP

