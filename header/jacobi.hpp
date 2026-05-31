#ifndef JACOBI_HPP
#define JACOBI_HPP

#include <cmath>
#include <functional>
#include <vector>

class Grid_Sol{

    int n; //dimension;
    //for parallelization;
    int row_start;
    int row_end;

    double h; // discretization 1/(n-1)

    //main structure of the solution...

    //definire la soluzione in modo da avere U(i,j)  (funzione at())
    // serve anche funzione ceh aggiorna con jacobi---> va inizializzata U(i,j) con zero quando viene creata e poi applicata jacobi_sweep per trovare la soluzione effettiva 
    
    // quindi possibile struttura :
    // variabile privata U vettore (o utilizzando eigen...)
    // in modo che sia anche parallelizabile 
    //funzione pubblica at() come setter/getter
    // + funzione per aggiornare . e aggiorni l'errore (forse meglio salvare due soluzioni a due iterate successive...)



};



#endif JACOBI_HPP