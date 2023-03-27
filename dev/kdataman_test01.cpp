
/*
	
	qa22a_02.cpp
	
	cl compile options:
	-EHsc -utf-8 -std:c++17
	
	愛知大学2022年度計量政治行政分析1
	qa22a_01.Rの内容をC++で書いてみる。
	MS Formsで得られたデータを、数値コードに置き換える。

	入力ファイルは、UTF-8にした。
	
	※kstatでRecodeするものを書いた気がしたが、それはFrequencyTableをつくるときに数値範囲のためのものらしかった。

	TODO:

	☑結果の正確性を確認
	　→確認できた。

	☑Datasetと変数名を与える方法でもできるようにしたい。

	☑コメントで説明を書く。
	□ヘッダとして独立させる。
	　※Lenovo側で。それからUbuntuで。
	□exampleをつくっておく。
	

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
#include "kdataman00.cpp"


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
	
	const string fn_input = "qa22a_02_input.csv";
	const string fn_codes_sa = "qa22a_02_codes_sa.csv";
	const string fn_codes_ma = "qa22a_02_codes_ma.csv";
	const string fn_output = "qa22a_02_out.txt";

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
	// SAとMAをまとめて変換する

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
	// Datasetから直接ルールを生成しようとしてみる

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
	Dataset ds_output_test = reprules_test.apply( ds_input);

	// 結果の表示
	ds_output_test.print();

	// ファイルに出力
	const string fn_output_test = "qa22a_02_out_test.txt";
	koutputfile kof_test( fn_output_test);
	kof_test.open( false, false, true);
	ds_output_test.writeFile( kof_test, "\t");

	return 0;

}


/* ********** Definitions of Member Functions ********** */ 

