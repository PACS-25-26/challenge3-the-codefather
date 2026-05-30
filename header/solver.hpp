#ifndef SOLVER_HPP
#define SOLVER_HPP

double serial_solver(int n, double tol, int maxiter);
double omp_solver(int n, double tol, int maxiter);
double mpi_solver(int n, double tol, int maxiter, int rank, int size);
double hybrid_solver(int n, double tol, int maxiter, int rank, int size);

#endif SOLVER_HPP