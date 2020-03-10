
/*
	
	kstattest01.cpp
	
	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto
	
	Testing kstat Ver.k09.02
	
*/


/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <string>

#include <k09/kstat02.cpp>


/* ********** Using Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Declarations ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{
	
	cout << "kstattest01.cpp" << endl
	     << "Testing kstat.cpp Ver.k09.02" << endl << endl;
	
	cout << "***** Testing class \"JudgeEqual\" and \"JudgeEqualTol\" *****" << endl << endl;
	
	int i1, i2, i3;
	i1 = 3;
	i2 = 3;
	i3 = -3;
		
	cout << "***** Printing int varibles *****" << endl;
	cout << "i1 = " << i1 << ", "
	     << "i2 = " << i2 << ", "
	     << "i3 = " << i3 << endl << endl;
	
	JudgeEqual <int> judgeint;

	cout << "***** Printing return values of operator() of \"judgeint\" *****" << endl;
	cout << "***** where \"judgeint\" is a JudgeEqual<int> object       *****" << endl;

	cout << "judgeint( i1, i2) = " << judgeint( i1, i2) << endl;
	cout << "judgeint( i1, i3) = " << judgeint( i1, i3) << endl;
	cout << endl;
	
	cout << "***** Printing return values of operator() of \"judgeinttol\" *****" << endl;
	cout << "***** where \"judgeinttol\" is a JudgeEqualTol<int> object    *****" << endl;
	cout << "***** initialized with Tolerance 2                          *****" << endl;

	JudgeEqualTol <int>    judgeinttol( 2);

	cout << "judgeinttol( i1, i2) = " << judgeinttol( i1, i2) << endl;
	cout << "judgeinttol( i1, i3) = " << judgeinttol( i1, i3) << endl;
	cout << endl;
	
	cout << "***** Printing return values of operator() of \"judgeinttol\" *****" << endl;
	cout << "***** where \"judgeinttol\" is a JudgeEqualTol<int> object    *****" << endl;
	cout << "***** after judgeinttol.setTol( 6) was called               *****" << endl;

	judgeinttol.setTol( 6);

	cout << "judgeinttol( i1, i2) = " << judgeinttol( i1, i2) << endl;
	cout << "judgeinttol( i1, i3) = " << judgeinttol( i1, i3) << endl;
	cout << "judgeinttol( i3, i1) = " << judgeinttol( i3, i1) << endl;
	cout << endl;
	
	double d1, d2, d3;

	d1 = 3.0;
	d2 = 3.1;
	d3 = -3.1;

	cout << "***** Printing double varibles *****" << endl;
	cout << "d1 = " << d1 << ", "
	     << "d2 = " << d2 << ", "
		 << "d3 = " << d3 << endl << endl;	
	
	cout << "***** Printing return values of operator() of \"judgedouble\" *****" << endl;
	cout << "***** where \"judgedouble\" is a JudgeEqual<double> object    *****" << endl;

	JudgeEqual <double> judgedouble;
	
	cout << "judgedouble( d1, d2) = " << judgedouble( d1, d2) << endl;
	cout << "judgedouble( d1, d3) = " << judgedouble( d1, d3) << endl;
	cout << endl;

	
	cout << "***** Printing return values of operator() of \"judgedoubletol\" *****" << endl;
	cout << "***** where \"judgedoubletol\" is a JudgeEqualTol<double> object *****" << endl;
	cout << "***** initialized with Tolerance 0.1                           *****" << endl;

	JudgeEqualTol <double> judgedoubletol( 0.1);
	
	cout << "judgedoubletol( d1, d2) = " << judgedoubletol( d1, d2) << endl;
	cout << "judgedoubletol( d1, d3) = " << judgedoubletol( d1, d3) << endl;
	cout << endl;

	cout << "***** Printing return values of operator() of \"judgedoubletol\" *****" << endl;
	cout << "***** where \"judgedoubletol\" is a JudgeEqualTol<double> object *****" << endl;
	cout << "***** after judgedoubletol.setTol( 6.2) was called             *****" << endl;

	judgedoubletol.setTol( 6.2);

	cout << "judgedoubletol( d1, d2) = " << judgedoubletol( d1, d2) << endl;
	cout << "judgedoubletol( d1, d3) = " << judgedoubletol( d1, d3) << endl;
	cout << "judgedoubletol( d2, d3) = " << judgedoubletol( d2, d3) << endl;
	cout << endl;
	
	cout << "***** Printing return values of operator() of \"judgedoubletol\" *****" << endl;
	cout << "***** where \"judgedoubletol\" is a JudgeEqualTol<double> object *****" << endl;
	cout << "***** after judgedoubletol.setTol( 6.3) was called             *****" << endl;

	judgedoubletol.setTol( 6.3);

	cout << "judgedoubletol( d1, d2) = " << judgedoubletol( d1, d2) << endl;
	cout << "judgedoubletol( d1, d3) = " << judgedoubletol( d1, d3) << endl;
	cout << "judgedoubletol( d2, d3) = " << judgedoubletol( d2, d3) << endl;
	cout << endl;
	
	return 0;
	
}


/* ********** Definitions of Member Functions ********** */

