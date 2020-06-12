
/*
	
	stest01.cpp

	koliのうち統計関係部分のテスト
	
	Written by Koji Yamamoto
	Copyright (C) 2019-2020 Koji Yamamoto
	
	TODO:　

	ksvggraph.cppのテストを書く。

	-std:c++17でコンパイルが通るかやってみる。
	gccでも。

	kstat.cppのテストを書く。

	kdatasetに戻る。

	Nelder-Meadを実装。




	↓以下は前から書いてあるもの。
	度数分布表をつくる。kstatを見て。
	　別に、連続変数用の機能をつける。
	　　start/end, width, bin を指定する方式。
	　　自動で、スタージェスの公式を使う方式。
	　　階級の端点の表を与える方式。
	
*/


/* ********** Preprocessor Directives ********** */

#include <k09/kdataset01.cpp>
#include <k09/kstat02.cpp>
#include <k09/koutputfile00.cpp>
#include <k09/ksvggraph00.cpp>
#include <k09/krand00.cpp>
#include <iostream> 
#include <iomanip>
#include <algorithm>

#include <boost/algorithm/string.hpp>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);

SvgGraph calculatePi( double);

SvgGraph createScatterAndCircle(
	const std::vector <double> &xvec,
	const std::vector <double> &yvec
);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int, char *[])
{
	
	vector <double> dvec;
	vector <double> dvecclean;

	{
		Dataset ds;
		bool b;

		cout << "Reading data...";
		b = ds.readCsvFile( "jhpsmerged_191029_v403.csv");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;

		cout << "Fixing variable types...";
		int nnum, nmis;
		ds.fixVariableType( nnum, nmis);	
		cout << "Done." << endl;
		
		cout << "Getting numeric vector before specifying missing...";
		b = ds.getNumericVectorWithoutMissing( dvec, "v403");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;
		
		cout << "Specifying missing cases and excluding very big values...";
		ds.specifyValid( 
			"v403",
			[]( double v)->bool{ return ( v < 99999.0 && v < 2500.0);}
		);
		cout << "Done." << endl;

		cout << "Getting numeric vector excl. missing...";
		b = ds.getNumericVectorWithoutMissing( dvecclean, "v403");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;
		
	}

	cout << endl;
	cout << "***************************************************" << endl;
	cout << "JHPS 2009 Household Income incl. Tax" << endl;
	cout << "Calculated by mean() and median()" << endl;
	cout << "Mean:   " << setprecision( 15) << mean( dvecclean)   << " (Ten Thousand Yen)" << endl;
	cout << "Median: " << median( dvecclean) << " (Ten Thousand Yen)" << endl;
	cout << "***************************************************" << endl;
	cout << "FYI: Mean from \"dirty\" data: " << setprecision( 15) << mean( dvec) << endl;


	// 度数分布表

	cout << endl;
	cout << "Number of unique values: " << countUniqueValues( dvecclean) << endl;
	cout << "FYI Number of unique values in \"dirty\" vector: " << countUniqueValues( dvec) << endl << endl;


	RecodeTable <double, int> rt;
	rt.setAutoTableFromContVar( dvecclean); 

	cout << "RecodeTable:" << endl;
	rt.print( cout, ","); 
	cout << endl;

	FreqType <int, int> ft;
	ft.setFreqFromRecodeTable( dvecclean, rt);

	ft.printPadding( cout);


	// ヒストグラムをつくる。

	vector <int> codes;
	vector <int> counts;
	vector <double> leftvec;
	vector <double> rightvec;
	ft.getVectors( codes, counts);
	ft.getRangeVectors( leftvec, rightvec);

	{
		SvgHistogramMaker histm( leftvec, rightvec, counts);
		histm.setGraphTitle( "Frequency - restricted to v less than 2500");
		histm.setXAxisTitle( "Household Income");
		histm.setYAxisTitle( "#Cases");
		SvgGraph svgg = histm.createGraph();
		svgg.writeFile( "stest01out01.svg");
	}
	{
		SvgHistogramMaker histm( leftvec, rightvec, counts, true); // アニメバージョン
		histm.setGraphTitle( "Frequency - restricted to v less than 2500");
		histm.setXAxisTitle( "Household Income");
		histm.setYAxisTitle( "#Cases");
		SvgGraph svgg = histm.createGraph();
		svgg.writeFile( "stest01out02.svg");
	}

	{
		SvgGraph svgg = calculatePi( 4000);
		svgg.writeFile( "stest01out03.svg");
	}
	
	return 0;



	// 今のRecodeTableには、左端・右端がない（無限大）という指定ができない。

	// FreqTypeにはすごく小さい機能だけを持たせることにして、
	// 別にFreqTableTypeか何かをつくって、そこに、RecodeTableを持たせたり、
	// それをもとにしたFreqを作らせたりしてもよいかも。

	/*
	度数分布表をつくる。kstatを見て。
	　別に、連続変数用の機能をつける。
	　　start/end, width, bin を指定する方式。
	　　自動で、スタージェスの公式を使う方式？
	　　※Stataでは、min{ sqrt(N), 10*ln(N)/ln(10)}らしいので、それでいく。
	　　階級の端点の表を与える方式。
	*/

}

SvgGraph calculatePi( double n)
{

	std::vector <double> xvec;
	std::vector <double> yvec;
	xvec.reserve( n);
	yvec.reserve( n);

	for ( int i = 0; i < n; i++) {
    	xvec.push_back( randomUniform( -1.0, 1.0));
    	yvec.push_back( randomUniform( -1.0, 1.0));
	}	

	int ninside = 0;
	for ( int i = 0; i < n; i++){
		if ( xvec[ i] * xvec[ i] + yvec[ i] * yvec[ i] < 1.0){
			ninside++;
		}
	}
	std::cout << "N of Draws: " << n << std::endl;
	std::cout << "Estimated PI: " << ( ( double)ninside / n ) * 4.0 << std::endl << std::endl;	

	SvgGraph svgg = createScatterAndCircle( xvec, yvec);
	
	return svgg;
	
}

SvgGraph createScatterAndCircle(
	const std::vector <double> &xvec,
	const std::vector <double> &yvec
)
{

	std::string graph_title = "Random Numbers and Circle";
	std::string xaxis_title = "x";
	std::string yaxis_title = "y";

	SvgScatterMaker maker( xvec, yvec);
	maker.setGraphTitle( graph_title);
	maker.setXAxisTitle( xaxis_title); 	
	maker.setYAxisTitle( yaxis_title); 

	auto svgg = maker.createGraph(); 

	GraphEllipse el( 0, 0, 1, 1);
	el.setFill( "none");
	el.setStroke( "red");
	el.setStrokewidth( 3);
	svgg.addElement( el);

	return svgg; 

}


/* ********** Definitions of Member Functions ********** */


