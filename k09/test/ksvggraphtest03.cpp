
/*
	
	ksvggraphtest03.cpp 

	Test for ksvggraph00.cpp in k09

	Actually, this makes an example SVG file for explanation
		
*/


/* ********** Preprocessor Directives ********** */

#include <k09/ksvggraph00.cpp>
#include <k09/krand00.cpp>
#include <map>


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
	
	cout << "ksvggraphtest03.cpp" << endl << endl;

	// preparing values
	RandomNumberEngine rne( 123);
	int n = 10000;
	vector <double> dvec( n);
	for ( double &v : dvec){
		v = rne.getNormal();
	}

	// making histogram 
	SvgHistogramMaker histm( dvec);
	histm.setGraphTitle( "Frequency");
	histm.setXAxisTitle( "x");
	histm.setYAxisTitle( "#Cases");

	map <string, double> args = {
		{ "svgwidth",             500},
		{ "svgheight",            500},
		{ "outermargin",           30},
		{ "graph_title_fontsize",  25},
		{ "graph_title_margin",    20},
		{ "axis_title_fontsize",   20},
		{ "axis_title_margin",     20},
		{ "axis_label_fontsize",   20},
		{ "axis_ticklength",       20},
	};

	histm.modifyMetrics( args);

	SvgGraph svgg = histm.createGraph();

	svgg.writeFile( "ksvggraphtest03out.svg");

	return 0;

}


/* ********** Definitions of Member Functions ********** */


/* ********** Endings of Namespace / Preprocessor Directives ********** */
