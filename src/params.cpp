#include "params.hpp"
#include <cmath>

parameters parse_args(int argc, char** argv){
    parameters p;
    for(int i=0;i<argc;i++){
        std::string str= argv[i];
        if(str == "--n") p.n = std::stoi(argv[++i]); //first insert then update
        if(str == "--tol") p.tol = std::stof(argv[++i]); 
        if(str == "--max_it") p.max_it = std::stoi(argv[++i]);
        if(str == "--mode"){
            std::string m =argv[++i];
            if(m == "MPI") p.mode = MPI;
            if(m == "OMP") p.mode = OMP;
            if(m == "HYBRID") p.mode = HYBRID;
        }

        // si può aggiungere anche la scelta della funzione..
        // if (s == "--f") {...

    }

    return p;
}


// mpirun -np 4 ./solver --n 256 --tol 1e-6 --max_it 50000 --mode MPI   