#include "jacobi.hpp"

double Jacobi_update(Matrix_Sol& M,int row_lo, int row_hi,
                         const std::function<double(double,double)>& f)
      {
        double h=M.get_h();
        
        double iter_err=0.0;

        #pragma omp parallel for reduction(+:iter_err) collapse(2) schedule(static) // rig aggiunta per omp: è una flag quindi lavora in parallelo solo se lo chiamo in parallelo
        for (int i=row_lo;i<row_hi;++i){
          for (int j=1;j<M.get_n()-1;++j){ // need to update U(i,j) for i,j =2,.....n-1
            int iglo=M.get_row_start()+i;
            int jglo=j;

            //posso mettere anche xi =iglo*h e yi = jglo*h perchè nella funzione servono gli effettivi valori f(i,j)=f(xi,yj)

            double Uk = 0.25*(M.at(i-1,j)+M.at(i+1,j)+M.at(i,j-1)+M.at(i,j+1)+f(iglo*h,jglo*h)*h*h); // can't use f(i,j)*h*h because i'm working in parallel and so i need global index for the function
            
            M.at_new(i,j)=Uk;

            double diff= Uk-M.at(i,j);
            iter_err += diff*diff;
          }

        }

        //updating U with U_new then returning the error 
        // doing so in solver i will do a while cycle until the output is small enough

        M.swap_buffers();

        return std::sqrt(iter_err*h);
                           
      }

      // possibile implementazione per un errore in norma L2
    double L2_err(Matrix_Sol& M,const std::function<double(double,double)>& u_ex){
        
      double h=M.get_h();
      double l2e = 0.0;

      for(int i=0;i<M.local_rows();++i){
        for(int j=0;j<M.get_n();++j){
          double xi=(M.get_row_start()+i)*h;
          double yj=(j*h);

          double diff=M.at(i,j)-u_ex(xi,yj);
          l2e += diff*diff;
        }
      }

      return std::sqrt(l2e*h);

    }