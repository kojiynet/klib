
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

	□コメントで説明を書く。
	□ヘッダとして独立させる。
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


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */

// 特定の変数に対する変換ルールを示すクラス
// SA変数用
class SARepRule {
public:

	// もとの変数名
	std::string vname_origin;

	// 変換後の変数名
	std::string vname_destin;
	
	// 変換前後の対応を示すmap
	// 1つ目が変換前文字列、2つ目が変換後文字列
	std::map <std::string, std::string> mapobj;


	// 以下、変換できなかったときに使うフィールド

	// mapにない文字列に対する変換結果となる値（デフォルトでは空）
	std::string str_for_unfound = "";

	// 変換できなかった文字列を入れる変数名（これが空の場合には変数をつくらない）
	std::string vname_for_unfound = "";


	// ***** Methods *****

	SARepRule( void)
	: vname_origin(), vname_destin(), mapobj() 
	{}

	SARepRule( const std::string vn_ori0, const std::string vn_des0)
	: vname_origin( vn_ori0), vname_destin( vn_des0), mapobj()
	{}
	
	// 対応関係を追加する。
	// もとの文字列についてダブりをチェックする。
	// あとから現れたルールは無視する。
	void addPair( const std::string &old0, const std::string &new0)
	{

		auto it = mapobj.find( old0);
		if ( it != mapobj.end()){
			alert(
				"SARepRule::addPair()",
				"For variable " + vname_origin + ", "
				"string " + old0 + " appeared more than once. "
				"Later ones are ignored."
			);
			return;
		}

		mapobj.insert( std::make_pair( old0, new0));

	}

	// 変換後文字列を得る。
	std::pair <bool, std::string> getReplaced( const std::string &old0) const
	{
		auto it = mapobj.find( old0);
		if ( it != mapobj.end()){
			return { true, it->second};
		}
		return { false, std::string()};
	}

	// 文字列のベクタを変換する。
	// 結果をDatasetに入れて返す。
	// 必要なら変換できなかった文字列の変数もつくる。
	Dataset apply( const std::vector <std::string> strvec0) const
	{

		Dataset ret;

		std::vector <std::string> resultvec;
		std::vector <std::string> resultvec_unfound;
		bool unfound_exist = false;

		for ( const std::string &s0 : strvec0){

			auto [ b, replaced] = getReplaced( s0);
			std::string replaced_unfound = "";

			if ( b == false){
				replaced = str_for_unfound;
				replaced_unfound = s0;
				unfound_exist = true;
			}

			resultvec.push_back( replaced);
			resultvec_unfound.push_back( replaced_unfound);

		}

		ret.addColumn( vname_destin, resultvec);
		// 変換できなかった文字列があれば、その文字列を入れた変数も結果に追加
		if ( unfound_exist == true){
			ret.addColumn( vname_for_unfound, resultvec_unfound);
		}

		return ret;

	}

	// 出力する。
	void print( void) const
	{
		std::cout << vname_origin << " TO " << vname_destin << std::endl;
		for ( const auto m0 : mapobj){
			std::cout << " " << m0.first << " TO " << m0.second << std::endl;
		}
		std::cout << " STR_FOR_UNFOUND: " << str_for_unfound << std::endl;
		std::cout << " VNAME_FOR_UNFOUND: " << vname_for_unfound << std::endl;
	}

};


// 特定の変数に対する変換ルールを示すクラス
// MA変数用
// 0/1のダミー変数にしていく。
class MARepRule {
public:

	// もとの変数名
	std::string vname_origin;

	// 変換の内容を示すmap
	// 1つ目が変換前文字列、2つ目が変換後の変数名
	std::map <std::string, std::string> mapobj;

	// 変換後の変数名のベクタ
	// 追加順を保持している。
	std::vector <std::string> dummyvarnames;

	// もとの変数の文字列を分解する際のdelimiter
	// 正確には、これは区切り文字の集合。",;"なら','と';'の両方が区切り文字になる。
	// デフォルトは ","
	std::string delimiter = ","; 
	

	// 以下、変換できなかったときに使うフィールド

	// 変換できなかった文字列を入れる変数名（これが空の場合には変数をつくらない）
	std::string vname_for_unfound = "";

	// ***** Methods *****

	MARepRule( void) : vname_origin(), mapobj(), dummyvarnames()
	{}

	MARepRule( const std::string &vn_ori0) : vname_origin( vn_ori0), mapobj(), dummyvarnames()
	{}
	
	MARepRule( const std::string &vn_ori0, const std::string &de0)
	: vname_origin( vn_ori0), mapobj(), dummyvarnames(), delimiter( de0)
	{}
	
