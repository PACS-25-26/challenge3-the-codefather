#include "params.hpp"
#include <cmath>

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

/*
// Factory function to generate thread-safe muParser closures
std::function<double(double, double)> make_muparser_closure(const std::string& expr_str) {
    return [expr_str](double x_val, double y_val) -> double {
        struct ParserState {
            mu::Parser parser;
            double x = 0.0;
            double y = 0.0;
        };
        
        // Thread-local cache map ensures separate thread execution contexts for OpenMP
        thread_local std::unordered_map<std::string, std::unique_ptr<ParserState>> thread_parsers;
        
        auto& state_ptr = thread_parsers[expr_str];
        if (!state_ptr) {
            state_ptr = std::make_unique<ParserState>();
            
            // Link local coordinate values to variables
            state_ptr->parser.DefineVar("x", &(state_ptr->x));
            state_ptr->parser.DefineVar("y", &(state_ptr->y));
            
            // Add user conveniences for math constants
            state_ptr->parser.DefineConst("pi", M_PI);
            state_ptr->parser.DefineConst("M_PI", M_PI);
            
            try {
                state_ptr->parser.SetExpr(expr_str);
            } catch (mu::Parser::exception_type& e) {
                std::cerr << "\n[muParser Compilation Error] Failed on expression: \"" 
                          << expr_str << "\"\nReason: " << e.GetMsg() << std::endl;
                throw std::runtime_error(e.GetMsg());
            }
        }
        
        // Assign thread-local point coordinates and evaluate math expression
        state_ptr->x = x_val;
        state_ptr->y = y_val;
        
        try {
            return state_ptr->parser.Eval();
        } catch (mu::Parser::exception_type& e) {
            std::cerr << "\n[muParser Evaluation Error] " << e.GetMsg() << std::endl;
            throw std::runtime_error(e.GetMsg());
        }
    };
}*/

// mpirun -np 4 ./solver --n 256 --tol 1e-6 --max_it 50000 --mode MPI   