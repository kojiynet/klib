
/*
	
	ksvgtest01.cpp 
		
*/


/* ********** Preprocessor Directives ********** */

#include <k09/ksvg00.cpp>


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
	
	SvgFile svgf( 400, 400, 0, 0, 400, 400);

	SvgRect r0( 100, 100, 200, 200);
	r0.addFill( "blue");
	r0.addStroke( "lightblue");
	r0.addStrokewidth( 10);
	svgf.addElement( r0);

	SvgCircle c0( 350, 350, 100);
	c0.addFill( "pink");
	c0.addStroke( "pink");
	c0.addFillopacity( 0.5);
	svgf.addElement( c0);

	SvgEllipse e0( 50, 50, 100, 10);
	e0.addStrokewidth( 0);
	e0.addFill( "green");
	svgf.addElement( e0);

	SvgLine l0( 300, 10, 400, 400); 
	l0.addStroke( "black");
	svgf.addElement( l0);

	SvgText t0( 200, 200, 0.123);
	t0.addFontfamily( "Century");
	t0.addFontsize( 50);
	t0.addTextanchor( "middle"); 
	t0.addDominantbaseline( "alphabetic"); 
	t0.addRotate( 30, 200, 200);
	svgf.addElement( t0);	

	svgf.writeFile( "ksvgtest01out01.svg");

	return 0;

}


/* ********** Definitions of Member Functions ********** */


/* ********** Endings of Namespace / Preprocessor Directives ********** */
