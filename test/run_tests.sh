#!/bin/bash

# Automatically locate the project root directory relative to this script
# (Moves up one level from project/test to project/)
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
cd "$ROOT_DIR"

echo "=== Scalability Test Setup ==="
mkdir -p test/data
mkdir -p output

echo "Extracting hardware info..."
lscpu > test/hw.info 2>/dev/null
echo "Hardware info saved to test/hw.info"

echo ""
echo "--------- 1. SERIAL TEST ---------"
echo "Varying grid size (n)"
for n in 16 32 64 128; do
    echo -n "n=$n: "
    mpirun -np 1 ./solver --n $n --tol 1e-6 --max_it 10000 --mode SERIAL | tee -a test/data/serial_runs.log
done

echo ""
echo "------- 2. HYBRID SCALABILITY --------"
echo "varing both np and NUM_THREADS for n=256"

for np in 1 2 4; do 
    for threads in 1 2 4; do
    # Prevent oversubscribing basic test setups
        if [ $((np * threads)) -le 8 ]; then 
            echo "--- np=$np | OMP_NUM_THREADS=$threads ---"
            OMP_NUM_THREADS=$threads mpirun -np $np ./solver --n 256 --tol 1e-6 --max_it 50000 --mode HYBRID | tee -a test/data/hybrid_np${np}_omp${threads}.log
        fi
    done
done

echo "Tests complete! Logs saved in test/data/"