
/*
	kstatboost
	Ver. k09.02
	
	Written by Koji Yamamoto
	Copyright (C) 2020-2022 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Statistical Computations using Boost
	Plus, Random Number from Distrbution using Boost 
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kstatboost_cpp_include_guard
#define kstatboost_cpp_include_guard

#include <vector>
#include <functional>

#include <k09/krand02.cpp> 

#include <boost/math/statistics/univariate_statistics.hpp>
#include <boost/math/statistics/bivariate_statistics.hpp>
#include <boost/math/distributions/beta.hpp>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Type Declarations: enum, class, etc. ********** */


/* ********** Function Declarations ********** */

double unbiasedVarBoost( const std::vector <double> &); 
double corrBoost( const std::vector <double> &, const std::vector <double> &); 

std::function <double(double)>
getBetaQ(
	double, double
);

void 
getBetaRandomVec(
	std::vector <double> &, RandomNumberEngine &, int, double, double, bool = false
);


/* ********** Type Definitions: enum, class, etc. ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */


double unbiasedVarBoost( const std::vector <double> &vec0)
{

	// sample_variance() returns unbiased variance in estimation from sample 
	double sigma_sq = boost::math::statistics::sample_variance( vec0);
	return sigma_sq;

}


// corrBoost()について、コンパイル時に警告が出てくるので、これを抑制しておく。
// 警告の趣旨は型変換によって精度が落ちる可能性を指摘したものらしい。
// 実際には、boost\math\statistics\bivariate_statictics.hppの
// correlation_coefficient_seq_impl( )の定義で、整数を浮動小数点にキャストしていないから。
// →Githubでプルリクを投げた。

#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(push) 
#pragma warning(disable:4244) 
#endif 

double corrBoost( const std::vector <double> &xvec, const std::vector <double> &yvec)
{

	double ret = boost::math::statistics::correlation_coefficient( xvec, yvec);
	return ret;

}

#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(pop) 
#endif 


// returns quatile function (inverse distribution function)
// of targeted Beta dist. with specific mean and variance 
std::function <double(double)> getBetaQ( double mean0, double var0)
{

	using boost::math::beta_distribution;
	
	double alpha = beta_distribution<>::find_alpha( mean0, var0);
	double beta = beta_distribution<>::find_beta( mean0, var0);

	beta_distribution<> mybeta( alpha, beta);

	auto ret =
		[=]( double p) -> double 
		{
			return quantile( mybeta, p);
		};
	
	return ret;

}

// returns vector of random numbers from targeted Beta distribution 
// if withProgress == true then this calls getDistRandomVecWithProgress()
// which will report progress 
void
getBetaRandomVec(
	std::vector <double> &ret,
	RandomNumberEngine &rne,
	int len,
	double mean0,
	double var0,
	bool withProgress // = false 
)
{

	ret.clear();

	auto qfunc = getBetaQ( mean0, var0);
	
	if ( withProgress == true){
		ret = rne.getDistRandomVecWithProgress( len, qfunc);
	} else {
		ret = rne.getDistRandomVec( len, qfunc);
	}

}


/* ********** Definitions of Member Functions ********** */




#endif /* kstatboost_cpp_include_guard */
