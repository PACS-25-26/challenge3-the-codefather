#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "params.hpp"
#include <iostream>

// Forward declarations of solver functions
double serial_solver(parameters p);

// The hybrid solver will be called by all ranks, but only the master rank will print the final error and timing information
double hybrid_solver(parameters p, int rank, int size);

#endif //SOLVER_HPP