	// もとの文字列についてダブりをチェックする。
	// あとから現れたルールは無視する。
	void addPair( const std::string &old0, const std::string &vn_dummy0)
	{

		auto it = mapobj.find( old0);
		if ( it != mapobj.end()){
			alert(
				"MARepRule::addPair()",
				"For variable " + vname_origin + ", "
				"string " + old0 + " appeared more than once. "
				"Later ones are ignored."
			);
			return;
		}

		mapobj.insert( std::make_pair( old0, vn_dummy0));
		dummyvarnames.push_back( vn_dummy0);

	}

	std::pair <bool, std::string> getDummyVarName( const std::string &old0) const
	{
		auto it = mapobj.find( old0);
		if ( it != mapobj.end()){
			return { true, it->second};
		}
		return { false, std::string()};
	}

	// 文字列のベクタを変換する。
	// 結果をDatasetに入れて返す。
	// 必要なら変換できなかった文字列の変数もつくる。
	Dataset apply( const std::vector <std::string> strvec0) const
	{

		Dataset ret;

		int ncase = strvec0.size();

		std::vector <std::string> strs_toadd( ncase, "0");

		// 変換後のダミー変数群を準備。
		// ここでは追加順を保持しているdummyvarnamesを使う。
		for ( const auto &dvn0 : dummyvarnames){
			ret.addColumn( dvn0, strs_toadd);
		}

		bool ignore_unfound = false;
		if ( vname_for_unfound.size() < 1){
			ignore_unfound = true;
		}
		bool unfound_exist = false;
		std::vector <std::string> strvec_unfound( ncase);

		// 元の変数を1ケースずつ処理

		for ( int caseid = 0; caseid < ncase; ++caseid){

			const std::string &s0 = strvec0[ caseid];

			// 複数挙げられている場合があるのでそれをバラす。
			std::vector <std::string> altvec;
			tokenize( altvec, s0, delimiter);

			if ( altvec.size() < 1){
				continue;
			}

			for ( const std::string &t0 : altvec){

				if ( t0.size() < 1){
					continue;
				}

				// ダミー変数名を特定する。
				// 該当しない文字列の場合はそれ用の変数に入れる。
				auto [ b, dummyvn] = getDummyVarName( t0);
				if ( b == false){

					if ( ignore_unfound == true){
						std::string msg = "Unexpected string encountered; string = \"" + vname_origin;
						alert( "", msg);
						continue;
					} else {
						unfound_exist = true;
						strvec_unfound[ caseid] = t0;
					}

				} else {

					ret.setValue( dummyvn, caseid, "1");

				}

			}

		}

		if ( unfound_exist == true){
			ret.addColumn( vname_for_unfound, strvec_unfound);
		}

		return ret;

	}

	void print( void) const
	{
		std::cout << vname_origin << std::endl;
		for ( const auto m0 : mapobj){
			std::cout << " " << m0.first << " INTO " << m0.second << std::endl;
		}
		std::cout << " VNAME_FOR_UNFOUND: " << vname_for_unfound << std::endl;
	}

};


// SAでもMAでも入れられるルール群
class RepRuleSet {
public:

	// SAでもMAでも入れられるルール群のvector
	std::vector < std::variant <SARepRule, MARepRule> > rulevec;

	RepRuleSet( void) : rulevec(){}

	void addSARepRules(
		const std::vector <std::string> &vn_ori0,
		const std::vector <std::string> &old0,
		const std::vector <std::string> &vn_des0,
		const std::vector <std::string> &new0
	)
	{
		int ncase = vn_ori0.size();
		if ( ncase != old0.size() || ncase != new0.size() || ncase != vn_des0.size()){
			alert( "Vectors' lengths are not the same; nothing was done");
			return;
		}
		for ( int i = 0; i < ncase; ++i){
			auto [ b, idx] = getIndexByOriginVar( vn_ori0[ i]);
			if ( true == b){
				// rulevec[ idx]がSAのルールかMAのルールかによって分岐
				if ( std::holds_alternative<SARepRule>( rulevec[ idx])){
					std::get<SARepRule>( rulevec[ idx]).addPair( old0[ i], new0[ i]);
				} else {
					alert( std::string( "Variable \"") + "\" is defined both as MA and SA variable");
					continue;
				}
			} else {
				SARepRule rule0{ vn_ori0[ i], vn_des0[ i]};
				rule0.addPair( old0[ i], new0[ i]);
				rulevec.push_back( rule0);
			}
		}
	}

	std::pair <bool, int> getIndexByOriginVar( const std::string &vn0)
	{
		for ( int i = 0; i < rulevec.size(); ++i){
			std::string vname_origin
				= std::visit(
					[]( const auto &r0){ return r0.vname_origin;},
					rulevec[ i]
				);
			if ( vname_origin == vn0){
				return { true, i};
			}
		}
		return { false, -1};
	}

