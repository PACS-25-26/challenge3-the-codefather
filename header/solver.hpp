#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "params.hpp"

double serial_solver(parameters p);
double omp_solver(parameters p);
double mpi_solver(parameters p, int rank, int size);
double hybrid_solver(parameters p, int rank, int size);

#endif SOLVER_HPP