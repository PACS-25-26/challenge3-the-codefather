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
        }}

        /* i need mu parser for this, so for now i will ignore it
        // 4. Runtime Runtime Expression Functions via muParser
        else if (str == "--f") {
            p.f = make_muparser_closure(argv[++i]);
        } 
        else if (str == "--u_ex") {
            p.u_ex = make_muparser_closure(argv[++i]);
        } 
        else if (str == "--g") {
            p.g = make_muparser_closure(argv[++i]);
        } 
        else if (str == "--alpha") {
            p.alpha = make_muparser_closure(argv[++i]);
        }*/

    return p;
}