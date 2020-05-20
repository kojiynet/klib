
/*
	krand.cpp
	Ver. k09.00
	
	Random number generator using C++11 Standard Library
	
	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef krand_cpp_include_guard
#define krand_cpp_include_guard

#include <random>
#include <cstdint>


/* ********** Using Directives ********** */


/* ********** Class Declarations ********** */

class RandomNumberEngine;
class RandomDiscreteUniform;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

double randomUniform( double = 0.0, double = 1.0);


/* ********** Class Definitions ********** */

/*
	class RandomNumberEngine
	Set up mt19937 
	Generates random numbers of uniform distribution
*/
class RandomNumberEngine {

private:
	
	std::mt19937 rng;
	
	
public:
	
	RandomNumberEngine( void);
	RandomNumberEngine( unsigned int);
	~RandomNumberEngine( void);
	
	void copyFrom( const RandomNumberEngine &);
	void setSeed( unsigned int);

	double getRealUniform( double = 0.0, double = 1.0);
	int getDiscreteUniform( int, int);
	double getNormal( double = 0.0, double = 1.0);

	void getBivariateNormal( double &, double &, double, double, double, double, double);

	std::vector <double> && getRealUniformSeq( int, double = 0.0, double = 1.0);
	
	friend class RandomDiscreteUniform;
	
};


/*
	class RandomDiscreteUniform
	Set up random number generator of discrete uniform distribution
	
	This class is "friend"-declared in class RandomNumberEngine.
*/
class RandomDiscreteUniform {

private:
	
	std::uniform_int_distribution <int> dist;
	
	
public:
	
	RandomDiscreteUniform( void);
	RandomDiscreteUniform( int, int);
	~RandomDiscreteUniform( void);
	
	void initialize( int min, int max);
	int getNextValue( RandomNumberEngine &);
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

// 簡易に、seedを1とする、[lower, upper)の一様乱数を取得する。
// lowerのデフォルト値は0.0
// upperのデフォルト値は1.0
// Simply gets a random number ranging [lower, upper) 
// whose seed is set 1 at first
// Default values: lower = 0.0, upper = 1.0
double randomUniform( double lower, double upper)
{

	static std::mt19937 engine( 1);

	std::uniform_real_distribution <double> dist( lower, upper);

	return dist( engine);

}


/* ********** Definitions of Member Functions ********** */

/* ****** class RandomDiscreteUniform ***** */

// By default, seed is set to 1.
RandomNumberEngine :: RandomNumberEngine( void)
{
	
	rng.seed( 1);
	
}

// Setting seed as s
RandomNumberEngine :: RandomNumberEngine( unsigned int s)
{
	
	// Because seed of mt19937 is actually set by the type "unit_fast32_t",
	// the boundary will be checked;
	// In the most environments "unsigned int" is a 32-bit type.
	// UINT_FAST32_MAX is defined in <cstdint>
	
	if ( s > UINT_FAST32_MAX){
		
		alert( "RandomNumberEngine :: RandomNumberEngine()");
		rng.seed( 1);
		
	} else {
		
		rng.seed( s);
		
	}
	
}

RandomNumberEngine :: ~RandomNumberEngine( void)
{
}

// copy rng from other instance
void RandomNumberEngine :: copyFrom( const RandomNumberEngine &rne0)
{
	
	rng = rne0.rng;
	
}

// Setting seed as s
void RandomNumberEngine :: setSeed( unsigned int s)
{
	
	// Because seed of mt19937 is actually set by the type "unit_fast32_t",
	// the boundary will be checked;
	// In the most environments "unsigned int" is a 32-bit type.
	// UINT_FAST32_MAX is defined in <cstdint>
	
	if ( s > UINT_FAST32_MAX){
		
		alert( "RandomNumberEngine :: setSeed()");
		
	} else {
		
		rng.seed( s);
		
	}
	
}

// returns random number from continuous uniform distribution
// default values: min = 0.0, max = 1.0
double RandomNumberEngine :: getRealUniform( double min, double max)
{
	
	std::uniform_real_distribution <double> dist( min, max);
	
	return dist( rng);
	
}

// returns random number from discrete uniform distribution
int RandomNumberEngine :: getDiscreteUniform( int min, int max)
{
	
	std::uniform_int_distribution <int> dist( min, max);
	
	return dist( rng);
	
}

// returns random number from normal distribution
// by default mean will be 0.0 and sigma will be 1.0
double RandomNumberEngine :: getNormal( double mean, double sigma)
{
	
	std::normal_distribution <double> dist( mean, sigma);
	
	return dist( rng);
	
}

// returns random numbers from bivariate normal distribution
// with ret1 corresponding to mean1 and sigma1,
//      ret2 corresponding to mean2 and sigma2,
//  and the correlation being rho
void RandomNumberEngine :: getBivariateNormal(
	double &ret1, double &ret2,
	double mean1, double sigma1,
	double mean2, double sigma2,
	double rho
)
{
	
	std::normal_distribution <double> dist( 0.0, 1.0);
	
	double x1 = dist( rng);
	double x2 = dist( rng);
	
	ret1 = mean1 + sigma1 * x1;
	ret2 = mean2 + sigma2 * ( rho * x1 + sqrt( 1 - rho * rho) * x2);
	
}

// returns vector of random numbers from continuous uniform distribution
// the size of vector will be n
// return value will be moved 
// default values: min = 0.0, max = 1.0
std::vector <double> && 
RandomNumberEngine :: 
getRealUniformSeq( int n, double min, double max)
{
	
	std::vector <double> ret( n);

	std::uniform_real_distribution <double> dist( min, max);

	for ( int i = 0; i < n; i++){
		ret[ i] = dist( rng);
	}

	return std::move( ret);
	
}


/* ****** class RandomDiscreteUniform ***** */

// By default, return values take 0 or 1 only.
RandomDiscreteUniform :: RandomDiscreteUniform( void)
{
	
	initialize( 0, 1);
	
}

// Setting minimum value and maximum value
RandomDiscreteUniform :: RandomDiscreteUniform( int min, int max)
{
	
	initialize( min, max);
	
}

RandomDiscreteUniform :: ~RandomDiscreteUniform( void)
{
}

// Setting minimum value and maximum value
void RandomDiscreteUniform :: initialize( int min, int max)
{
	
	std::uniform_int_distribution <int> dist0( min, max);
	dist = dist0;
	
}

// Generate next random number using engine "rne"
int RandomDiscreteUniform :: getNextValue( RandomNumberEngine &rne)
{
	
	return dist( rne.rng);
	
}


#endif // krand_cpp_include_guard
