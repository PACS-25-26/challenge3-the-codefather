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
- the forcing term : $ f(x,y) = 8\pi^2 \sin(2\pi x)\sin(2\pi y) $
- the exact solution is:

$$
	u(x,y) = \sin(2\pi x)\sin(2\pi y)
$$


## main function

-
- 
- 
- 

## Project structure

```text
project/
├── src/
|    ├──main.cpp
|    ├──solver.cpp
|    ├──jacobi.cpp
|    ├──vtk_writer.cpp
|    ├──
├──header/
|    ├──solver.hpp
|    ├──jacobi.hpp
|    ├──vtk_writer.hpp
├── test/
|    ├──test
|    ├──RESULT.md
├──Makefile
└──README.md
```


