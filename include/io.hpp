#ifndef IO_HPP
#define IO_HPP

#include "jacobi.hpp" // For Matrix_Sol
#include "params.hpp" // For parameters

// This function gathers data from all MPI ranks and writes the VTK file
void export_to_vtk(Matrix_Sol& M, parameters p, int rank, int size, int num_owned);

void export_to_vtk_3d(Matrix_Sol& M, parameters p, int rank, int size, int num_owned);

#endif