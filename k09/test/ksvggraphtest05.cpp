
/*
	
	ksvggraphtest05.cpp 

	Test for ksvggraph00.cpp in k09

	Drawing histogram
	ヒストグラムを描く。

	ksvggraphtest05_input.csvは、Reed-Smith SMDデータ。
		
*/


/* ********** Preprocessor Directives ********** */

#include <iostream> 
#include <string> 
#include <vector> 

#include <k09/koutputfile01.cpp>
#include <k09/ksvggraph00.cpp>
#include <k09/kdataset03.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);
std::vector <double> readVshareLdp2014( const std::string &);



/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int, char *[])
{
	
	using namespace std;

	const string filename = "ksvggraphtest05_input.csv";

	vector <double> dvec_valid = readVshareLdp2014( filename);


	// Making histogram for a continuous variable
	// ヒストグラムをつくる。

	{
		SvgHistogramMaker histm( dvec_valid);
		histm.setGraphTitle( u8"ヒストグラム - 2014年自民党候補者の得票率");
		histm.setXAxisTitle( u8"得票率");
		histm.setYAxisTitle( u8"ケース数");
		SvgGraph svgg = histm.createGraph();
		svgg.writeFile( "ksvggraphtest05_out01.svg");
	}


	// Making animated version of histogram
	// アニメーション版のヒストグラム

	{
		SvgHistogramMaker histm( dvec_valid, true); 
		histm.setGraphTitle( u8"ヒストグラム - 2014年自民党候補者の得票率");
		histm.setXAxisTitle( u8"得票率");
		histm.setYAxisTitle( u8"ケース数");
		SvgGraph svgg = histm.createGraph();
		svgg.writeFile( "ksvggraphtest05_out02.svg");
	}

	return 0;

}

std::vector <double> readVshareLdp2014( const std::string &filename)
{

	vector <double> dvec;
	vector <double> dvec_valid;

	Dataset ds;
	bool b;

	cout << "Reading data...";

	b = ds.readCsvFile( "smddata.csv");
	if ( b == false){
		alertExit( "test");
		return dvec_valid;
	}
	cout << "Done." << endl;

	cout << "Fixing variable types...";
	int nnum, nmis;
	ds.fixVariableType( nnum, nmis);	
	cout << "Done." << endl;
	
	cout << "Getting numeric vector before specifying missing...";
	b = ds.getNumericVectorWithoutMissing( dvec, "vshare");
	if ( b == false){
		alertExit( "test");
		return dvec_valid;
	}
	cout << "Done." << endl;

	// 2014年の自民のみに。	
	cout << "Specifying valid cases, which are LDP candidates in 2014...";
	auto filterfunc = 
		[]( const CaseMapType &v)->bool
		{

			bool ret = false;

			if ( v[ "party_id"] == 1 && v[ "year"] == 2014){
				ret = true;
			}
			
			return ret;

		};
	
	ds.specifyValid( "vshare", filterfunc);
	cout << "Done." << endl;


	cout << "Getting numeric vector excl. missing...";
	b = ds.getNumericVectorWithoutMissing( dvec_valid, "vshare");
	if ( b == false){
		alertExit( "test");
		return dvec_valid;
	}
	cout << "Done." << endl;

	return dvec_valid;

}


/* ********** Definitions of Member Functions ********** */

