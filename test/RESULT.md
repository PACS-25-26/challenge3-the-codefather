# Scalability Test Results

## 1. Hardware Environment
* **CPU Model:** AMD Ryzen 7 6800H (Virtualized via Microsoft Hypervisor)
* **Available Logical Processors:** 4 (2 Physical Cores + SMT)
* **Testing Conditions:** Grid sizes scaling from n=16 to n=256, with a fixed tolerance of 1e-6.

## 2. Mathematical Correctness
Across all parallel configurations, the solver consistently converged in exactly 25,671 iterations with a Final Global L2 Error of 0.00288879 for the $256 \times 256$ grid. This demonstrates that the parallel domain decomposition, ghost cell exchanges, and boundary condition updates are mathematically accurate and yield identical results regardless of the core or thread count.

## 3. Serial Performance (Grid Scaling)
The standalone serial execution shows the expected geometric growth in computational cost as the grid resolution doubles in both dimensions:

| Grid Size (N) | Iterations | Time (Seconds) |
| :--- | :---: | :---: |
| 16 | 186 | 0.0059 |
| 32 | 661 | 0.0242 |
| 64 | 2,187 | 0.3330 |
| 128 | 7,350 | 4.4620 |

## 4. Parallel & Hybrid Performance (N=256)
The table below summarizes the total execution time for the $256 \times 256$ grid across different combinations of MPI ranks (`np`) and OpenMP threads (`OMP_NUM_THREADS`):

| MPI Ranks (np) | OpenMP Threads | Total Processes/Threads | Time (Seconds) | Speedup (vs. Baseline) |
| :---: | :---: | :---: | :---: | :---: |
| 1 | 1 | 1 | 65.8542 | 1.00x (Baseline) |
| 1 | 2 | 2 | 38.3073 | 1.71x |
| 1 | 4 | 4 | 42.5397 | 1.54x |
| 2 | 1 | 2 | 39.0196 | 1.68x |
| 2 | 2 | 4 | 25.1168 | 2.62x |
| 2 | 4 | 8 | 27.6517 | 2.38x |
| 4 | 1 | 4 | 25.0285 | 2.63x |
| 4 | 2 | 8 | 16.8602 | 3.90x |

## 5. Performance Discussion & Findings

* **MPI vs. OpenMP at Equal Resource Counts:** When comparing runs utilizing exactly 2 hardware units, the shared-memory OpenMP approach (`np=1, OMP=2` at 38.30s) performed slightly faster than the distributed-memory approach (`np=2, OMP=1` at 39.01s). This shows that for small process counts, avoiding explicit MPI message-passing overhead across ghost cells offers a minor efficiency advantage.
* **The Hyperthreading Bottleneck:** Scaling from 2 execution units to 4 execution units (e.g., comparing `np=1, OMP=2` to `np=2, OMP=2`) does not result in a linear 2x speedup. Instead, the runtime drops from ~38.3s to ~25.1s (a ~1.5x improvement). This behavior matches the hypervisor hardware configuration: the environment has only 2 physical cores, meaning the 3rd and 4th logical processors are hyperthreads sharing hardware execution pipelines and caches.
* **Hardware Oversubscription Penalty:** When testing np=1 with OMP=4, the time worsened to 42.53 seconds. This indicates that allocating too many threads on restricted virtual hardware increases thread-management and cache-thrashing overhead, negating any parallel benefits. 
* **Optimal Configuration:** The fastest observed runtime was the highly dense configuration of 4 MPI ranks with 2 OpenMP threads each (16.86 seconds). While this technically oversubscribed the virtual environment, the specific chunking of the memory access allowed the CPU scheduler to pipeline the arithmetic highly efficiently.
