#!/bin/bash

# Automatically locate the project root directory relative to this script
# (Moves up one level from project/test to project/)
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
cd "$ROOT_DIR"

echo "=== Different features Test Setup ==="
mkdir -p test/data_feature
mkdir -p output

echo "Extracting hardware info..."
lscpu > test/hw.info 2>/dev/null
echo "Hardware info saved to test/hw.info"

echo ""
echo "--------- 1. SCHWARZ TEST ---------"
   OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 256 --mode HYBRID --bc_type ROBIN --case POLYNOMIAL --algo SCHWARZ --inner_it 20 --vtk_type SURFACE | tee -a test/data/serial_runs.log

echo ""
echo "--------- 2. NEUMANN JACOBI TEST ---------"
   OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 256 --mode HYBRID --bc_type NEUMANN --case SINUSOIDAL --algo POINT_JACOBI --inner_it 20 --vtk_type SURFACE | tee -a test/data/serial_runs.log

echo ""
echo "--------- 3. CUSTOM  TEST ---------"
   OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 128 --mode HYBRID --bc_type DIRICHLET --eq_f "8 * pi^2 * sin(2 * pi * x) * sin(2 * pi * y)" --eq_g "0.0" --eq_uex "sin(2 * pi * x) * sin(2 * pi * y)" | tee -a test/data/serial_runs.log

echo ""
echo "--------- 4. CUSTOM DIRICHLET  TEST ---------"
echo "eq_f 8 * pi^2 * sin(2 * pi * x) * sin(2 * pi * y)"
   OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 256 --mode HYBRID --bc_type DIRICHLET --eq_f "8 * pi^2 * sin(2 * pi * x) * sin(2 * pi * y)" --eq_g "0.0" --eq_uex "sin(2 * pi * x) * sin(2 * pi * y)" | tee -a test/data/serial_runs.log
