
/*
	
	kdataman
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2023 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Data Manipulation/Management 
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kdataman_cpp_include_guard
#define kdataman_cpp_include_guard

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <k09/kdataset03.cpp>


/* ********** Namespace Declarations/Directives ********** */


/* ********** Class Declarations ********** */

class SARepRule;
class MARepRule;
class RepRuleSet;


/* ********** Function Declarations ********** */


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


	// ********** Methods **********

	// デフォルトコンストラクタ
	SARepRule( void)
	: vname_origin(), vname_destin(), mapobj() 
	{}

	// もとの変数名と変換後の変数名を引数にとるコンストラクタ
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

	// ルールの内容をstd::coutに出力する。
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

	// デフォルトコンストラクタ
	MARepRule( void) : vname_origin(), mapobj(), dummyvarnames()
	{}

	// もとの変数名を引数にもつコンストラクタ
	MARepRule( const std::string &vn_ori0) : vname_origin( vn_ori0), mapobj(), dummyvarnames()
	{}
	
	// もとの変数名と区切り文字集合を引数にもつコンストラクタ
	MARepRule( const std::string &vn_ori0, const std::string &de0)
	: vname_origin( vn_ori0), mapobj(), dummyvarnames(), delimiter( de0)
	{}
	
	// もとの文字列と変換後の変数名との対応を追加する。
	// もとの文字列についてダブりをチェックする。
	// ダブっていた場合、あとから現れたルールは無視する。
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

	// もとの文字列から、対応するダミー変数名を得る。
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

	// ルールの内容をstd::coutに出力する。
	void print( void) const
	{
		std::cout << vname_origin << std::endl;
		for ( const auto m0 : mapobj){
			std::cout << " " << m0.first << " INTO " << m0.second << std::endl;
		}
		std::cout << " VNAME_FOR_UNFOUND: " << vname_for_unfound << std::endl;
	}

};

// ルール群を入れるクラス
// SAでもMAでも入れられる。
class RepRuleSet {
public:

	// SAでもMAでも入れられるルール群のvector
	// 1つの要素が1つの変換前変数にかかるルールを示す。
	std::vector < std::variant <SARepRule, MARepRule> > rulevec;

	// デフォルトコンストラクタ
	RepRuleSet( void) : rulevec(){}

	// SAの変換ルール群を追加する。
	// 以下の内容が入ったvectorを渡す。
	// 　vn_ori0 変換前の変数名
	// 　old0    変換される文字列
	// 　vn_des0 変換後の変数名
	// 　new0    変換後の文字列
	// つまり
	// 「vn_ori0にold0が入っていたら、vn_des0にnew0を入れる」
	// という対応を示す。
	// 通常、1つのvn_ori0に1つのvn_des0が対応し、
	// その中で複数のold0とnew0の対応が存在する。
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

	// rulevecにあるルールから、
	// 変換前の変数名としてvn0とマッチするものを探し、
	// そのインデックスを返す。
	std::pair <bool, int> getIndexByOriginVar( const std::string &vn0)
	{
		for ( int i = 0; i < rulevec.size(); ++i){
			std::string vname_origin = 
				std::visit(
					[]( const auto &r0){ return r0.vname_origin;},
					rulevec[ i]
				);
			if ( vname_origin == vn0){
				return { true, i};
			}
		}
		return { false, -1};
	}

	// SAのルールについて、
	// ルールにない文字列に対する変換結果となる値を一括で指定する。
	void setSADestinStrForUnfound( const std::string &s0)
	{
		for ( auto &r0 : rulevec){
			if ( std::holds_alternative<SARepRule>( r0)){
				std::get<SARepRule>( r0).str_for_unfound = s0;
			}
		}
	}

	// SAのルールについて、
	// ルールにない文字列を入れる変数名につけるサフィックスを一括で指定する。
	// ルールにない文字列を入れる変数名は、
	// 　変換後変数名＋サフィックス
	// という名称になる。
	void setSAVnSuffixForUnfound( const std::string &suf0)
	{
		for ( auto &r0 : rulevec){
			if ( std::holds_alternative<SARepRule>( r0)){
				auto &sarule0 = std::get<SARepRule>( r0);
				sarule0.vname_for_unfound = sarule0.vname_destin + suf0;
			}
		}
	}

	// MAの変換ルール群を追加する。
	// 以下の内容が入ったvectorを渡す。
	// 　vn_ori0   変換前の変数名
	// 　old0      変換される文字列
	// 　vn_dummy0 変換後のダミー変数名
	// 　de0       文字列をトークナイズする際の区切り文字集合
	// つまり
	// 「vn_ori0の内容をde0でトークナイズする。得られた文字列に、old0があれば、vn_dummy0に1を入れる」
	// という対応を示す。
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

	// MAのルールについて、
	// ルールにない文字列を入れる変数名につけるサフィックスを一括で指定する。
	// ルールにない文字列を入れる変数名は、
	// 　変換前変数名＋サフィックス
	// という名称になる。
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
	// 以下のようにDatasetオブジェクト内の変数名を指定する。
	// 　vn_vn_ori0 変換前の変数名　の入っている変数名
	// 　vn_old0    変換される文字列　の入っている変数名
	// 　vn_vn_des0 変換後の変数名　の入っている変数名
	// 　vn_new0    変換後の文字列	の入っている変数名
	void addSARepRules(
		const Dataset &ds0,
		const std::string &vn_vn_ori0, 
		const std::string &vn_old0, 
		const std::string &vn_vn_des0, 
		const std::string &vn_new0
	)
	{
		
		using namespace std; 

		vector <string> vn_ori_vec;
		vector <string> old_vec;
		vector <string> vn_des_vec;
		vector <string> new_vec;

		ds0.getStringVector( vn_ori_vec, vn_vn_ori0);
		ds0.getStringVector( old_vec, vn_old0);
		ds0.getStringVector( vn_des_vec, vn_vn_des0);
		ds0.getStringVector( new_vec, vn_new0);

		addSARepRules( vn_ori_vec, old_vec, vn_des_vec, new_vec);

	}

	// Datasetオブジェクトを与えて、その変数からMAルールを追加する。
	// 以下のようにDatasetオブジェクト内の変数名を指定する。
	// 　vn_vn_ori0   変換前の変数名　の入っている変数名
	// 　vn_old0      変換される文字列　の入っている変数名
	// 　vn_vn_dummy0 変換後のダミー変数名　の入っている変数名
	// de0は区切り文字集合
	void addMARepRules( 
		const Dataset ds0,
		const std::string &vn_vn_ori0,
		const std::string &vn_old0,
		const std::string &vn_vn_dummy0,
		const std::string &de0
	)
	{

		using namespace std; 

		vector <string> vn_ori_vec;
		vector <string> old_vec;
		vector <string> vn_dummy_vec;

		ds0.getStringVector( vn_ori_vec, vn_vn_ori0);
		ds0.getStringVector( old_vec, vn_old0);
		ds0.getStringVector( vn_dummy_vec, vn_vn_dummy0);

		addMARepRules( vn_ori_vec, old_vec, vn_dummy_vec, de0);

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

	// ルールの内容をstd::coutに出力する。	
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


/* ********** Definitions of Member Functions ********** */


#endif /* kdataman_cpp_include_guard */