	void setSADestinStrForUnfound( const std::string &s0)
	{
		for ( auto &r0 : rulevec){
			if ( std::holds_alternative<SARepRule>( r0)){
				std::get<SARepRule>( r0).str_for_unfound = s0;
			}
		}
	}

	void setSAVnSuffixForUnfound( const std::string &suf0)
	{
		for ( auto &r0 : rulevec){
			if ( std::holds_alternative<SARepRule>( r0)){
				auto &sarule0 = std::get<SARepRule>( r0);
				sarule0.vname_for_unfound = sarule0.vname_destin + suf0;
			}
		}
	}

	void addMARepRules( 
		const std::vector <std::string> &vn_ori0,
		const std::vector <std::string> &old0,
		const std::vector <std::string> &vn_dummy0,
		const std::string &de0
	)
	{

		int ncase = vn_ori0.size();
		if ( ncase != old0.size() || ncase != vn_dummy0.size()){
			alert( "Vectors' lengths are not the same; nothing was done");
			return;
		}
		for ( int i = 0; i < ncase; ++i){
			auto [ b, idx] = getIndexByOriginVar( vn_ori0[ i]);
			if ( true == b){
				// rulevec[ idx]がMAのルールかSAのルールかによって分岐
				if ( std::holds_alternative<MARepRule>( rulevec[ idx])){
					std::get<MARepRule>( rulevec[ idx]).addPair( old0[ i], vn_dummy0[ i]);
				} else {
					alert( std::string( "Variable \"") + "\" is defined both as MA and SA variable");
					continue;
				}
			} else {
				MARepRule rule0{ vn_ori0[ i]};
				rule0.addPair( old0[ i], vn_dummy0[ i]);
				rule0.delimiter = de0;
				rulevec.push_back( rule0);
			}
		}

	}

	void setMAVnSuffixForUnfound( const std::string &suf0)
	{
		for ( auto &r0 : rulevec){
			if ( std::holds_alternative<MARepRule>( r0)){
				auto &marule0 = std::get<MARepRule>( r0);
				marule0.vname_for_unfound = marule0.vname_origin + suf0;
			}
		}
	}

	// Datasetオブジェクトを与えて、その変数からSAルールを追加する。
	void addSARepRules(
		const Dataset &ds0,
		const std::string &orivn0, 
		const std::string &oldstrvn0, 
		const std::string &desvn0, 
		const std::string &newstrvn0
	)
	{
		
		using namespace std; 

		vector <string> orivarvec;
		vector <string> oldvec;
		vector <string> desvarvec;
		vector <string> newvec;

		ds0.getStringVector( orivarvec, orivn0);
		ds0.getStringVector( oldvec, oldstrvn0);
		ds0.getStringVector( desvarvec, desvn0);
		ds0.getStringVector( newvec, newstrvn0);

		addSARepRules( orivarvec, oldvec, desvarvec, newvec);

	}

	// Datasetオブジェクトを与えて、その変数からMAルールを追加する。
	void addMARepRules( 
		const Dataset ds0,
		const std::string &orivn0,
		const std::string &oldstrvn0,
		const std::string &desvn0,
		const std::string &de0
	)
	{

		using namespace std; 

		vector <string> orivarvec;
		vector <string> oldvec;
		vector <string> desvarvec;

		ds0.getStringVector( orivarvec, orivn0);
		ds0.getStringVector( oldvec, oldstrvn0);
		ds0.getStringVector( desvarvec, desvn0);

		addMARepRules( orivarvec, oldvec, desvarvec, de0);

	}


	// ds0というデータセットにルールを適用し、
	// 結果として得られたデータセットを返す。
	// そもそもルールのない変数については何も出力しない。
	Dataset apply( const Dataset &ds0)
	{

		Dataset ret;

		std::vector <std::string> datavarnames = ds0.getVarNames();

		for ( const auto &dvn0 : datavarnames){

			auto [ b, idx] = getIndexByOriginVar( dvn0);

			if ( b == true){

				std::vector <std::string> rawstrvec;
				ds0.getStringVector( rawstrvec, dvn0);

				Dataset tempresult = 
					std::visit(
						[&]( const auto &v){ return v.apply( rawstrvec); },
						rulevec[ idx]
					);

				ret.mergeDataset( tempresult);

			} else {

				// do nothing

			}

		}

		return ret;

	}

	void print( void)
	{
		for ( const auto &r0 : rulevec){
			std::visit(
				[]( const auto &v){ v.print();},
				r0
			);
		}
	}

};


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

