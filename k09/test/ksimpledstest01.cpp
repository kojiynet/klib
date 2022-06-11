
/*

	ksimpleds01.cpp

	Test for ksimpleds00.cpp in k09

	cl compiler options:
	-EHsc -utf-8 -std:c++17

*/


/* ********** Preprocessor Directives ********** */

#include <string>
#include <vector>
#include <iostream>
#include <k09/kstat03.cpp>
#include <k09/kstatboost02.cpp>

#include <k09/ksimpleds00.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);
int mainproc( void);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{
	int ret = 0;

	try {

		ret = mainproc();

	} catch( std::exception &e){

		cout << e.what() << endl;

	}

	return ret;

}

int mainproc( void)
{

	using namespace std;

	string output_fn = "ksimpledstest01out_stat.txt";

	// カテゴリごとの母集団サイズ
	int popsize_category1 = 100'000;
	int popsize_category2 = 100'000;

	// サンプリング回数の指定（サンプルサイズごとに）
	int nsim = 3; 

	// サンプリング時のサンプルサイズの指定
	vector <int> nobsvec;
	nobsvec.resize( 998);
	std::iota( nobsvec.begin(), nobsvec.end(), 3);


	cout << endl << "ksimpledstest01.cpp" << endl << endl;


	// 母集団の生成

	cout << "Generating random numbers..." << endl;

	RandomNumberEngine rne( 123); 

	double mu;
	double sigma;

	vector <double> vals_category1;
	vector <double> vals_category2;
	mu = 0.4;
	sigma = 0.1;
	getBetaRandomVec( vals_category1, rne, popsize_category1, mu, sigma * sigma, true);
	mu = 0.6;
	sigma = 0.2;
	getBetaRandomVec( vals_category2, rne, popsize_category2, mu, sigma * sigma, true);

	
	// 母集団データをSimpleDatasetにつめる

	Timer tm;

	tm.restart();

	SimpleDataset popds; 
	vector <string> popvnvec { "id", "category", "value"};
	popds.setVarNames( popvnvec);

	popds.addSequentialNumber( "id", 0.0, popsize_category1);
	popds.addConstant( "category", 1.0, popsize_category1);
	popds.addVector( "value", vals_category1);
	popds.assertRecutangular();

	popds.addSequentialNumber( "id", popds.size(), popsize_category2);
	popds.addConstant( "category", 2.0, popsize_category2);
	popds.addVector( "value", vals_category2);
	popds.assertRecutangular();

	tm.markEnd();
	
	cout << "Duration for Storing Population: " << tm.getInterval() << " millisecond" << endl << endl;


	// 母集団から選別する。

	cout << "Selecting from population..." << endl;

	vector <double> all_vec = popds.getVector( "value");
	
	// 属性ごとのvectorをつくる。
	// getVectorIf()を使う。
	// この際の選別の方法を異にしてみて、それぞれにかかる時間を計っている。

	// mapを使うバージョン
	tm.restart();
	vector <double> category1_vec = 
		popds.getVectorIf( 
			"value",
			[]( std::map <std::string, double> v)->bool { return ( std::round( v.at( "category")) == 1.0); }
		);
	tm.markEnd();
	cout << "Duration 1: " << tm.getInterval() << " millisecond" << endl;
	
	// 選別に1変数を用いる場合で、変数名を指定するバージョン。
	tm.restart();
	vector <double> category2_vec =
		popds.getVectorIf( 
			"value", 
			[]( std::function <double(std::string)> v)->bool { return ( std::round( v( "category")) == 2.0); }
		);
	tm.markEnd();
	cout << "Duration 2: " << tm.getInterval() << " millisecond" << endl;

	// あらかじめ、選別に使用する変数を指定しておくバージョン。
	tm.restart();
	vector <double> category1_vec_2 =
		popds.getVectorIf( 
			"value",   // result to get 
			"category", // variable to use 
			[]( double category){ return ( std::round( category) ==  1.0); }
		);
	tm.markEnd();
	cout << "Duration 3: " << tm.getInterval() << " millisecond" << endl << endl;


	// 母集団統計量を算出する。

	tm.restart();

	cout
		<< "mean            " << mean( all_vec) << endl
		<< "mean category1  " << mean( category1_vec) << endl
		<< "mean category2  " << mean( category2_vec) << endl
		<< "var(all)        " << unbiasedVarBoost( all_vec) << endl
		<< "var(category1)  " << unbiasedVarBoost( category1_vec) << endl
		<< "var(category2)  " << unbiasedVarBoost( category2_vec) << endl
		<< "N               " << all_vec.size() << endl
		<< "N category1     " << category1_vec.size() << endl
		<< "N category1     " << category1_vec.size() << endl
	;

	tm.markEnd();

	cout << "Duration for calculating pop stats: " << tm.getInterval() << " millisecond" << endl << endl;


	// サンプリングする。
	// そのたびに統計量を算出して記録する。

	cout << "Sampling..." << endl;

	SimpleDataset resultds; 
	vector <string> vnvec { 
		"nobs", "iter",
		"n_category1", "n_category2", 
		"mean", "mean_category1", "mean_category2", 
		"category_diff"
	};	
	resultds.setVarNames( vnvec);

	RandomNumberEngine rne_to_sample( 456);

	for ( int nobs : nobsvec){

		for ( int j = 0; j < nsim; j++){

			SimpleDataset sampleds = popds.getSample( rne_to_sample, nobs);

			vector <double> vec = sampleds.getVector( "value");

			vector <double> vec_cat1 = 
				sampleds.getVectorIf(
					"value", "category",
					[]( double v){ return ( std::round( v) == 1.0); }
				);
			
			vector <double> vec_cat2 = 
				sampleds.getVectorIf(
					"value", "category",
					[]( double v){ return ( std::round( v) == 2.0); }
				);
			
			double mean_cat1   = mean( vec_cat1);
			double mean_cat2   = mean( vec_cat2);
			resultds.addCase( "nobs",           nobs);
			resultds.addCase( "iter",           j);
			resultds.addCase( "n_category1",    vec_cat1.size());
			resultds.addCase( "n_category2",    vec_cat2.size());
			resultds.addCase( "mean",           mean( vec));
			resultds.addCase( "mean_category1", mean_cat1);
			resultds.addCase( "mean_category2", mean_cat2);
			resultds.addCase( "category_diff",  mean_cat1 - mean_cat2);
			resultds.assertRecutangular();
		}

	}

	cout << "Ended Sampling." << endl << endl;


	// 結果をファイルに書き込む。

	cout << "Writing to file..." << endl;

	koutputfile kof( output_fn);
	bool s = kof.open( false, false, true);
	if ( s == true){
		
		resultds.writeToFile( kof);
		kof.close();

	}

	cout << "Ended writing to file." << endl << endl;

	return 0;

}


/* ********** Definitions of Member Functions ********** */ 

