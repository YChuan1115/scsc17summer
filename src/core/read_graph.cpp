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

int readGraph(char *input, double *E){
	std::fstream pfile;
	int count = 0;
	double *a, *b;
	int tmp;

	pfile.open(input,std::ios::in);
    assert( pfile );

    // skip first line
    pfile.ignore(4096, '\n');

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Count size
    while( !pfile.eof() ) {
    	++count;
    	fin.ignore(4096, '\n');
    	std::cout << count << std::endl;
    }
    std::cout << "Size of data is " << count << "x" << 2 << std::endl;

    // Return to top of file

    return 0;
}