[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tKSbaXxd)
# challenge3: A matrix-free parallel solver for the Laplace equation

## The Problem 

For this project we have to implement a parallel solver for the laplace equation
on a unit square domain and Dirichlet condition on the boundaries.

The formulation:

$$
	-\Delta u = f(x,y), \qquad u|_{\partial \Omega} = 0.
$$

The numeric method we use is the **Jacobi iteration** method and parallelize by:
- MPI
- OpenMP
- Hybrid version

---

## Jacobi iteration

The main idea behind this method is an iterative formula :

$$
	U^{k+1}_{i,j} = \frac{1}{4}\left(U^k_{i-1,j} + U^k_{i+1,j} + U^k_{i,j-1} + U^k_{i,j+1}- h^2 f_{i,j}\right)
$$

where:
- the grid spacing is : $h = \frac{1}{n-1} $
- the forcing term : $f(x,y) = 8\pi^2 \sin(2\pi x)\sin(2\pi y)$
- the exact solution is:

$$
	u(x,y) = \sin(2\pi x)\sin(2\pi y)
$$

## 1. Features
* **Hybrid Parallelization:** Combines distributed memory scaling (MPI) with shared-memory multi-threading (OpenMP) for optimal compute efficiency.
* **Flexible Boundary Conditions:** Supports homogeneous/non-homogeneous **Dirichlet**, **Neumann**, and **Robin** boundary condition models.
* **Mathematical Test Cases:** Pre-configured analytical functions for strict benchmarking against exact solutions:
  * `SINUSOIDAL`: Perfect for Homogeneous Dirichlet.
  * `EXPONENTIAL`: Perfect for Non-Homogeneous Dirichlet.
  * `POLYNOMIAL`: Perfect for Non-Homogeneous Neumann or Robin.
* **VTK Output Generation:** Exports simulation solutions as `FLAT` (2D structured points) or `SURFACE` (3D topological surface) for immediate rendering in ParaView.

---

## 2. Prerequisites

Before building and running the suite, ensure your environment has the following software installed:
* **MPI Library:** OpenMPI or MPICH (`mpic++` compiler wrapper and `mpirun` runtime wrapper).
* **OpenMP Runtime Library:** Typically bundled with your C++ compiler (e.g., GCC/Clang).
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

```bash
mpirun -np <ranks> ./solver [arguments]
```

### Available Arguments:
* `--n <int>`: Grid size dimension $N \times N$ (default: 32).
* `--tol <double>`: Error tolerance for convergence (default: 1e-5).
* `--max_it <int>`: Maximum number of Jacobi iterations (default: 1000).
* `--mode <SERIAL|HYBRID>`: Execution mode (default: HYBRID).
* `--bc_type <DIRICHLET|NEUMANN|ROBIN>`: Physical boundary condition strategy (default: DIRICHLET).
* `--case <SINUSOIDAL|EXPONENTIAL|POLYNOMIAL>`: Mathematical test case (default: SINUSOIDAL).
* `--vtk_type <FLAT|SURFACE>`: Format for VTK export (default: SURFACE).

### Example Run:
```bash
mpirun -np 4 ./solver --n 128 --tol 1e-6 --max_it 5000 --mode HYBRID --bc_type ROBIN --case POLYNOMIAL --vtk_type SURFACE
```

---


## Project structure

```text
project/
├── src/
|    ├──main.cpp
|    ├──solver.cpp
|    ├──jacobi.cpp
|    ├──vtk_writer.cpp
|    ├──params.cpp
|    └──io.cpp
├──header/
|    ├──solver.hpp
|    ├──jacobi.hpp
|    ├──params.hpp
|    ├──vtk_writer.hpp
|    └──io.hpp
├── test/
|    ├──test
|    └──RESULT.md
├──Makefile
└──README.md
```





## 5. Automated Benchmarking Suite

An automated execution script is provided in the `test/` directory. It profiles the solver under two separate regimes:
1. **Serial Grid Scaling:** Measures performance trends as grid refinement levels double ($16 \times 16$ to $128 \times 128$).
2. **Hybrid Parallel Scaling:** Evaluates compute efficiency across combinations of MPI distributed memory ranks and OpenMP shared memory threads for a fixed $256 \times 256$ resolution.

### Running the Suite:
Make sure the bash script has permission to execute, then run it from the root directory:
```bash
chmod +x test/run_tests.sh
./test/run_tests.sh
```
*Note: The script automatically handles path management. Logs are saved in `test/data/` and hardware information in `test/hw.info`.*

---
