
/*
	
	kdataman_test01.cpp

	kdataman.cppのテスト

	→CSVファイルを簡略化する。

	
	cl compile command:
	cl -EHsc -utf-8 -std:c++17 kdataman_test01.cpp 

	before running this, we may need to do:
	chcp65001

*/


/* ********** Preprocessor Directives ********** */

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <variant>
#include <k09/kutil02.cpp>
#include <k09/kinputfile01.cpp>
#include <k09/koutputfile01.cpp>
#include <k09/kdataset03.cpp>
#include <k09/kstr00.cpp>
#include <k09/kdataman00.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{
	
	using namespace std;
	
	const string fn_input    = "kdataman_test01_input.csv";
	const string fn_codes_sa = "kdataman_test01_sacodes.csv";
	const string fn_codes_ma = "kdataman_test01_macodes.csv";
	const string fn_output   = "kdataman_test01_out1.txt";
	const string fn_output2  = "kdataman_test01_out2.txt";

	Dataset ds_input;
	Dataset ds_codes_sa;
	Dataset ds_codes_ma;

	// データを読む
	cout << "Reading data...";
	bool b;
	b = ds_input.readCsvFile( fn_input);
	if ( b == false){
		alert( "main()", "File not found");
		return 0;
	}
	b = ds_codes_sa.readCsvFile( fn_codes_sa);
	if ( b == false){
		alert( "main()", "File not found");
		return 0;
	}
	b = ds_codes_ma.readCsvFile( fn_codes_ma);
	if ( b == false){
		alert( "main()", "File not found");
		return 0;
	}
	cout << "Done." << endl;
	// fixVariableTypeしない。stringのままでいい。


	// ********************
	// コード対応のファイルから、vectorをとってきて、
	// それをオブジェクトに与えていく方法

	// SAコード変換ルールのファイルから情報を読む

	std::vector <string> sa_varvec;
	std::vector <string> sa_oldvec;
	std::vector <string> sa_newvec;

	// varは変数名の列、stringは選択肢の文字列表記の列、codeはコードの列、だと仮定
	ds_codes_sa.getStringVector( sa_varvec, "var");
	ds_codes_sa.getStringVector( sa_oldvec, "string");
	ds_codes_sa.getStringVector( sa_newvec, "code");

	// MAコード変換ルールのファイルから情報を読む

	std::vector <string> ma_varvec;
	std::vector <string> ma_oldvec;
	std::vector <string> ma_dummyvarvec;

	// varは変数名の列、stringは選択肢の文字列表記の列、newvarは対応するダミー変数の列、だと仮定
	ds_codes_ma.getStringVector( ma_varvec, "var");
	ds_codes_ma.getStringVector( ma_oldvec, "string");
	ds_codes_ma.getStringVector( ma_dummyvarvec, "newvar");


	RepRuleSet reprules;

	// SAのルールを追加
	reprules.addSARepRules( sa_varvec, sa_oldvec, sa_varvec, sa_newvec);
	// 変換できなかった文字列に対応させてvname_destinに入れる値を一括指定
	reprules.setSADestinStrForUnfound( "9999");
	// 変換できなかった文字列を入れる変数名につけるsuffixを一括指定（vname_denstinにつける）
	reprules.setSAVnSuffixForUnfound( "_other");

	// MAのルールを追加
	reprules.addMARepRules( ma_varvec, ma_oldvec, ma_dummyvarvec, ";");
	// 変換できなかった文字列を入れる変数名につけるsuffixを一括指定（vname_originにつける）
	reprules.setMAVnSuffixForUnfound( "_other");

	// ルールの確認
	reprules.print();

	// ルールの適用
	Dataset ds_output = reprules.apply( ds_input);

	// 結果の表示
	ds_output.print();

	// ファイルに出力
	koutputfile kof( fn_output);
	kof.open( false, false, true);
	ds_output.writeFile( kof, "\t");


	// ********************
	// Datasetから直接ルールを生成する方法

	RepRuleSet reprules_test;

	// SAのルールを追加
	reprules_test.addSARepRules(
		ds_codes_sa,
		"var", "string", "var", "code"
	);
	// 変換できなかった文字列に対応させてvname_destinに入れる値を一括指定
	reprules_test.setSADestinStrForUnfound( "9999");
	// 変換できなかった文字列を入れる変数名につけるsuffixを一括指定（vname_denstinにつける）
	reprules_test.setSAVnSuffixForUnfound( "_other");

	// MAのルールを追加
	reprules_test.addMARepRules( 
		ds_codes_ma,
		"var", "string", "newvar",
		";"
	);
	// 変換できなかった文字列を入れる変数名につけるsuffixを一括指定（vname_originにつける）
	reprules_test.setMAVnSuffixForUnfound( "_other");

	// ルールの確認
	reprules_test.print();

	// ルールの適用
	Dataset ds_output2 = reprules_test.apply( ds_input);

	// 結果の表示
	ds_output2.print();

	// ファイルに出力
	koutputfile kof2( fn_output2);
	kof2.open( false, false, true);
	ds_output2.writeFile( kof2, "\t");

	return 0;

}


/* ********** Definitions of Member Functions ********** */ 

