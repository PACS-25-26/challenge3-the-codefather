#ifndef JACOBI_HPP
#define JACOBI_HPP

#include <cmath>
#include <functional>
#include <vector>

class Matrix_Sol{

    private:

    int n; //dimension;
    //for parallelization;
    int row_start;
    int row_end;

    double h; // discretization 1/(n-1)

    std::vector<double> U; //solution at iter k;
    std::vector<double> U_new; // new iteration solution

    public:

    // constructor
    Matrix_Sol(int n,int row_start,int row_end) : n(n) ,row_start(row_start),row_end(row_end),U((row_end - row_start) * n, 0.0),
          U_new((row_end - row_start) * n, 0.0)
          {
            h=(1.0/(n-1));
          }

    void swap_buffers() 
        { 
            std::swap(U, U_new); 
        }

    // getter and setter:
    double& at(int local_row, int col) 
        {
         return U[local_row * n + col];
        }
    double  at(int local_row, int col) const 
        {
             return U[local_row * n + col]; 
        }
    double& at_new(int local_row, int col) 
        { 
             return U_new[local_row * n + col]; 
        }
    double at_new(int local_row, int col) const 
        { 
             return U_new[local_row * n + col]; 
        }

    int local_rows() const { return row_end - row_start; }
    
    int get_n()         const { return n; }
    int get_row_start() const { return row_start; }
    int get_row_end()   const { return row_end; }
    double get_h()      const { return h; }
   
    //altre possibili get o set per MPI...

};

 // main function:

    double Jacobi_update(Matrix_Sol& M,
                         int row_lo, int row_hi,
                         const std::function<double(double,double)>& f);
        // anche coi template penso si possa fare però non servirebbe più .cpp ma solo header
    
 // possibile funzione errore L2... per benchmark con soluzione esatta

    double L2_err(Matrix_Sol& M,const std::function<double(double,double)>& u_ex);

#endif //JACOBI_HPP
 //main structure of the solution...

    //definire la soluzione in modo da avere U(i,j)  (funzione at())
    // serve anche funzione ceh aggiorna con jacobi---> va inizializzata U(i,j) con zero quando viene creata e poi applicata jacobi_sweep per trovare la soluzione effettiva 
    
    // quindi possibile struttura :
    // variabile privata U vettore (o utilizzando eigen...)
    // in modo che sia anche parallelizabile 
    //funzione pubblica at() come setter/getter
    // + funzione per aggiornare . e aggiorni l'errore (forse meglio salvare due soluzioni a due iterate successive...)


/*
#pragma once
#include <vector>
#include <cmath>
#include <functional>

// Rappresenta la griglia locale di un rank (o globale nel caso seriale)
struct Grid {
    int n;          // dimensione globale
    int row_start;  // prima riga di questa griglia (0-indexed, inclusi i ghost)
    int row_end;    // ultima riga (esclusa)
    double h;       // mesh spacing = 1/(n-1)
    std::vector<double> U;     // soluzione corrente, size = rows * n
    std::vector<double> U_new; // buffer aggiornamento

    Grid(int n, int row_start, int row_end)
        : n(n), row_start(row_start), row_end(row_end),
          h(1.0/(n-1)),
          U((row_end - row_start) * n, 0.0),
          U_new((row_end - row_start) * n, 0.0) {}

    // Accesso comodo: U[local_row][col]
    double& at(int local_row, int col) { return U[local_row * n + col]; }
    double  at(int local_row, int col) const { return U[local_row * n + col]; }
    double& at_new(int local_row, int col) { return U_new[local_row * n + col]; }

    int local_rows() const { return row_end - row_start; }
};

// Esegue un singolo sweep Jacobi sulle righe interne [row_lo, row_hi)
// (esclude la prima e l'ultima riga che sono boundary o ghost)
double jacobi_sweep(Grid& g,
                    int row_lo, int row_hi,
                    const std::function<double(double,double)>& f);

// Calcola l'errore L2 locale
double l2_error(const Grid& g,
                const std::function<double(double,double)>& u_exact);
*/