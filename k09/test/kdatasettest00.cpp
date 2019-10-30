
/*
	
	kdatasettest00.cpp
	
	Written by Koji Yamamoto
	Copyright (C) 2018-2019 Koji Yamamoto
	
	Testing kdataset Ver.k09.00
	
*/


/* ********** Preprocessor Directives ********** */

// define this syboml to make "printTest()" available
#define kdataset_cpp_test
	#include <k09/kdataset00.cpp>
#undef kdataset_cpp_test

#include <iostream> 


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);
void testEffectiveDigits( void);
void testDatacolumn( void);
void testDataset( void);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int, char *[])
{
	
	testEffectiveDigits();

	testDatacolumn();
		
	testDataset();

	// Datasetに、
	// printの類が未実装。
	// あと明示的にファイルに書き出すものもつくる。
	
}

void testEffectiveDigits( void)
{
	
	cout << endl
	     << "***** Testing \"effectiveDigits()\" function *****" << endl
		 << endl;

	/*
	
	// 以下をeffectiveDigits()用のテストプログラムに入れていく。
	
	string str;
	int ld, sd;
	
	str = "-1.234e+3";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	str = "-123e-3";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	str = "4234.2334";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	str = "4234.00000";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	str = "40200000";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	str = "002340.0560";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	str = "0.000004020";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	str = ".004020";
	b = effectiveDigits( ld, sd, str);
	cout << "str: " << str << " ReturnValue: " << b << " larged: " << ld << " smalld: " << sd << endl;
	
	*/ 

}

void testDatacolumn( void)
{

	vector <string> svec;
	int nmisret;
	Datacolumn dc;
	
	cout << endl
	     << "***** Testing \"Datacolumn\" class *****" << endl
		 << endl;

	svec.clear();
	svec.push_back( "123");
	svec.push_back( "456.78");
	svec.push_back( "-0.0912300e-12");
	svec.push_back( "   456.78   ");
	
	dc.setData( "Var1", svec);
	dc.print();
	cout << endl;
	
	bool b = dc.convertToNumeric( nmisret);
	cout << "return value of convertToNumeric( nmisret): " << b << endl;
	cout << "\"nmisret\" after convertToNumeric( nmisret): " << nmisret << endl
	     << "Note: this \"nmisret\" stands for N of missing cases." << endl;
	cout << endl;

	dc.print();
	cout << endl;
	
	
	// integer example
	
	svec.clear();
	svec.push_back( "123");
	svec.push_back( "456.78E2");
	svec.push_back( "-0.0912300e+5");
	svec.push_back( "   45678   ");
	
	dc.setData( "Var2", svec);
	dc.print();
	cout << endl;
	
	b = dc.convertToNumeric( nmisret);
	cout << "return value of convertToNumeric( nmisret): " << b << endl;
	cout << "\"nmisret\" after convertToNumeric( nmisret): " << nmisret << endl;
	dc.print();
	cout << endl;
	
	
	// missing example 1
	
	svec.clear();
	svec.push_back( "123");
	svec.push_back( ".");
	svec.push_back( "");
	svec.push_back( "      ");
	svec.push_back( "-0.0912300e+5");
	
	dc.setData( "Var3", svec);
	dc.print();
	cout << endl;
	
	b = dc.convertToNumeric( nmisret);
	cout << "return value of convertToNumeric( nmisret): " << b << endl;
	cout << "\"nmisret\" after convertToNumeric( nmisret): " << nmisret << endl;
	dc.print();
	cout << endl;
	
	// missing example 2
	
	svec.clear();
	svec.push_back( "\t");
	svec.push_back( ".");
	svec.push_back( "");
	svec.push_back( "      ");
	svec.push_back( " \t ");
	
	dc.setData( "Var4", svec);
	dc.print();
	cout << endl;
	
	b = dc.convertToNumeric( nmisret);
	cout << "return value of convertToNumeric( nmisret): " << b << endl;
	cout << "\"nmisret\" after convertToNumeric( nmisret): " << nmisret << endl;
	dc.print();
	cout << endl;

}

void testDataset( void)
{

	Dataset ds;
	bool b;
	
	string fn = "kdataset_test.csv";
	b = ds.readCsvFile( fn);
	cout << "Reading the file: " << fn << " -> success/failure: " << b << endl;
	cout << endl;
	ds.printTest();
	cout << endl;
	cout << "Return value of ds.getNVar(): " << ds.getNVar() << endl;
	cout << "Return value of ds.getNCase(): " << ds.getNCase() << endl;
	
	int nvarconv, nvarmis;
	ds.fixVariableType( nvarconv, nvarmis);
	
	cout << "After fixVariableType()" << endl;
	cout << endl;
	cout << "N of Vars Converted: " << nvarconv << "; N of Vars With Missing: " << nvarmis << endl;
	cout << endl;
	ds.printTest();
	
	vector <double> dvec;
	
	b = ds.getNumericVector( dvec, "var1");
	cout << "Return value of ds.getNumericVector( \"var1\", dvec): " << b << endl;
	cout << "Contents of dvec: " << endl;
	for ( auto v : dvec){
		cout << " " << v;
	}
	cout << endl << endl;
	
	// 各行をvectorにしてそれをvectorにして返せる
	vector <string> varnames;
	vector < vector <double> > rowvecs;
	b = ds.getRowVectors( varnames, rowvecs);
	cout << "Return value of ds.getRowVectors( varnames, rowvecs): " << b << endl;
	if ( rowvecs.size() > 0 && varnames.size() == rowvecs[ 0].size()){
		
		cout << "Varnames:";
		for ( int i = 0; i < varnames.size(); i++){
			cout << " " << varnames[ i];
		}
		cout << endl;
		
		for ( int i = 0; i < rowvecs.size(); i++){
			cout << " Values:";
			for ( auto v : rowvecs[ i]){
				cout << " " << v;
			}
			cout << endl;
		}
	}
	cout << endl;
	
	// print()する
	cout << "Using print()" << endl << endl;
	ds.print(); // ←これが、dsが空の場合（ファイルが取得できなかった場合）におかしくなる。
	cout << endl;
	
	// titleを入れてprint()する
	cout << "Using print() with title" << endl << endl;
	ds.setTitle( "Sample Dataset");
	ds.print();
	cout << endl;

}


/* ********** Definitions of Member Functions ********** */

