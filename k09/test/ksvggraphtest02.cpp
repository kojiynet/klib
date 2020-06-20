
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

	// making histogram 
	SvgHistogramMaker histm( dvec);
	histm.setGraphTitle( "Frequency - Standard Normal Random, n = 10000");
	histm.setXAxisTitle( "x");
	histm.setYAxisTitle( "#Cases");
	SvgGraph svgg = histm.createGraph();

	// adding element by theoretical coordinate
	GraphLine l0( -2, 0, 2, 0);
	l0.setStroke( "red");
	l0.setStrokewidth( 4);
	svgg.addElement( l0);

	svgg.writeFile( "ksvggraphtest02out1.svg");


	cout << "Testing class SvgScatterMaker" << endl << endl;

	// preparing values
	vector <double> dvec2( n);
	for ( int i = 0; i < n; i++){
		dvec2[ i] = dvec[ i] + rne.getNormal();
	}

	// making scatterplot
	SvgScatterMaker scatterm( dvec, dvec2);
	scatterm.setGraphTitle( "Scatterplot - Bivariate Normal Random, n = 10000");
	scatterm.setXAxisTitle( "x");
	scatterm.setYAxisTitle( "y");
	SvgGraph svgg2 = scatterm.createGraph();

	// adding element by theoretical coordinate
	GraphLine l1( -4, -4, 6, 6);
	l1.setStroke( "green");
	l1.setStrokewidth( 2);
	svgg2.addElement( l1);

	svgg2.writeFile( "ksvggraphtest02out2.svg");

	return 0;

}


/* ********** Definitions of Member Functions ********** */


/* ********** Endings of Namespace / Preprocessor Directives ********** */
