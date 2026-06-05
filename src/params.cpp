#include "params.hpp"
#include <cmath>

// Function to parse command-line arguments and populate the parameters struct
parameters parse_args(int argc, char** argv){
    parameters p;
    for (int i = 1; i < argc; ++i) {
        std::string str = argv[i];
        
        // 1. Basic Solver Parameters
        if (str == "--n") {
            p.n = std::stoi(argv[++i]);
        } 
        else if (str == "--tol") {
            p.tol = std::stod(argv[++i]);
        } 
        else if (str == "--max_it") {
            p.max_it = std::stoi(argv[++i]);
        } 

        else if (str == "--vtk_type") {
            std::string m = argv[++i];
            if (m == "FLAT")  p.vtk_type = FLAT;
            else if (m == "SURFACE")  p.vtk_type = SURFACE;
        }
        
        // 2. Parallelization Framework Choice
        else if (str == "--mode") {
            std::string m = argv[++i];
            if (m == "SERIAL")  p.mode = SERIAL;
            else if (m == "HYBRID")  p.mode = HYBRID;
        } 
        
        // 3. Boundary Condition Physics Strategy
        else if (str == "--bc_type") {
            std::string bc = argv[++i];
            if (bc == "DIRICHLET")  p.bc_type = DIRICHLET;
            else if (bc == "NEUMANN")    p.bc_type = NEUMANN;
            else if (bc == "ROBIN")      p.bc_type = ROBIN;
        }

        // 4. Mathematical Test Case
        else if (str == "--case") {
            std::string tc = argv[++i];
            if (tc == "SINUSOIDAL") p.test_case = SINUSOIDAL;
            else if (tc == "EXPONENTIAL") p.test_case = EXPONENTIAL;
            else if (tc == "POLYNOMIAL") p.test_case = POLYNOMIAL;
        }
    }


        // --- OVERRIDE LAMBDAS BASED ON TEST CASE ---
    
    if (p.test_case == EXPONENTIAL) {
        // Harmonic function (Delta u = 0)
        p.u_ex  = [](double x, double y) { return std::exp(x) * std::cos(y); };
        p.f     = [](double x, double y) { return 0.0; };
        p.g     = [](double x, double y) { return std::exp(x) * std::cos(y); };
        p.alpha = [](double x, double y) { return 1.0; }; // Example coefficient
    } 
    else if (p.test_case == POLYNOMIAL) {
        // Quadratic function
        p.u_ex  = [](double x, double y) { return (x * x) + (y * y); };
        p.f     = [](double x, double y) { return -4.0; };
        p.g     = [](double x, double y) { return (x * x) + (y * y); };
        p.alpha = [](double x, double y) { return 2.0; }; // Example coefficient
    }
    // Note: If test_case is SINUSOIDAL, it relies on the defaults already set in the struct.
    return p;
}