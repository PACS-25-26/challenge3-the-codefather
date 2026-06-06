[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tKSbaXxd)
# challenge3: A matrix-free parallel solver for the Laplace equation

## The Problem 

For this project we have to implement a parallel solver for the laplace equation
on a unit square domain and Dirichlet condition on the boundaries.

The formulation:

$$-\Delta u = f(x,y), \qquad u|_{\partial \Omega} = 0.$$

The numeric method we use is the **Jacobi iteration** method and parallelize by:
- MPI
- OpenMP
- Hybrid version

---

## Jacobi iteration

The main idea behind this method is an iterative formula:

$$U^{k+1}_{i,j} = \frac{1}{4}\left(U^k_{i-1,j} + U^k_{i+1,j} + U^k_{i,j-1} + U^k_{i,j+1}- h^2 f_{i,j}\right)$$

where:
- the grid spacing is: $h = \frac{1}{n-1}$
- the forcing term: $f(x,y) = 8\pi^2 \sin(2\pi x)\sin(2\pi y)$
- the exact solution is:

$$u(x,y) = \sin(2\pi x)\sin(2\pi y)$$

## 1. Features
* **Hybrid Parallelization:** Combines distributed memory scaling (MPI) with shared-memory multi-threading (OpenMP) for optimal compute efficiency.
* **Advanced Iterative Solvers:** Supports standard **Point Jacobi** and overlapping **Block Jacobi (Schwarz-type)** iterations for accelerated local convergence.
* **Flexible Boundary Conditions:** Supports homogeneous/non-homogeneous **Dirichlet**, **Neumann**, and **Robin** boundary condition models.
* **Dynamic Mathematical Parsing:** Integrates `muparser` to allow users to define custom forcing terms, exact solutions, and boundary functions directly from the command line without recompiling.
* **Mathematical Test Cases:** Pre-configured analytical functions for strict benchmarking against exact solutions:
  * `SINUSOIDAL`: Perfect for Homogeneous Dirichlet.
  * `EXPONENTIAL`: Perfect for Non-Homogeneous Dirichlet.
  * `POLYNOMIAL`: Perfect for Non-Homogeneous Neumann or Robin.
  * `CUSTOM`: User-defined equations parsed at runtime via muparser.
* **VTK Output Generation:** Exports simulation solutions as `FLAT` (2D structured points) or `SURFACE` (3D topological surface) for immediate rendering in ParaView.

---

## 2. Prerequisites

Before building and running the suite, ensure your environment has the following software installed:
* **MPI Library:** OpenMPI or MPICH (`mpic++` compiler wrapper and `mpirun` runtime wrapper).
* **OpenMP Runtime Library:** Typically bundled with your C++ compiler (e.g., GCC/Clang).
* **muParser:** C++ math parsing library required for custom runtime equations.
  * Ubuntu/Debian: `sudo apt-get install libmuparser-dev`
  * macOS (Homebrew): `brew install muparser`
* **Build Tools:** `make` utility.
* **Documentation Generator:** `doxygen` (optional, for building code documentation).

---

## 3. Compilation & Build Instructions

A `Makefile` is provided to manage the compilation process. To ensure a clean, optimized compilation state, run the following commands from your project root directory:

```bash
# Remove old object files and binaries
make clean

# Compile the project with high optimization flags (-O2) and OpenMP enabled
make all
```

Additional Make targets available:
* `make docs`: Generates code documentation using Doxygen (saved in `docs/`).
* `make distclean`: Performs a deep clean, removing build artifacts, test data, and generated VTK outputs.

---

## 4. Usage & Command-Line Arguments

The compiled executable `solver` can be run manually with highly customizable parameters.
it is adviced to always use OMP_NUM_THREADS = numompthr to avoid letting the system decide an stall the program

```bash
OMP_NUM_THREADS mpirun -np <ranks> ./solver [arguments]
```

### Available Arguments:
* **Basic Config:**
  * `--n <int>`: Grid size dimension $N \times N$ (default: 32).
  * `--tol <double>`: Error tolerance for convergence (default: 1e-5).
  * `--max_it <int>`: Maximum number of outer iterations (default: 1000).
  * `--mode <SERIAL|HYBRID>`: Execution mode (default: HYBRID).
* **Algorithm Configuration:**
  * `--algo <JACOBI|SCHWARZ>`: Iterative algorithm type (default: JACOBI).
  * `--inner_it <int>`: Number of local inner iterations per outer communication step (for SCHWARZ, default: 15).
  * `--inner_tol <double>`: Local convergence tolerance threshold (for SCHWARZ, default: 1e-7).
* **Physics & Boundaries:**
  * `--bc_type <DIRICHLET|NEUMANN|ROBIN>`: Physical boundary condition strategy (default: DIRICHLET).
  * `--case <SINUSOIDAL|EXPONENTIAL|POLYNOMIAL|CUSTOM>`: Mathematical test case (default: SINUSOIDAL).
* **Custom Math Expressions (Requires `--case CUSTOM`):**
  * `--eq_f "<string>"`: Custom forcing term function $f(x,y)$.
  * `--eq_g "<string>"`: Custom boundary function $g(x,y)$.
  * `--eq_alpha "<string>"`: Custom Robin coefficient function $\alpha(x,y)$.
  * `--eq_uex "<string>"`: Custom exact solution $u(x,y)$ for L2 error benchmarking.
* **Output:**
  * `--vtk_type <FLAT|SURFACE>`: Format for VTK export (default: SURFACE).

### Example Runs:

**1. Block Jacobi (Schwarz) Iteration with Robin Boundaries:**
```bash
OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 128 --mode HYBRID --bc_type ROBIN --case POLYNOMIAL --algo SCHWARZ --inner_it 20 --vtk_type SURFACE
```

**2. Custom Equations via muParser (Dynamic Runtime Parsing):**
```bash
OMP_NUM_THREADS=2 mpirun -np 4 ./solver --n 64 --mode HYBRID --bc_type DIRICHLET --eq_f "8 * pi^2 * sin(2 * pi * x) * sin(2 * pi * y)" --eq_g "0.0" --eq_uex "sin(2 * pi * x) * sin(2 * pi * y)"
```

---

## Project structure

```text
project/
├── src/
│   ├── main.cpp
│   ├── solver.cpp
│   ├── jacobi.cpp
│   ├── params.cpp
│   └── io.cpp
├── include/
│   ├── solver.hpp
│   ├── jacobi.hpp
│   ├── params.hpp
│   └── io.hpp
├── test/
│   ├── run_tests.sh
│   └── RESULT.md
├── Makefile
└── README.md
```

## 5. Automated Benchmarking Suite

An automated execution script is provided in the `test/` directory. Our resault logs can be seen in `test/tes_result` and they are discussed in RESULT.md. It profiles the solver under two separate regimes:
1. **Serial Grid Scaling:** Measures performance trends as grid refinement levels double ($16 \times 16$ to $128 \times 128$).
2. **Hybrid Parallel Scaling:** Evaluates compute efficiency across combinations of MPI distributed memory ranks and OpenMP shared memory threads for a fixed $256 \times 256$ resolution.

### Running the Suite:
Make sure the bash script has permission to execute, then run it from the root directory:
```bash
chmod +x test/run_tests.sh
./test/run_tests.sh
```

### Running feature_tests:
this script was used to test the different features of the program:
```bash
chmod +x test/feature_tests.sh
./test/feature_tests.sh
```
