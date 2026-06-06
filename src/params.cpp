#include "params.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <omp.h>
#include "muParser.h"

// Helper function to create an OpenMP-safe lambda for muParser
std::function<double(double, double)> create_parser(const std::string& expr) {
    int max_threads = omp_get_max_threads();
    
    // Structure to hold independent parsing states for each thread
    struct ThreadParser {
        mu::Parser p;
        double x;
        double y;
    };
    
    // Allocate a vector of parsers (one per potential OpenMP thread)
    auto parsers = std::make_shared<std::vector<ThreadParser>>(max_threads);
    
    for (int i = 0; i < max_threads; ++i) {
        (*parsers)[i].p.DefineVar("x", &(*parsers)[i].x);
        (*parsers)[i].p.DefineVar("y", &(*parsers)[i].y);
        (*parsers)[i].p.DefineConst("pi", M_PI); // Add standard constants
        (*parsers)[i].p.SetExpr(expr);
    }

    // Return a lambda that captures the shared vector by value.
    // Each thread modifies only its own parser state.
    return [parsers](double x, double y) -> double {
        int tid = omp_get_thread_num();
        auto& tp = (*parsers)[tid];
        tp.x = x;
        tp.y = y;
        return tp.p.Eval();
    };
}

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
        
        else if (str == "--algo") {
            std::string alg = argv[++i];
        if (alg == "JACOBI")   p.algo_type = POINT_JACOBI;
        else if (alg == "SCHWARZ") p.algo_type = SCHWARZ;
        }
        else if (str == "--case") {
            std::string tc = argv[++i];
            if (tc == "SINUSOIDAL") p.test_case = SINUSOIDAL;
            else if (tc == "EXPONENTIAL") p.test_case = EXPONENTIAL;
            else if (tc == "POLYNOMIAL") p.test_case = POLYNOMIAL;
            else if (tc == "CUSTOM") p.test_case = CUSTOM;
        }
        
        else if (str == "--eq_f") {
            p.f = create_parser(argv[++i]);
            p.test_case = CUSTOM; // Automatically switch to custom to prevent overrides
        }
        else if (str == "--eq_g") {
            p.g = create_parser(argv[++i]);
            p.test_case = CUSTOM;
        }
        else if (str == "--eq_alpha") {
            p.alpha = create_parser(argv[++i]);
            p.test_case = CUSTOM;
        }
        else if (str == "--eq_uex") {
            p.u_ex = create_parser(argv[++i]);
            p.test_case = CUSTOM;
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