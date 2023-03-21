
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

	SAとMAを同時に変換できるようにしたい。
	→できた。結果は同じ。出力の変数の順番が違うだけ。
	→以前のやり方を消す（SARepRuleSet, MARepRuleSet）
	※RepRuleSetは異種リストっぽくなっている？
	
	Datasetと変数名を与える方法でもできるようにしたい。

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

// 変換ルールSARepRuleの集合。変数群（データセット）にまとめて適用する意図のもの。
class SARepRuleSet {
public:

	// 変換ルール群
	std::vector <SARepRule> rulevec;

	SARepRuleSet( void) : rulevec(){}

	SARepRuleSet( 
		const std::vector <std::string> &vn_ori0,
		const std::vector <std::string> &old0,
		const std::vector <std::string> &new0,
		const std::vector <std::string> &vn_des0
	)
	{
		initialize( vn_ori0, old0, new0, vn_des0);
	}

	void initialize( 
		const std::vector <std::string> &vn_ori0,
		const std::vector <std::string> &old0,
		const std::vector <std::string> &new0,
		const std::vector <std::string> &vn_des0
	)
	{
		int ncase = vn_ori0.size();
		if ( ncase != old0.size() || ncase != new0.size() || ncase != vn_des0.size()){
			alert( "Vectors' lengths are not the same");
			return;
		}
		for ( int i = 0; i < ncase; ++i){
			auto [ b, idx] = getIndexByOriginVar( vn_ori0[ i]);
			if ( true == b){
				rulevec[ idx].addPair( old0[ i], new0[ i]);
			} else {
				SARepRule r{ vn_ori0[ i], vn_des0[ i]};
				r.addPair( old0[ i], new0[ i]);
				rulevec.push_back(  r);
			}
		}
	}

	std::pair <bool, int> getIndexByOriginVar( const std::string &vn0)
	{
		for ( int i = 0; i < rulevec.size(); ++i){
			if ( rulevec[ i].vname_origin == vn0){
				return { true, i};
			}
		}
		return { false, -1};
	}

	void setDestinStrForUnfound( const std::string &s0)
	{
		for ( auto &r : rulevec){
			r.str_for_unfound = s0;
		}
	}

	void setVnSuffixForUnfound( const std::string &suf0)
	{
		for ( auto &r : rulevec){
			r.vname_for_unfound = r.vname_destin + suf0;
		}
	}

	void print( void)
	{
		for ( auto &r : rulevec){
			r.print();
		}
	}

	// vn0を元変数名とするルールのインデックスをintとして返す。
	// なければboolとしてfalseを返す。
	std::pair <bool, int> getRuleIndexOfOrigin( const std::string &vn0)
	{

		for ( int i = 0; i < rulevec.size(); ++i){
			if ( rulevec[ i].vname_origin == vn0){
				return { true, i};
			}
		}

		return  { false, -1};

	}

