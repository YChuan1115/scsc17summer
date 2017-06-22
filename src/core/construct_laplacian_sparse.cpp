////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    construct_laplacian_sparse.cpp
/// @brief   The implementation of Laplacian construction.
///
/// @author  Yen Chen Chen, Yuhsiang Mike Tsai
///

#include <harmonic.hpp>
#include <iostream>
#include <cmath>
using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @todo  To be implemented!
///
double CrossNorm(const double *x, const double *y) {
  double z[3];
  z[0] = x[1]*y[2] - x[2]*y[1];
  z[1] = x[2]*y[0] - x[0]*y[2];
  z[2] = x[0]*y[1] - x[1]*y[0];
  return sqrt(z[0]*z[0]+z[1]*z[1]+z[2]*z[2]);
}

double Sum(const int n, const double *x, const int incx) {
  double sum = 0;
  for (int i = 0; i < n; ++i)
  {
    sum += x[i*incx];
  }
  return sum;
}

double Dot(const int n, const double *x, const double *y) {
  double ans=0;
  for (int i=0; i<n; i++){
    ans+= x[i]*y[i];
  }
  return ans;
}
void constructLaplacianSparse( 
  const Method method, const int nv, const int nb, const int nf, const double *V, const int *F,
  double **ptr_Lii_val, int **ptr_Lii_row, int **ptr_Lii_col, int *ptr_Lii_nnz,
  double **ptr_Lib_val, int **ptr_Lib_row, int **ptr_Lib_col, int *ptr_Lib_nnz
) {
  int Lii_nnz=nv-nb, Lib_nnz=0, F_x=0, F_y=0, F_z=0;
  for (int i=0; i<nf; i++) {
    F_x = F[i]-1;
    F_y = F[nf+i]-1;
    F_z = F[2*nf+i]-1;
    if (F_x >= nb && F_y >= nb) Lii_nnz++;
    if (F_y >= nb && F_z >= nb) Lii_nnz++;
    if (F_z >= nb && F_x >= nb) Lii_nnz++;
    if (F_x >= nb && F_y < nb)  Lib_nnz++;
    if (F_y >= nb && F_z < nb)  Lib_nnz++;
    if (F_z >= nb && F_x < nb)  Lib_nnz++;
  }
  *ptr_Lii_nnz=Lii_nnz;
  *ptr_Lib_nnz=Lib_nnz;
  *ptr_Lii_val = new double [Lii_nnz];
  *ptr_Lii_row = new int [Lii_nnz];
  *ptr_Lii_col = new int [Lii_nnz];
  *ptr_Lib_val = new double [Lib_nnz];
  *ptr_Lib_row = new int [Lib_nnz];
  *ptr_Lib_col = new int [Lib_nnz];
  int *Lib_row=*ptr_Lib_row, *Lib_col=*ptr_Lib_col, *Lii_row=*ptr_Lii_row, *Lii_col=*ptr_Lii_col;
  double *Lib_val=*ptr_Lib_val, *Lii_val=*ptr_Lii_val;
  for (int i=0; i<nv-nb; i++) {
    Lii_val[i]=0;
    Lii_col[i]=i+nb;
    Lii_row[i]=i+nb;
  }
  int index_Lii=nv-nb, index_Lib=0;
  int row=0, col=0;
  if (method == Method::SIMPLE) //Simple Laplacian Matrix
  {
    for (int i = 0; i < nf; ++i)
    {
      for (int k=0; k<3; k++) {
        row =F[k*nf+i]-1;
        col =F[((k+1)%3)*nf+i]-1;
        if (row >= nb && col>= nb) {
          Lii_val[index_Lii]=-1;
          Lii_col[index_Lii]=col;
          Lii_row[index_Lii]=row;
          Lii_val[row-nb]++;
          index_Lii++;
        }
        else if (row>=nb && col< nb) {
          Lib_val[index_Lib]=-1;
          Lib_col[index_Lib]=col;
          Lib_row[index_Lib]=row;
          Lii_val[row-nb]++;
          index_Lib++;
        }
      }
    }
    if (index_Lib!=Lib_nnz || index_Lii+nv-nb != Lii_nnz) {
      cerr<<"SIMPLE: nnz, index  Error\n";
      exit(1);
    }
    
  }else if (method == Method::COMPLEX) // Cotengent Laplacian Matrix
  {
    // double *v_ki = new double [3];
    // double *v_kj = new double [3];
    // double *v_ij = new double [3];
    // for (int i = 0; i < nf; ++i)
    // {
    //   int F_x = F[i]-1;
    //   int F_y = F[nf+i]-1;
    //   int F_z = F[2*nf+i]-1;

    //   v_ki[0] = V[F_x] - V[F_z];
    //   v_ki[1] = V[nv+F_x] - V[nv+F_z];
    //   v_ki[2] = V[2*nv+F_x] - V[2*nv+F_z];
    //   v_kj[0] = V[F_y] - V[F_z];
    //   v_kj[1] = V[nv+F_y] - V[nv+F_z];
    //   v_kj[2] = V[2*nv+F_y] - V[2*nv+F_z];
    //   v_ij[0] = V[F_y] - V[F_x];
    //   v_ij[1] = V[nv+F_y] - V[nv+F_x];
    //   v_ij[2] = V[2*nv+F_y] - V[2*nv+F_x];
    //   L[F_x*nv+F_y] += -0.5*Dot(3, v_ki, v_kj)/CrossNorm(v_ki, v_kj);
    //   L[F_y*nv+F_x] = L[F_x*nv+F_y];
    //   L[F_y*nv+F_z] += 0.5*Dot(3, v_ij, v_ki)/CrossNorm(v_ij, v_ki);
    //   L[F_z*nv+F_y] = L[F_y*nv+F_z];
    //   L[F_z*nv+F_x] += -0.5*Dot(3, v_kj, v_ij)/CrossNorm(v_kj, v_ij);
    //   L[F_x*nv+F_z] = L[F_z*nv+F_x];
    // }
    // for (int i = 0; i<nv; i++){
    //   L[i*nv+i]=-1*Sum(nv, L+i*nv, 1);
    // }
  }
}
