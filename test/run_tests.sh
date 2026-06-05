#!/bin/bash

echo "=== Scalability Test ==="
echo "Running from project root: $(pwd)"
echo ""

echo "---------serial test---------"
echo "varing n"

for n in 16 32 64 128 256; do
    echo -n "n=$n: "
    mpirun -np 1 ../solver --n $n --tol 1e-6 --max_it 100000
done

echo "--------mpi scalability--------"
echo "varing np"

for np in 1 2 4; do
    echo "--- np=$np ---"
    time mpirun -np $np ../solver --n 128 --tol 1e-6 --max_it 800000
    echo ""
done

echo "-------hybrid scalability--------"
echo "varing both np and NUM_THREADS"

for np in 1 2 4; do #non ho abbastanza core per tutte queste combinazioni
    for threads in 1 2 4; do
        echo "--- np=$np | OMP_NUM_THREADS=$threads ---"
        OMP_NUM_THREADS=$threads time mpirun -np $np ../solver --n 128 --tol 1e-6 --max_it 800000
        echo ""
    done
done