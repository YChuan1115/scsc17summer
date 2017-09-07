////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    sgp_main.cpp
/// @brief   The main function.
///
/// @author  William Liao
///

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include "sgp.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  Main function for spectral graph partitioning.
///
int main( int argc, char** argv ){
	int err_test;

	// need 2 argument!
    assert( argc == 2 );

    // check 2nd argument
    assert( argv[1] != NULL );

    // read file
    int E_size, *E;
    cout << "read file..." << endl;
    err_test = readGraph(argv[1], &E, &E_size);
    assert( err_test == 0 );

    // Construct adjacency matrix of graph
    int nnz, *cooRowIndA, *cooColIndA, n;
    double *cooValA;
    cout << "Construct adjacency matrix of graph..." << endl;
    err_test = GraphAdjacency(E, E_size, &nnz, &cooRowIndA, &cooColIndA, &cooValA, &n);
    assert( err_test == 0 );

    // Construct Laplacian
    int *csrRowIndA, *csrColIndA;
    double  *csrValA;
    GraphLaplacian(nnz, cooRowIndA, cooColIndA, cooValA, n, &csrRowIndA, &csrColIndA, &csrValA);
    cout << "nnz = " << nnz << endl;

    // Solve EVP
    double mu0 = 0.005, mu;
    double *x;
    x = new double[n];

    return 0;
}