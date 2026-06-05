#include "io.hpp"

#include <mpi.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

void export_to_vtk(Matrix_Sol& M, parameters p, int rank, int size, int num_owned) {
    int n = p.n;
    double h = M.get_h();
    
    // Determine the exact number of rows this rank should send
    int rows_to_send = num_owned;
    int start_send_row = 1;

    if (rank == 0) {
        rows_to_send += 1;    // Include top boundary
        start_send_row = 0;   // Start from local row 0
    }
    if (rank == size - 1) {
        rows_to_send += 1;    // Include bottom boundary
    }

    int send_count = rows_to_send * n; 
    std::vector<int> recv_counts(size, 0);
    std::vector<int> displs(size, 0);

    // Collect sizing info
    MPI_Gather(&send_count, 1, MPI_INT, 
               recv_counts.data(), 1, MPI_INT, 
               0, MPI_COMM_WORLD);

    std::vector<double> global_U;
    if (rank == 0) {
        global_U.resize(n * n, 0.0); 
        displs[0] = 0; // Start at the absolute beginning of the array
        for (int i = 1; i < size; ++i) {
            displs[i] = displs[i - 1] + recv_counts[i - 1];
        }
    }

    // Gather solution data including active boundaries
    MPI_Gatherv(M.row_ptr(start_send_row), send_count, MPI_DOUBLE,
                global_U.data(), recv_counts.data(), displs.data(),
                MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Write file
    if (rank == 0) {
        std::string filename = "solution_n" + std::to_string(n) + ".vtk";
        std::ofstream vtk_file(filename);

        if (!vtk_file) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;}
        
        vtk_file << "# vtk DataFile Version 3.0\n"
                 << "Laplace Solver Result\n"
                 << "ASCII\n"
                 << "DATASET STRUCTURED_POINTS\n"
                 << "DIMENSIONS " << n << " " << n << " 1\n"
                 << "ORIGIN 0.0 0.0 0.0\n"
                 << "SPACING " << h << " " << h << " 0.0\n"
                 << "POINT_DATA " << n * n << "\n"
                 << "SCALARS u double 1\n"
                 << "LOOKUP_TABLE default\n";

        for (double val : global_U) {
            vtk_file << val << "\n";
        }
        vtk_file.close();
        std::cout << "[IO] Exported successfully to " << filename << std::endl;
    }
}