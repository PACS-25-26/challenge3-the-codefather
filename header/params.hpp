#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <functional>
#include <cmath>

    //-----------------------------
    //       Reading input for:
    //          -   n : matrix size    
    //          -   tol: tollerance
    //          -   max_it: max iteration
    //          -   mode: Parallel_mode
    //------------------------------------

    //--------------------------------
    //     Type of Parallelization
    //-------------------------------
    enum Parallel_mode{MPI,OMP,HYBRID};

struct parameters{
    
    int n = 32;
    double tol =1e-5;
    int max_it=1000;
    Parallel_mode mode =HYBRID;

    // forcing term
    std::function<double(double,double)> f =[](double x,double y){
        return 8*M_PI*sin(2*M_PI*x)*sin(2*M_PI*y);
    };

    //exact solution for benchmarking
    std::function<double(double,double)> u_ex =[](double x,double y){
        return sin(2*M_PI*x)*sin(2*M_PI*y);
    };

};

parameters parse_args(int argc, char** argv);


#endif PARAMS_HPP