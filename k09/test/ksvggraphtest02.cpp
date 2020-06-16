
/*
	
	ksvggraphtest02.cpp 

	Test for ksvggraph00.cpp in k09
		
*/


/* ********** Preprocessor Directives ********** */

#include <k09/ksvggraph00.cpp>
#include <k09/krand00.cpp>
#include <k09/kstat02.cpp>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{
	
	cout << "ksvggraphtest02.cpp" << endl << endl;

	cout << "Testing class SvgHistogramMaker" << endl << endl;

	// preparing values
	RandomNumberEngine rne( 123);
	int n = 10000;
	vector <double> dvec( n);
	for ( double &v : dvec){
		v = rne.getNormal();
	}

	// この下での作業が多すぎないか…。
	// histmに直接vectorを入れることもできるように。
	// histmにftを入れることもできるように。
	// ftに入れることもできるように。
	// →その後、stestを直す。

	// making freq table
	RecodeTable <double, int> rt;
	rt.setAutoTableFromContVar( dvec);
	FreqType <int, int> ft;
	ft.setFreqFromRecodeTable( dvec, rt);
	cout << "Resulted Frequency Table: " << endl;
	ft.printPadding( cout);

	// making histogram
	vector <int> codes;
	vector <int> counts;
	vector <double> leftvec;
	vector <double> rightvec;
	ft.getVectors( codes, counts);
	ft.getRangeVectors( leftvec, rightvec);
	SvgHistogramMaker histm( leftvec, rightvec, counts);
	histm.setGraphTitle( "Frequency - Standard Normal Random, n = 10000");
	histm.setXAxisTitle( "x");
	histm.setYAxisTitle( "#Cases");
	SvgGraph svgg = histm.createGraph();
	svgg.writeFile( "ksvggraphtest02out.svg");

	return 0;

}


/* ********** Definitions of Member Functions ********** */


/* ********** Endings of Namespace / Preprocessor Directives ********** */
