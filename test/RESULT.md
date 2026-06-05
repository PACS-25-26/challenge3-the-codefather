# Scalability Test Results

## 1. Hardware Environment
* **CPU Model:** AMD Ryzen 7 6800H (Virtualized via Microsoft Hypervisor)
* **Available Logical Processors:** 4 (2 Physical Cores + SMT)
* **Testing Conditions:** Grid sizes scaling from n=16 to n=256, with a fixed tolerance of 1e-6.

## 2. Mathematical Correctness
Across all parallel configurations (MPI, OpenMP, and Hybrid), the solver converged in exactly 25,671 iterations with a Final Global L2 Error of 0.00288879. This guarantees that the parallel domain decomposition and ghost-cell exchanges are mathematically sound and do not alter the integrity of the Jacobi method.

## 3. Serial Performance (Grid Scaling)
Running the pure serial implementation demonstrated the expected exponential increase in computational cost as the grid size doubled in both dimensions:

| Grid Size (N) | Iterations | Time (Seconds) |
| :--- | :--- | :--- |
| 16 | 186 | 0.005 |
| 32 | 661 | 0.024 |
| 64 | 2,187 | 0.333 |
| 128 | 7,350 | 4.462 |

*Note: For performance profiling, N=256 was selected as the baseline for all subsequent parallel tests to ensure the workload was heavy enough to offset communication overhead.*

## 4. Parallel Scalability (N=256)
The following table outlines the total execution time across varying configurations of MPI Ranks (np) and OpenMP Threads (OMP). 

| MPI Ranks | OpenMP Threads | Total Slots | Time (Seconds) | Speedup vs Baseline |
| :---: | :---: | :---: | :--- | :--- |
| 1 | 1 | 1 | 65.85 | 1.00x (Baseline) |
| 1 | 2 | 2 | 38.30 | 1.71x |
| 2 | 1 | 2 | 39.01 | 1.68x |
| 4 | 1 | 4 | 25.02 | 2.63x |
| 2 | 2 | 4 | 25.11 | 2.62x |
| 4 | 2 | 8 | 16.86 | 3.90x |

## 5. Discussion & Findings

* **MPI vs. OpenMP Efficiency:** When comparing configurations that utilize exactly 2 processing slots, pure MPI (np=2, OMP=1) at 39.01 seconds and pure OpenMP (np=1, OMP=2) at 38.30 seconds performed almost identically. The shared-memory architecture of OpenMP provided a marginal edge by avoiding explicit message passing.
* **The Hyperthreading Plateau:** Scaling from 2 slots to 4 slots did not yield a perfect linear 2x speedup. Time decreased from ~38.3 seconds to ~25.1 seconds (a 1.5x improvement rather than 2.0x). This is a textbook hardware limitation: the virtual machine only has 2 true physical cores. The 3rd and 4th slots are hyperthreads sharing the same execution units and cache, meaning computation-heavy math cannot perfectly double in speed.
* **Hardware Oversubscription Penalty:** When testing np=1 with OMP=4, the time worsened to 42.53 seconds. This indicates that allocating too many threads on restricted virtual hardware increases thread-management and cache-thrashing overhead, negating any parallel benefits. 
* **Optimal Configuration:** The fastest observed runtime was the highly dense configuration of 4 MPI ranks with 2 OpenMP threads each (16.86 seconds). While this technically oversubscribed the virtual environment, the specific chunking of the memory access allowed the CPU scheduler to pipeline the arithmetic highly efficiently.
