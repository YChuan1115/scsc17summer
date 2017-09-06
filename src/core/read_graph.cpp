////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    read_graph.cpp
/// @brief   Read graph from file
///
/// @author  William Liao
///

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>

int readGraph(char *input, double **E){
	std::fstream pfile;
	int count = -1, n = 0;
	double *a, *b;
	int tmp;

	pfile.open(input,std::ios::in);
    assert( pfile );

    // skip first line
    pfile.ignore(4096, '\n');

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Count size
    while( !pfile.eof() ) {
    	count++;
    	pfile.ignore(4096, '\n');
    }
    std::cout << "Size of data is " << count << "x" << 2 << std::endl;

    // Return to top of file
    pfile.clear();
  	pfile.seekg(0, std::ios::beg);

  	a = new double[count];
    b = new double[count];
    *E = new double[2*count];

    // Read graph
  	pfile.ignore(4096, '\n');
  	while( !pfile.eof() ) {
    	pfile >> a[n];
    	std::cout << "a[" << n << "] = " << a[n] << std::endl;
    	pfile >> b[n];
    	std::cout << "b[" << n << "] = " << b[n] << std::endl;
    	pfile.ignore(4096, '\n');
    }
    std::cout << "a[" << count-1 << "] = " << a[count-1] << std::endl;

    return 0;
}