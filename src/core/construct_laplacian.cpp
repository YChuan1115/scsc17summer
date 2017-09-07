////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    construct_laplacian.cpp
/// @brief   The implementation of Laplacian construction.
///
/// @author  Yen Chen Chen
/// @author  Yuhsiang Mike Tsai
///

#include <harmonic.hpp>
#include <iostream>
#include <cmath>
#include <cassert>
#include <numeric>
#include <mkl_spblas.h>
using namespace std;

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
void constructLaplacian(
    const Method method,
    const int nv,
    const int nf,
    const double *V,
    const int *F,
    double *L
) {
  for (int i=0; i<nv*nv; i++)
  {
    L[i]=0;
  }
  if (method == Method::KIRCHHOFF) // Kirchhoff Laplacian Matrix
  {
    for (int i = 0; i < nf; ++i)
    {
      int F_x = F[i]-1;
      int F_y = F[nf+i]-1;
      int F_z = F[2*nf+i]-1;
      L[F_x*nv+F_y] = -1;
      L[F_y*nv+F_z] = -1;
      L[F_z*nv+F_x] = -1;
      L[F_y*nv+F_x] = -1;
      L[F_z*nv+F_y] = -1;
      L[F_x*nv+F_z] = -1;
    }
    for (int i = 0; i<nv; i++){
      L[i*nv+i]=-1*Sum(nv, L+i*nv, 1);
    }
  }else if (method == Method::COTANGENT) // Cotangent Laplacian Matrix
  {
    double *v_ki = new double [3];
    double *v_kj = new double [3];
    double *v_ij = new double [3];
    for (int i = 0; i < nf; ++i)
    {
      int F_x = F[i]-1;
      int F_y = F[nf+i]-1;
      int F_z = F[2*nf+i]-1;

      v_ki[0] = V[F_x] - V[F_z];
      v_ki[1] = V[nv+F_x] - V[nv+F_z];
      v_ki[2] = V[2*nv+F_x] - V[2*nv+F_z];
      v_kj[0] = V[F_y] - V[F_z];
      v_kj[1] = V[nv+F_y] - V[nv+F_z];
      v_kj[2] = V[2*nv+F_y] - V[2*nv+F_z];
      v_ij[0] = V[F_y] - V[F_x];
      v_ij[1] = V[nv+F_y] - V[nv+F_x];
      v_ij[2] = V[2*nv+F_y] - V[2*nv+F_x];
      L[F_x*nv+F_y] += -0.5*Dot(3, v_ki, v_kj)/CrossNorm(v_ki, v_kj);
      L[F_y*nv+F_x] = L[F_x*nv+F_y];
      L[F_y*nv+F_z] += 0.5*Dot(3, v_ij, v_ki)/CrossNorm(v_ij, v_ki);
      L[F_z*nv+F_y] = L[F_y*nv+F_z];
      L[F_z*nv+F_x] += -0.5*Dot(3, v_kj, v_ij)/CrossNorm(v_kj, v_ij);
      L[F_x*nv+F_z] = L[F_z*nv+F_x];
    }
    for (int i = 0; i<nv; i++){
      L[i*nv+i]=-1*Sum(nv, L+i*nv, 1);
    }
  }
}

void GraphLaplacian(int *nnz, int *cooRowIndA,
  int *cooColIndA, double *cooValA, int n, int **csrRowIndA,
  int **csrColIndA, double **csrValA, double shift_sigma){
  double *rowsum, *acsr, *dcsr, tmp=0, beta=-1.0;
  int *sumInd, *ja, *ia, *jd, *id, *tmp_RInd, info, k=0;
  int *job;
  char trans = 'N';
  int request = 1;
  int sort, nzmax=n*n;

  sumInd = new int[n];
  rowsum = new double[n];
  acsr   = new double[*nnz];
  ja     = new int[*nnz];
  ia     = new int[n+1];
  dcsr   = new double[n];
  jd     = new int[n];
  id     = new int[n+1];
  job    = new int[6];

  // Compute sum of each row of A
  for (int i = 0; i < n; i++)
  {
    rowsum[i] = 0;
  }

  for (int i = 0; i < *nnz; i++)
  {
    if (i>0 && cooRowIndA[i]!=cooRowIndA[i-1])
    {
      rowsum[k] = tmp+shift_sigma;
      tmp = 0;
      k++;
    }
    tmp = tmp + cooValA[i];
    if (k==n-1 && i==*nnz-1)
    {
      rowsum[k] = tmp+shift_sigma;
    }
  }

  for (int i = 0; i < n; i++)
  {
    sumInd[i] = i;
  }

  //L = D - A
  job[0] = 2;
  job[1] = 1;
  job[2] = 0;
  job[5] = 0;
  mkl_dcsrcoo(job, &n, acsr, ja, ia, nnz, cooValA, cooRowIndA, cooColIndA, &info);
  mkl_dcsrcoo(job, &n, dcsr, jd, id, &n, rowsum, sumInd, sumInd, &info);
  *csrRowIndA = new int[n+1];
  tmp_RInd    = new int[n+1];
  mkl_dcsradd(&trans, &request, &sort, &n, &n, dcsr, jd, id, &beta, acsr, ja, ia, *csrValA, *csrColIndA, tmp_RInd, &nzmax, &info);
  assert( info == 0 );
  request = 2;
  k = tmp_RInd[n]-1;
  *csrColIndA   = new int[k];
  *csrValA      = new double[k];
  mkl_dcsradd(&trans, &request, &sort, &n, &n, dcsr, jd, id, &beta, acsr, ja, ia, *csrValA, *csrColIndA, tmp_RInd, &nzmax, &info);
  assert( info == 0 );
  copy(tmp_RInd, tmp_RInd+(n+1), *csrRowIndA);
  *nnz = k;

  delete rowsum;
  delete sumInd;
  delete job;
  delete tmp_RInd;
}