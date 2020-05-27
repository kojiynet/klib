
/*
	kstatboost
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Statistical Computations using Boost
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kstatboost_cpp_include_guard
#define kstatboost_cpp_include_guard

#include <vector>

#include <boost/math/tools/bivariate_statistics.hpp>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Type Declarations: enum, class, etc. ********** */


/* ********** Function Declarations ********** */

double corrBoost( const std::vector <double> &, const std::vector <double> &); 


/* ********** Type Definitions: enum, class, etc. ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

double corrBoost( const std::vector <double> &xvec, const std::vector <double> &yvec)
{

	double ret = boost::math::tools::correlation_coefficient( xvec, yvec);
	return ret;

}


/* ********** Definitions of Member Functions ********** */




#endif /* kstatboost_cpp_include_guard */
