////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    main.hpp
/// @brief   The main function.
///
/// @author  Mu Yang <<emfomy@gmail.com>>
///

#include <iostream>
#include <harmonic.hpp>
#include <timer.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  Main function
///
int main( int argc, char** argv ) {

  const char *input  = "input.obj";
  const char *output = "output.obj";
  Method method  = Method::SIMPLE;

  int nv, nf, nb, *F = nullptr, *idx_b;
  double timer, *V = nullptr, *C = nullptr, *L, *U;

  // Read arguments
  readArgs(argc, argv, input, output, method);

  // Read object
  readObject(input, &nv, &nf, &V, &C, &F);

  cout << endl;

  // Verify boundary
  idx_b = new int[nv];
  cout << "Verifying boundary .....................";
  tic(&timer);
  verifyBoundary(nv, nf, F, &nb, idx_b); cout << " Done.  ";
  toc(&timer);

  // Reorder vertices
  cout << "Reordering vertices ....................";
  tic(&timer);
  reorderVertex(nv, nb, nf, idx_b, V, C, F); cout << " Done.  ";
  toc(&timer);

  // Construct Laplacian
  L = new double[nv * nv];
  cout << "Constructing Laplacian .................";
  tic(&timer);
  constructLaplacian(method, nv, nf, V, F, L); cout << " Done.  ";
  toc(&timer);

  // Map boundary
  U = new double[2 * nv];
  cout << "Mapping Boundary .......................";
  tic(&timer);
  mapBoundary(nv, nb, V, U); cout << " Done.  ";
  toc(&timer);

  // Solve harmonic
  cout << "Solving Harmonic .......................";
  tic(&timer);
  solveHarmonic(nv, nb, L, U); cout << " Done.  ";
  toc(&timer);

  cout << endl;

  // Write object
  writeObject(output, nv, nf, U, C, F);

  // Free memory
  delete[] V;
  delete[] C;
  delete[] F;
  delete[] L;
  delete[] U;
  delete[] idx_b;

  return 0;
}
