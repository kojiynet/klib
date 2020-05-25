
/*
	krandmp
	Ver. k09.00
	
	OpenMP version of 
	Random number generator using C++11 Standard Library
	
	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef krandmp_cpp_include_guard
#define krandmp_cpp_include_guard

#include <random>
#include <cstdint>
#include <k09/kutil01.cpp>


/* ********** Namespace Declarations/Directives ********** */


/* ********** Class Declarations ********** */

class RandomNumberEngineMP;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int getNMaxThreads( void);
int getNUsedThreads( void);
void setNThreads( int);

// Declare functions
// to get the maximum number of threads,
// to set the number of threads used,
// and to get ID for the current thread
#ifdef _OPENMP
#include <omp.h>
#else
int omp_get_max_threads( void);
void omp_set_num_threads( int num_threads);
int omp_get_num_threads( void);
int omp_get_thread_num( void);
#endif


/* ********** Class Definitions ********** */

class RandomNumberEngineMP {

private:

	// this instance is used to set seeds of 
	// other random number generators
	std::mt19937 rng;

	// this function is to get seed using "rng" above
	uint_fast32_t getRandomSeed( void);
	
public:
	
	RandomNumberEngineMP( void);
	RandomNumberEngineMP( unsigned int);
	~RandomNumberEngineMP( void);
	
	void setSeed( unsigned int);

	std::vector <double> getRealUniformSeq( int, double = 0.0, double = 1.0);

};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */


// Returns the maximum number of threads that
// OpenMP can hold in the environment 
int getNMaxThreads( void)
{
	return omp_get_max_threads();
}


// Returns the number of threads to be used
int getNUsedThreads( void)
{
	int ret;
	#pragma omp parallel
	{
		ret = omp_get_num_threads();
	}
	return ret;
}


// Sets the number of threads that
// OpenMP will use  
void setNThreads( int n0)
{
	omp_set_num_threads( n0);
}


// OpenMPを使ってコンパイルされない場合の関数の定義
// Define functions to use when compiled without OpenMP 
#ifndef _OPENMP

int omp_get_max_threads(void)
{
	return 1;
}

void omp_set_num_threads( int num_threads)
{
	return;
}

int omp_get_num_threads( void)
{
	return 1;
}

int omp_get_thread_num( void)
{
	return 0;
}

#endif // _OPENMP


/* ********** Definitions of Member Functions ********** */

/* ***** class RandomNumberEngineMP ***** */


// Default constructor
// set seed equal to 1 
RandomNumberEngineMP ::
RandomNumberEngineMP( void)
: rng( 1)
{}


// Constructor
// set seed equal to s0 
RandomNumberEngineMP ::
RandomNumberEngineMP( unsigned int s0)
{
	
	// Because seed of mt19937 is actually set by the type "unit_fast32_t",
	// the boundary will be checked;
	// In the most environments "unsigned int" is a 32-bit type.
	// UINT_FAST32_MAX is defined in <cstdint>
	
	if ( s0 > UINT_FAST32_MAX){
		
		alert( "RandomNumberEngineMP :: RandomNumberEngineMP()");
		rng.seed( 1);
		
	} else {
		
		rng.seed( s0);
		
	}
		
}


RandomNumberEngineMP ::
~RandomNumberEngineMP( void)
{}


// Setting seed as s0
void 
RandomNumberEngineMP :: 
setSeed( unsigned int s0)
{
	
	// Because seed of mt19937 is actually set by the type "uint_fast32_t",
	// the boundary will be checked;
	// In the most environments "unsigned int" is a 32-bit type.
	// UINT_FAST32_MAX is defined in <cstdint>
	
	if ( s0 > UINT_FAST32_MAX){
		
		alert( "RandomNumberEngineMP :: setSeed()");
		
	} else {
		
		rng.seed( s0);
		
	}
	
}


uint_fast32_t
RandomNumberEngineMP :: 
getRandomSeed( void)
{

	std::uniform_int_distribution <uint_fast32_t> dist( 0, UINT_FAST32_MAX);

	return dist( rng);

}


// ↓戻り値の型を&&にしてはいけない。
// それをすると関数内のみがスコープのオブジェクトの参照が返ってしまうらしい。
// vectorにはムーブコンストラクタがあるので、move()を使って返せば中身を抜き取ってくれるらしい。
std::vector <double>  
RandomNumberEngineMP :: 
getRealUniformSeq(
	int npoints,
	double lower, // = 0.0
	double upper  // = 1.0
)
{

	int nthreads = getNUsedThreads(); 

	std::vector <std::mt19937> enginevec( nthreads);
	for ( int i = 0; i < enginevec.size(); i++){
		enginevec[ i].seed( getRandomSeed());
	}

	std::vector <double> ret( npoints);

	std::uniform_real_distribution <double> dist( lower, upper);

	#pragma omp parallel for
	for ( int i = 0; i < npoints; i++){
		int tid = omp_get_thread_num();
		ret[ i] = dist( enginevec[ tid]);
	}

	return std::move( ret);

}


#endif // krandmp_cpp_include_guard
