
/*

	krandmptest01.cpp

	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto
	
	Testing krandmp Ver.k09.00

	Use compilation options:
	> cl krandmptest01.cpp -EHsc -openmp -Ox
	> g++ krandmptest01.cpp -std=c++17 -fopenmp -O3

*/
	

/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <chrono>

#include <boost/math/tools/bivariate_statistics.hpp>

#include <k09/krand00.cpp>
#include <k09/krandmp00.cpp>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);
double corr( const std::vector <double> &, const std::vector <double> &);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{

	using time_point = std::chrono::system_clock::time_point;

	int npoints = 50000000;

	int nmaxthreads = getNMaxThreads();
	std::cout << "N Max Threads: " << nmaxthreads << std::endl << std::endl;

	int nusedthreads = getNUsedThreads();
	std::cout << "N Used Threads: " << nusedthreads << std::endl << std::endl;

	// マルチスレッド
	{
		
		std::cout << "Trial 1: Multi-Threaded" << std::endl;

		time_point startt = std::chrono::system_clock::now();
	
		RandomNumberEngineMP xrnemp( 123);
		RandomNumberEngineMP yrnemp( 456);
		// below instances will be initialized by moved objects
		std::vector <double> xvec = xrnemp.getRealUniformSeq( npoints, -1.0, 1.0);
		std::vector <double> yvec = yrnemp.getRealUniformSeq( npoints, -1.0, 1.0); 

		time_point endt = std::chrono::system_clock::now();
		auto millisec = std::chrono::duration_cast <chrono::milliseconds> ( endt - startt);
		std::cout << millisec.count() << " milliseconds passed." << std::endl;
		std::cout << "Corr: " << corr( xvec, yvec) << std::endl;
		std::cout << std::endl;
		
	}


	// シングルスレッドでの比較用
	{
		
		std::cout << "Trial 2: Single-Threaded" << std::endl;

		time_point startt = std::chrono::system_clock::now();
	
		RandomNumberEngine xrne( 123);
		RandomNumberEngine yrne( 456);
		// below instances will be initialized by moved objects
		vector <double> xvec = xrne.getRealUniformSeq( npoints, -1.0, 1.0);
		vector <double> yvec = yrne.getRealUniformSeq( npoints, -1.0, 1.0); 

		time_point endt = std::chrono::system_clock::now();
		auto millisec = std::chrono::duration_cast <chrono::milliseconds> ( endt - startt);
		std::cout << millisec.count() << " milliseconds passed." << std::endl;
		std::cout << "Corr: " << corr( xvec, yvec) << std::endl;
		std::cout << std::endl;

	}

	return 0;
	
}


double corr( const std::vector <double> &xvec, const std::vector <double> &yvec)
{

	double ret = boost::math::tools::correlation_coefficient( xvec, yvec);
	return ret;

}


/* ********** Definitions of Member Functions ********** */

