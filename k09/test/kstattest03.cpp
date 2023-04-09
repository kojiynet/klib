
/*
	
	kstattest03.cpp
	
	Written by Koji Yamamoto
	Copyright (C) 2023 Koji Yamamoto
	
	Testing kstat Ver.k09.03

	Showing mean, median, and making frequency table using recode table

	For g++ use following options:
		"-std=c++17",
		"-I/home/ky/cpp/copied/koli",
		"-lpthread",
		"-lboost_filesystem",
		"-lboost_thread",
		"-lboost_chrono",
	
*/


/* ********** Preprocessor Directives ********** */

#include <iostream>

#include <k09/kstat03.cpp>
#include <k09/kdataset03.cpp>


/* ********** Using Directives ********** */

//using namespace std;


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

	using namespace std;
	
	cout << "kstattest03.cpp" << endl
	     << "Testing kstat.cpp Ver.k09.03" << endl << endl;
	

	Dataset ds;
	bool b;

	cout << "Reading data...";

	// kstattest03_input.csvは、Reed-Smithデータのうち、2014年自民党候補に限定したもの。
	// kstattest03_input.csv: Reed-Smith SMD Dataset, limited to 2014 LDP candidates
	b = ds.readCsvFile( "kstattest03_input.csv");
	if ( b == false){
		return 0;
	}
	cout << "Done." << endl;

	cout << "Fixing variable types...";
	int nnum, nmis;
	ds.fixVariableType( nnum, nmis);	
	cout << "Done." << endl;
	
	vector <double> dvec;

	cout << "Getting numeric vector...";
	b = ds.getNumericVectorWithoutMissing( dvec, "vshare");
	if ( b == false){
		return 0;
	}
	cout << "Done." << endl;

	// testing mean() and median()

	cout << endl;
	cout << "vshare for 2014 LDP candidates" << endl;
	cout << endl;
	cout << "Calculated by mean() and median()" << endl;
	cout << "Mean:   " << setprecision( 15) << mean( dvec) << endl;
	cout << "Median: " << median( dvec) << endl;
	cout << endl;

	// 度数分布表
	// Making Frequency Table

	cout << "Number of unique values: " << countUniqueValues( dvec) << endl;
	cout << endl;

	RecodeTable <double, int> rt;
	rt.setAutoTableFromContVar( dvec); 

	cout << "RecodeTable:" << endl;
	rt.print( cout, ","); 
	cout << endl;

	FreqType <int, int> ft;
	ft.setFreqFromRecodeTable( dvec, rt);

	cout << "Frequency Table:" << endl;
	ft.printPadding( cout);
	cout << endl;


	return 0;
	
}


/* ********** Definitions of Member Functions ********** */

