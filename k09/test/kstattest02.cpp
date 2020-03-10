
/*
	
	kstattest02.cpp
	
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
	
	cout << "kstattest02.cpp" << endl
	     << "Testing kstat.cpp Ver.k09.02" << endl << endl;
	

	cout << "***** Testing class \"FreqType\" *****" << endl << endl;
	
	FreqType <int> ft;
	vector <int> keyvec;
	vector <int> frevec;
	
	ft.increment( 1);
	ft.increment( 2);
	ft.increment( 1);
	ft.increment( 3);
	ft.increment( 1);
	ft.increment( 5);
	ft.increment( 2);
	ft.getVectors( keyvec, frevec);
	
	cout << "****************************************************" << endl;
	cout << "Declaring FreqType <int> ft, " << endl
	     << "and running ft.increment() several times " << endl
		 << "so that the keys 1, 2, 1, 3, 1, 5, 2 are incremented" << endl;
	cout << "****************************************************" << endl;
	cout << "***** Results of ft.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvec.size(); i++){
		cout << " key: " << keyvec[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << endl;
	

	ft.clear();
	ft.setEqualWithTol( 3);
	ft.increment( 1);
	ft.increment( 2);
	ft.increment( 1);
	ft.increment( 3);
	ft.increment( 1);
	ft.increment( 5);
	ft.increment( 2);
	ft.getVectors( keyvec, frevec);
	
	cout << "****************************************************" << endl;
	cout << "Running ft.clear(), ft.setEqualWithTol( 3), " << endl
	     << "and ft.increment() several times " << endl
		 << "so that the keys 1, 2, 1, 3, 1, 5, 2 are incremented" << endl;
	cout << "****************************************************" << endl;
	cout << "***** Results of ft.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvec.size(); i++){
		cout << " key: " << keyvec[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << endl;
	
	
	ft.clear();
	ft.setEqualWithTol( 3);
	ft.increment( 2);
	ft.increment( 1);
	ft.increment( 1);
	ft.increment( 3);
	ft.increment( 1);
	ft.increment( 5);
	ft.increment( 2);
	ft.getVectors( keyvec, frevec);


	cout << "****************************************************" << endl;
	cout << "Running ft.clear(), ft.setEqualWithTol( 3), " << endl
	     << "and ft.increment() several times " << endl
	     << "so that the keys 2, 1, 1, 3, 1, 5, 2 are incremented" <<endl;
	cout << "Note that in this case 2 is added first" << endl;
	cout << "****************************************************" << endl;
	cout << "***** Results of ft.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvec.size(); i++){
		cout << " key: " << keyvec[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << endl;


	ft.clear();
	ft.setEqualWithTol( 3);
	ft.addPossibleKey( 1);
	ft.increment( 2);
	ft.increment( 1);
	ft.increment( 1);
	ft.increment( 3);
	ft.increment( 1);
	ft.increment( 5);
	ft.increment( 2);
	ft.getVectors( keyvec, frevec);


	cout << "*******************************************************" << endl;
	cout << "Running ft.clear(), ft.setEqualWithTol( 3), " << endl
	     << "ft.addPossibleKey( 1), and ft.increment() several times " << endl
		 << "so that the keys 2, 1, 1, 3, 1, 5, 2 are incremented" << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ft.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvec.size(); i++){
		cout << " key: " << keyvec[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << endl;
	
	
	FreqType <double> ftd;
	vector <double> keyvecd;
	
	ftd.setEqualExactly();
	ftd.increment( 1.0);
	ftd.increment( 2.0);
	ftd.increment( 1.1);
	ftd.increment( 3.0);
	ftd.increment( 1.2);
	ftd.increment( 5.0);
	ftd.increment( 2.1);
	ftd.getVectors( keyvecd, frevec);
	
	cout << "*******************************************************" << endl;
	cout << "Declaring FreqType <double> ftd, " << endl
	     << "running ftd.setEqualExactly(), " << endl
	     << "and running ftd.increment() several times" << endl
	     << "so that the following keys are incremented:" << endl
	     << "1.0, 2.0, 1.1, 3.0, 1.2, 5.0, 2.1" << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ftd.getVectors() are:            *****" << endl;
	for ( int i = 0; i < keyvecd.size(); i++){
		cout << " key: " << keyvecd[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << endl;

	
	ftd.clear();
	ftd.setEqualWithTol( 0.5);
	ftd.addPossibleKey( 1.0);
	ftd.addPossibleKey( 2.0);
	ftd.addPossibleKey( 3.0);
	ftd.addPossibleKey( 4.0);
	ftd.addPossibleKey( 5.0);
	ftd.increment( 1.1);
	ftd.increment( 1.0);
	ftd.increment( 2.49);
	ftd.increment( 3.0);
	ftd.increment( 1.2);
	ftd.increment( 5.0);
	ftd.increment( 1.501);
	ftd.getVectors( keyvecd, frevec);
	
	cout << "*******************************************************" << endl;
	cout << "Running ftd.clear(), running ftd.setEqualWithTol( 0.5)," << endl
	     << "running ftd.addPossibleKey()" << endl
		 << "to add keys 1.0, 2.0, 3.0, 4.0, 5.0," << endl
		 << "and running ftd.increment() several times" << endl
		 << "so that the following keys are incremented:" << endl
	     << "1.1, 1.0, 2.49, 3.0, 1.2, 5.0, 1.501" << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ftd.getVectors() are:            *****" << endl;
	for ( int i = 0; i < keyvecd.size(); i++){
		cout << " key: " << keyvecd[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << "***** ftd.getSumCount() is:                       *****" << endl;
	cout << ftd.getSumCount() << endl;
	cout << "***** ftd.meanFromFreq() is:                      *****" << endl;
	cout << ftd.meanFromFreq() << endl;
	cout << "***** ftd.medianFromFreq() is:                    *****" << endl;
	cout << ftd.medianFromFreq() << endl;
	cout << "***** Result of ftd.modeFromFreq( ret) is:        *****" << endl;
	vector <double> ret;
	ftd.modeFromFreq( ret);
	for ( auto num : ret){
		cout << num << " ";
	}
	cout << endl << endl;



	FreqType <double> ftd2;
	ftd2.setEqualWithTol( 0.5);
	ftd2.addPossibleKeys( { 1.0, 2.0, 3.0, 4.0, 5.0});
	// this can be written like below instead.
	// ftd2.addPossibleKeys( vector <double> ( { 1.0, 2.0, 3.0, 4.0, 5.0}));
	ftd2.addCount( 1.0, 2);
	ftd2.addCount( 2.0, 1);
	ftd2.addCount( 3.0, 1);
	ftd2.addCount( 4.0, 2);
	ftd2.getVectors( keyvecd, frevec);

	cout << "*******************************************************" << endl;
	cout << "Declaring FreqType <double> ftd2, " << endl
	     << "running ftd2.setEqualWithTol( 0.5), " << endl
	     << "and running ftd2.addPossibleKeys()" << endl
	     << "and ftd2.addCount() several times" << endl
	     << "so that the following frequencies are stored:" << endl
	     << "1.0 *2, 2.0 *1, 3.0 *1, 4.0 *2" << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ftd2.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvecd.size(); i++){
		cout << " key: " << keyvecd[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << "***** ftd2.getSumCount() is: *****" << endl;
	cout << ftd2.getSumCount() << endl;
	cout << "***** ftd2.meanFromFreq() is: *****" << endl;
	cout << ftd2.meanFromFreq() << endl;
	cout << "***** ftd2.medianFromFreq() is: *****" << endl;
	cout << ftd2.medianFromFreq() << endl;
	cout << "***** Results of ftd2.modeFromFreq( ret) is: *****" << endl;
	ftd2.modeFromFreq( ret);
	for ( auto num : ret){
		cout << num << " ";
	}
	cout << endl << endl;



	ftd.addFreqType( ftd2);
	ftd.getVectors( keyvecd, frevec);
	
	cout << "*******************************************************" << endl;
	cout << "Running ftd.addFreqType( ftd2)" << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ftd.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvecd.size(); i++){
		cout << " key: " << keyvecd[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << "***** ftd.getSumCount() is: *****" << endl;
	cout << ftd.getSumCount() << endl << endl;	



	FreqType <double, long long int> ftd3;
	ftd3.setEqualWithTol( 0.5);
	ftd3.addPossibleKeys( { 1.0, 2.0, 3.0, 4.0, 5.0});
	ftd3.addCount( 1.0, 2);
	ftd3.addCount( 2.0, 1);
	ftd3.addCount( 3.0, 1);
	ftd3.addCount( 4.0, 2);
	vector <long long int> ll_frevec;
	ftd3.getVectors( keyvecd, ll_frevec);
	
	cout << "*******************************************************" << endl;
	cout << "Example of counting by type \"long long int\"" << endl
	     << "Declaring FreqType <double, long long int> ftd3, " << endl
	     << "running ftd3.setEqualWithTol( 0.5)," << endl
	     << "and running ftd3.addPossibleKeys() " << endl
	     << "and ftd3.addCount() several times" << endl
	     << "so that the following frequencies are stored:" << endl
	     << "1.0 *2, 2.0 *1, 3.0 *1, 4.0 *2" << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ftd3.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvecd.size(); i++){
		cout << " key: " << keyvecd[ i] << " freq: " << ll_frevec[ i] << endl;
	}
	cout << endl;




	ftd3.clearCount();
	ftd3.getVectors( keyvecd, ll_frevec);
	cout << "*******************************************************" << endl;
	cout << "Doing ftd3.clearCount() " << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ftd3.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvecd.size(); i++){
		cout << " key: " << keyvecd[ i] << " freq: " << ll_frevec[ i] << endl;
	}
	cout << endl;



	ft.clear();
	ft.setEqualExactly();
	ft.addPossibleKeys( { 1, 2, 3, 4, 5, 6});
	ft.increment( 1);
	ft.increment( 2);
	ft.increment( 2);
	ft.increment( 5);
	ft.increment( 6);
	ft.increment( 6);
	ft.getVectors( keyvec, frevec);
	
	cout << "*******************************************************" << endl;
	cout << "Test for median calculation" << endl;
	cout << "Running ft.clear(), ft.setEqualExactly(), " << endl
	     << "and ft.increment() several times" << endl
	     << "so that the keys 1, 2, 2, 5, 6, 6 are incremented" << endl;
	cout << "*******************************************************" << endl;
	cout << "***** Results of ft.getVectors() are: *****" << endl;
	for ( int i = 0; i < keyvec.size(); i++){
		cout << " key: " << keyvec[ i] << " freq: " << frevec[ i] << endl;
	}
	cout << "***** ft.medianFromFreq() returns: *****" << endl;
	cout << ft.medianFromFreq() << endl << endl;



	

	
	return 0;
	
}


/* ********** Definitions of Member Functions ********** */