	// ds0というデータセットにルールを
	// 適用した結果得られたデータセットを返す。
	// そもそもルールのない変数については何もしない。
	Dataset apply( const Dataset &ds0)
	{

		Dataset ret;

		std::vector <std::string> datavarnames = ds0.getVarNames();

		for ( const auto &dvn0 : datavarnames){

			auto [ b, idx] = getRuleIndexOfOrigin( dvn0);

			if ( b == true){

				const auto &reprule0 = rulevec[ idx];
				std::vector <std::string> resultvec;
				std::vector <std::string> resultvec_unfound;
				bool unfound_exist = false;

				std::vector <std::string> rawstrvec;
				ds0.getStringVector( rawstrvec, dvn0);

				for ( const std::string &s0 : rawstrvec){

					auto [ b, replaced] = reprule0.getReplaced( s0);
					std::string replaced_unfound = "";

					if ( b == false){
						replaced = reprule0.str_for_unfound;
						replaced_unfound = s0;
						unfound_exist = true;
					}

					resultvec.push_back( replaced);
					resultvec_unfound.push_back( replaced_unfound);

				}

				ret.addColumn( reprule0.vname_destin, resultvec);
				// 変換できなかった文字列があれば、その文字列を入れた変数も結果に追加
				if ( unfound_exist == true){
					ret.addColumn( reprule0.vname_for_unfound, resultvec_unfound);
				}

			}

		}

		return ret;

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

// 変換ルールMARepRuleの集合。
// 変数群（データセット）にまとめて適用する意図のもの。
class MARepRuleSet {
public:

	// 変換ルール群
	std::vector <MARepRule> rulevec;

	MARepRuleSet( void) : rulevec(){}

	MARepRuleSet( 
		const std::vector <std::string> &vn_ori0,
		const std::vector <std::string> &old0,
		const std::vector <std::string> &vn_dummy0
	)
	{
		initialize( vn_ori0, old0, vn_dummy0);
	}

	MARepRuleSet( 
		const std::vector <std::string> &vn_ori0,
		const std::vector <std::string> &old0,
		const std::vector <std::string> &vn_dummy0,
		const std::string &de0
	)
	{
		initialize( vn_ori0, old0, vn_dummy0);
		setDelimiter( de0);
	}

	void initialize( 
		const std::vector <std::string> &vn_ori0,
		const std::vector <std::string> &old0,
		const std::vector <std::string> &vn_dummy0
	)
	{
		int ncase = vn_ori0.size();
		if ( ncase != old0.size() || ncase != vn_dummy0.size()){
			alert( "Vectors' lengths are not the same");
			return;
		}
		for ( int i = 0; i < ncase; ++i){
			auto [ b, idx] = getIndexByOriginVar( vn_ori0[ i]);
			if ( true == b){
				rulevec[ idx].addPair( old0[ i], vn_dummy0[ i]);
			} else {
				MARepRule r{ vn_ori0[ i]};
				r.addPair( old0[ i], vn_dummy0[ i]);
				rulevec.push_back(  r);
			}
		}
	}

	// もとの変数の文字列を分解する際のdelimiterを、一括で設定する。
	// 正確には、これは区切り文字の集合。",;"なら','と';'の両方が区切り文字になる。
	void setDelimiter( const std::string &de0)
	{
		for ( auto &rule0 : rulevec){
			rule0.delimiter = de0;
		}
	}

	void setVnSuffixForUnfound( const std::string &suf0)
	{
		for ( auto &r : rulevec){
			r.vname_for_unfound = r.vname_origin + suf0;
		}
	}

	std::pair <bool, int> getIndexByOriginVar( const std::string &vn0)
	{
		for ( int i = 0; i < rulevec.size(); ++i){
			if ( rulevec[ i].vname_origin == vn0){
				return { true, i};
			}
		}
		return { false, -1};
	}

	// vn0を元変数名とするルールのインデックスをintとして返す。
	// なければboolとしてfalseを返す。
	std::pair <bool, int> getRuleIndexOfOrigin( const std::string &vn0)
	{

		for ( int i = 0; i < rulevec.size(); ++i){
			if ( rulevec[ i].vname_origin == vn0){
				return { true, i};
			}
		}

		return  { false, -1};

	}

	// ds0というデータセットにルールを
	// 適用した結果得られたデータセットを返す。
	// そもそもルールのない変数については何もしない。
	Dataset apply( const Dataset &ds0)
	{

		Dataset ret;

		int ncase = ds0.getNCase();

		std::vector <std::string> datavarnames = ds0.getVarNames();

		for ( const auto &vn0 : datavarnames){

			auto [ b, idx] = getRuleIndexOfOrigin( vn0);

			if ( b == false){
				continue;
			}

			const auto &reprule0 = rulevec[ idx];


			// tempdsというデータセットに変換後の変数を準備する。

			Dataset tempds;

			std::vector <std::string> strs_toadd( ncase, std::string( "0"));

			// 変換後のダミー変数群を準備。
			// ここでは追加順を保持しているdummyvarnamesを使う。
			for ( const auto &dvn0 : reprule0.dummyvarnames){
				tempds.addColumn( dvn0, strs_toadd);
			}

			bool ignore_unfound = false;
			if ( reprule0.vname_for_unfound.size() < 1){
				ignore_unfound = true;
			}
			bool unfound_exist = false;
			std::vector <std::string> strvec_unfound( ncase);


			// 元の変数を1ケースずつ処理

			std::vector <std::string> rawstrvec;
			ds0.getStringVector( rawstrvec, vn0);

			for ( int caseid = 0; caseid < ncase; ++caseid){

				const std::string &s0 = rawstrvec[ caseid];

				// 複数挙げられている場合があるのでそれをバラす。
				std::vector <std::string> altvec;
				tokenize( altvec, s0, reprule0.delimiter);

				if ( altvec.size() < 1){
					continue;
				}

				for ( const std::string &t0 : altvec){

					if ( t0.size() < 1){
						continue;
					}

					// ダミー変数名を特定する。
					// 該当しない文字列の場合はそれ用の変数に入れる。
					auto [ b, dummyvn] = reprule0.getDummyVarName( t0);
					if ( b == false){
						if ( ignore_unfound == true){
							std::string msg = "Unexpected string encountered; string = \"" + t0;
							alert( "", msg);
							continue;
						} else {
							unfound_exist = true;
							strvec_unfound[ caseid] = t0;
						}

					} else {

						tempds.setValue( dummyvn, caseid, "1");

					}

				}

			}

			if ( unfound_exist == true){
				tempds.addColumn( reprule0.vname_for_unfound, strvec_unfound);
			}

			ret.mergeDataset( tempds);

		}

		return ret;

	}

	void print( void)
	{
		for ( auto &r : rulevec){
			r.print();
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


	// ********** ここから、SAの変換

	// コード変換ルールのファイルからルールのインスタンスを生成

	std::vector <string> sa_varvec;
	std::vector <string> sa_oldvec;
	std::vector <string> sa_newvec;

	// varは変数名の列、stringは選択肢の文字列表記の列、codeはコードの列、だと仮定
	ds_codes_sa.getStringVector( sa_varvec, "var");
	ds_codes_sa.getStringVector( sa_oldvec, "string");
	ds_codes_sa.getStringVector( sa_newvec, "code");

	// 文字列vector群からルールのインスタンスを生成
	SARepRuleSet sa_ruleset( sa_varvec, sa_oldvec, sa_newvec, sa_varvec);

	// 変換できなかった文字列に対応させてvname_destinに入れる値を一括指定
	sa_ruleset.setDestinStrForUnfound( "9999");
	// 変換できなかった文字列を入れる変数名につけるsuffixを一括指定（vname_denstinにつける）
	sa_ruleset.setVnSuffixForUnfound( "_other");

	// ルールの確認
	sa_ruleset.print();

	// ルールの適用
	Dataset sa_ds_output = sa_ruleset.apply( ds_input);

	// 結果の表示
	sa_ds_output.print();



	// ********** ここから、MAの変換

	// コード変換ルールのファイルからルールのインスタンスを生成

	std::vector <string> ma_varvec;
	std::vector <string> ma_oldvec;
	std::vector <string> ma_dummyvarvec;

	// varは変数名の列、stringは選択肢の文字列表記の列、newvarは対応するダミー変数の列、だと仮定
	ds_codes_ma.getStringVector( ma_varvec, "var");
	ds_codes_ma.getStringVector( ma_oldvec, "string");
	ds_codes_ma.getStringVector( ma_dummyvarvec, "newvar");

	// vector群からルールのインスタンスを生成
	MARepRuleSet ma_ruleset( ma_varvec, ma_oldvec, ma_dummyvarvec, ";");

	// 変換できなかった文字列を入れる変数名につけるsuffixを一括指定（vname_originにつける）
	ma_ruleset.setVnSuffixForUnfound( "_other");

	// ルールの確認
	ma_ruleset.print();

	// ルールの適用
	Dataset ma_ds_output = ma_ruleset.apply( ds_input);

	// 結果の表示
	ma_ds_output.print();


	// ********************
	// ファイルに出力

	Dataset ds_output_all = sa_ds_output;
	ds_output_all.mergeDataset( ma_ds_output);

	koutputfile kof( fn_output);
	kof.open( false, false, true);
	ds_output_all.writeFile( kof, "\t");




	// ********************
	// SAとMAをまとめて変換する

	std::cout << "MATOMETE" << std::endl;

	class RepRuleSet {
	public:

		// SAでもMAでも入れられるルール群のvector
		std::vector < std::variant <SARepRule, MARepRule> > rulevec;

		RepRuleSet( void) : rulevec(){}

		void addSARepRules(
			const std::vector <std::string> &vn_ori0,
			const std::vector <std::string> &old0,
			const std::vector <std::string> &new0,
			const std::vector <std::string> &vn_des0
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
						alert( "Variable \""s + "\" is defined both as MA and SA variable");
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
						alert( "Variable \""s + "\" is defined both as MA and SA variable");
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

		// ds0というデータセットにルールを
		// 適用した結果得られたデータセットを返す。
		// そもそもルールのない変数については何も出力されない。
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

	RepRuleSet reprules;

	// SAのルールを追加
	reprules.addSARepRules( sa_varvec, sa_oldvec, sa_newvec, sa_varvec);
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
	Dataset ds_output_test = reprules.apply( ds_input);

	// 結果の表示
	ds_output_test.print();

	// ファイルに出力
	std::string fn_output_test = "qa22a_02pend_out_test.txt";
	koutputfile kof_test( fn_output_test);
	kof_test.open( false, false, true);
	ds_output_test.writeFile( kof_test, "\t");



	return 0;

}


/* ********** Definitions of Member Functions ********** */ 

