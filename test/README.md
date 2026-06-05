# Parallel Laplace Numerical Solver & Scalability Analysis

This repository contains a hybrid MPI + OpenMP parallel implementation of a Laplace numerical solver using Jacobi iterations, alongside an automated benchmarking suite to evaluate performance scaling.

---

## 1. Prerequisites

Before running the evaluation suite, ensure your environment has the following software installed:
* **MPI Library:** OpenMPI or MPICH (`mpic++` compiler wrapper and `mpirun` runtime wrapper).
* **OpenMP Runtime Library:** Typically bundled with your C++ compiler (e.g., GCC).
* **Build Tools:** `make` utility.
* **Documentation Generator:** `doxygen` (optional, for building code documentation).

---

## 2. Compilation Instructions

To ensure a clean, optimized compilation state, run the following commands from your project root directory:

```bash
# Remove old object files and binaries
make clean

# Compile the project with high optimization flags (-O2) and OpenMP enabled
make all
```
## 3. How to Run the Automated Benchmarking Suite

An automated execution script is provided in the `test/` directory. It profiles the solver under two separate regimes:
* **Serial Grid Scaling:** Measures performance trends as grid refinement levels double ($16 \times 16$ to $128 \times 128$).
* **Hybrid Parallel Scaling:** Evaluates compute efficiency across combinations of MPI distributed memory ranks and OpenMP shared memory threads for a fixed $256 \times 256$ resolution.

### Step 1: Grant Execution Permissions
Make sure the bash script has permission to execute on your system:
```bash
chmod +x test/run_tests.sh
```

Run the Suite: 
Execute the script from either the project root directory or from inside the test/ folder. The script automatically handles path management to execute everything relative to the project root:

```bash
./test/run_tests.sh
```

The log results are saved in test/data. The hw.info in test

Use make distclean to remove data and hd.info
