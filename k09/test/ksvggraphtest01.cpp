
/*
	
	ksvggraphtest01.cpp 

	Test for ksvggraph00.cpp in k09
		
*/


/* ********** Preprocessor Directives ********** */

#include <k09/ksvggraph00.cpp>


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
	
	cout << "ksvggraphtest01.cpp" << endl << endl;

	cout << "Testing getGridPoints()" << endl << endl;

	// ちょうどいい間隔と基準点の実験。
	{
		cout << "Results of getGridPoints( -12.34, 567.8): " << endl;
		vector <double> gridpoints = getGridPoints( -12.34, 567.8);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}
	
	{
		cout << "Results of getGridPoints( -12.34, 567.8, 4, false, false): " << endl;
		vector <double> gridpoints = getGridPoints( -12.34, 567.8, 4, false, false);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}
	
	{
		cout << "Results of getGridPoints( -1234.5, 567.8): " << endl;
		vector <double> gridpoints = getGridPoints( -1234.5, 567.8);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		cout << "Results of getGridPoints( -1234.5, 567.8, 5): " << endl;
		vector <double> gridpoints = getGridPoints( -1234.5, 567.8, 5);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		cout << "Results of getGridPoints( 123.5, 5678.9): " << endl;
		vector <double> gridpoints = getGridPoints( 123.5, 5678.9);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		cout << "Results of getGridPoints( -80001.0, -299.9): " << endl;
		vector <double> gridpoints = getGridPoints( -80001.0, -299.9);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		cout << "Results of getGridPoints( -80001.0, -299.9, 5): " << endl;
		vector <double> gridpoints = getGridPoints( -80001.0, -299.9, 5);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	return 0;

}


/* ********** Definitions of Member Functions ********** */


/* ********** Endings of Namespace / Preprocessor Directives ********** */
