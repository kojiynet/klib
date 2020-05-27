
/*
	
	kutiltest03.cpp 

	testing Timer class
		
*/


/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <vector>
#include <k09/kutil02.cpp>
#include <k09/krand00.cpp>


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
	

	cout << "Test 1" << endl;

	Timer tm{}; // automatically start to measure
	cout << "Timer has started." << endl;

	// do something
	{
		int n = 1000000;
		vector <int> intvec( n);
		RandomNumberEngine rne( 123);
		for ( int i = 0; i < n; i++){
			intvec[ i] = rne.getDiscreteUniform( 1, 1000);
			if ( intvec[ i] == 1000){
				cout << ".";
			}
		}
		cout << endl;
	}

	tm.markEnd(); // this does not reset the timer
	cout << "While doing something, " << tm.getInterval() << " milliseconds passed." << endl;

	// do something
	{
		int n = 1000000;
		vector <int> intvec( n);
		RandomNumberEngine rne( 123);
		for ( int i = 0; i < n; i++){
			intvec[ i] = rne.getDiscreteUniform( 1, 1000);
			if ( intvec[ i] == 1){
				cout << ".";
			}
		}
		cout << endl;
	}

	tm.markEnd();
	cout << "While doing something and also some other thing, " << tm.getInterval() << " milliseconds passed." << endl;
	cout << endl;


	cout << "Test 2" << endl;	
	tm.restart();
	cout << "Timer has restarted." << endl;
	cout << "Before tm.markEnd(), tm.getInterval() returns: " << tm.getInterval() << endl;
	tm.markEnd();
	cout << "After tm.markEnd(), tm.getInterval() returns: " << tm.getInterval() << endl;


	return 0;


}


/* ********** Definitions of Member Functions ********** */


/* ********** Endings of Namespace / Preprocessor Directives ********** */
