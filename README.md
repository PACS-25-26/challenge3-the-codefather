[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tKSbaXxd)
# challenge3
The third challenge

\mainpage A matrix–free parallel solver for the Laplace equation

\section The Problem 

For this project we have to implement a parallel solver for the laplace equation
on a unit square domain and Dirichlet condition on the boundaries.

The formulation:
\f[
	-\Delta u = f(x,y), \qquad u|_{\partial \Omega} = 0.
\f]

\\
The numeric method we use is the Jacobi iteration method and parallelize by:
- MPI
- OpenMP
- Hybrid version

\section main function

-
- 
- 
- 

\section Project structure
'''
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
'''


