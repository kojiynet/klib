
/*
	
	kdataset
	Ver. k09.04
	
	Written by Koji Yamamoto
	Copyright (C) 2016-2023 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Dataset and Data column Classes
	
	When functions dealing with characters,
	they are applicable only to ASCII characters.
	
	TODO:
	・文字列→数値の変換の際に、もとの桁数を保持したい。
	　その際、有効桁数が、doubleの有効桁の限界を超えていたらどうするかを考える。
	・koliという名前空間をつくる。
	
	k07/kdataset00.cppまでは旧版だった。
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kdataset_cpp_include_guard
#define kdataset_cpp_include_guard

#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <boost/dynamic_bitset.hpp>
#include <k09/kutil01.cpp>
#include <k09/kinputfile02.cpp>
#include <k09/koutputfile01.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class CaseMapType; 
class Dataset;
class Datacolumn;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

//int countPrec( const std::string &);
//bool isInteger( const std::string &);

bool effectiveDigits( int &, int &, const std::string &);

namespace { // Used only inside this file

bool localStoI( int &, const std::string &); 

} // end namespace


/* ********** Class Definitions ********** */

/*
Dataset :: specifyValid()のための関数オブジェクトで用いられる型。
*/
class CaseMapType {

public:

	// 欠損値にアクセスしたことを伝達するための例外クラス
	class CaseMapTypeError : public std::runtime_error {
	public:
		CaseMapTypeError( void) : runtime_error( "CaseMapTypeError"){}
		~CaseMapTypeError(){}
	};
	
	std::map <std::string, double> vals;
	std::map <std::string, bool> vbits;

	CaseMapType( void) : vals(), vbits() {}
	~CaseMapType( void){}

	const double & operator[]( const std::string &s0) const
	{

		auto it = vals.find( s0);

		// 指定された変数がなければエラー
		if ( it == vals.end()){
			throw std::out_of_range( "CaseMapType");
		}

		// アクセス先がNaNならエラー
		if ( std::isnan( vals.at( s0))){
			throw CaseMapTypeError();
		}

		// アクセス先がユーザー欠損値ならエラー
		if ( vbits.at( s0) == false) {
			throw CaseMapTypeError();
		}

		return vals.at( s0);

	}

};


class Dataset {
	
private:
	
	std::string title;
	int nvar;
	int ncase;
	std::vector <Datacolumn> dc;
	
	
public:
	
	Dataset( void);
	Dataset( const Dataset &);
	~Dataset( void);
	
	Dataset & operator=( const Dataset &);
	
	void copyFrom( const Dataset &);
	bool readCsvFile( const std::string &); // calling readFile() using "," as separater
	bool readTsvFile( const std::string &); // calling readFile() using "\t" as separater
	bool readFile( const std::string &, const std::string &); // fixvariabletypeはしない。
	void fixVariableType( int &, int &); // 変換結果として、numericの変数の数と、numericのうちmissingの存在する変数の数を返す。
	
	void writeFile( koutputfile &, const std::string &);

	void setTitle( const std::string &);
	
	int getNVar( void) const;
	int getNCase( void) const;
	int getNNumericColumn( void) const;
	std::vector <std::string> getVarNames( void) const;
	
	bool getColumnIndex( int &, const std::string &) const;
	
	bool getStringVector( std::vector <std::string> &, const std::string &) const;

	bool getNumericVector( std::vector <double> &, const std::string &) const;
	bool getNumericVectorWithoutMissing( std::vector <double> &, const std::string &) const;
	bool getRowVectors(
	 std::vector <std::string> &, std::vector < std::vector <double> > &
	) const;
	bool getRowValidBits(
	 std::vector <std::string> &, std::vector < boost::dynamic_bitset<> > &
	) const;
	
	bool addColumn( const std::string &, const std::vector <std::string> &);
	
	bool setValue( const std::string &, int, const std::string &);
	
	bool mergeDataset( const Dataset &);
	
	bool specifyValid( const std::string &, std::function < bool( double) > );
	bool specifyValid(
		const std::string &,
		std::function <
			bool( const CaseMapType &)
		> 
	);

	void print( void) const;
	
	// ↓以下未実装
	void printStream( std::ostream &, const std::string);
	// ↑ストリームに区切り文字を指定して単純に出力。
	// titleは出さない。
	
	void printStream( std::ostream &, const std::string, int k, int el);
	// ↑ストリームに区切り文字を指定して出力。
	// 精度も指定する。
	// titleは出さない。
	
	#ifdef kdataset_cpp_test
		void printTest( void); // to test
	#endif
	
};

class Datacolumn {
	
private:
	
	bool isnumeric;    // "false" by default
	std::string vname;
	int ncase;
	int nmissing;      // valid only for numeric column
	                   // "nmissing" reflects only system-missing on reading
	
	std::vector <std::string> strs; // emtpy if "isnumeric" is true
	std::vector <double> vals;      // empty if "isnumeric" is false
	// For missing cases of numeric column,
	// "numeric_limits <double> :: quiet_NaN()" is assigned in "vals" elements
	
	// 以下のうち、convertToNumericを1度したら不変になるグループを確定したい。
	
	// maxwidth group, as represented by string
	bool maxwidthvalid; // true if maxwidth has a valid value
	int maxwidth;       // for string
	
	// digit group, for numeric vector

	// digitsvalid is false if digits variables (maxdigit, mindigit, maxprec)
	// are not assured to be correct
	// possibly because there are only missing ('.') cases
	bool digitsvalid;
	
	// for numeric vector; 
	// maxdigit == k indicates the largest valid digit is that of 10^k
	// which was detected in converting from string
	int maxdigit;
	
	// for numeric vector; 
	// mindigit == k indicates the smallest valid digit is that of 10^k
	// which was detected in converting from string
	int mindigit;
	
	// for numeric vector
	// the largest number of effective digits
	// which was detected in converting from string
	int maxprec;
	
	// for numeric vector
	// the container representing if the case is valid (i.e. not user-missing)
	// vbits is empty (its size is zero) unless user-missing is specified
	boost::dynamic_bitset<> vbits;

public:
	
	Datacolumn( void);
	Datacolumn( const Datacolumn &);
	~Datacolumn( void);
	
	Datacolumn & operator=( const Datacolumn &);
	
	void copyFrom( const Datacolumn &);
	void setData( const std::string &, const std::vector <std::string> &);
	bool convertToNumeric( int &);

	std::tuple <bool, double, std::string> getValue( int);

	bool setValue( int, const std::string &);
	void setValidBits( const boost::dynamic_bitset<> &);
	
	bool isNumeric( void) const;
	bool isInteger( void) const;
	std::string getVarName( void) const;
	int getNCase( void) const;
	double getNumericValue( int) const;
	bool getValidBit( int) const;
	bool getNumericVector( std::vector <double> &) const;
	bool getNumericVectorWithoutMissing( std::vector <double> &) const;
	bool getStringVector( std::vector <std::string> &) const;

	bool specifyValid( std::function < bool( double) > );

	void print( void) // to test
	{
		
		std::cout << "vname: " << vname << std::endl;
		
		if ( isnumeric == true ){
			for ( auto v : vals){
				std::cout << "vals: " << v << std::endl;
			}
		} else {
			for ( auto s : strs){
				std::cout << "strs: " << s << std::endl;
			}
		}
		
		std::cout << "ncase: " << ncase << std::endl;
		std::cout << "nmissing: " << nmissing << std::endl;
		std::cout << "maxwidthvalid: " << maxwidthvalid << std::endl;
		std::cout << "maxwidth: " << maxwidth << std::endl;
		std::cout << "digitsvalid: " << digitsvalid << std::endl;
		std::cout << "maxdigit: " << maxdigit << std::endl;
		std::cout << "mindigit: " << mindigit << std::endl;
		std::cout << "maxprec: " << maxprec << std::endl;
		std::cout << "isInteger(): " << isInteger() << std::endl;
		
	}
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */



/* ********** Function Definitions ********** */

/**
 * @brief             浮動小数点数表現の有効桁の範囲を識別する関数
 *                    (Identify largest/smallest effective digits
 *                     of numeric expression)
 * @param[in]  str    識別対象となる浮動小数点数表現（10進数）の文字列
 *                    (String to parse)
 * @param[out] laregd 有効桁のうち最大の桁は(10^larged)の桁である
 *                    (The largest effective digit is (10^larged))
 * @param[out] smalld 有効桁のうち最小の桁は(10^smalld)の桁である
 *                    (The largest effective digit is (10^smalld))
 * @return bool       strが有効な数値表現で識別できたか否か
 *                    (returns boolean indicating whether str is valid
 *                     numeric experssion)
 * @retval true       strが有効な数値表現だったので識別ができた
 *                    (str is valid numeric expression)
 * @retval false      strが有効な数値表現ではなかったので識別できなかった
 *                    (str is not valid numeric expression)
 * @detail see below:
 *   strに含まれている浮動小数点数表現（10進数）について、
 *   有効桁を10進数でどの位からどの位までかで表す。
 *   有効桁のうち最大の桁が（10^larged）の桁、
 *   有効桁のうち最小の桁が（10^smalld）の桁、となるように値を入れる。
 *   strがゼロの場合は、1の位のみの1桁の数値として扱う。
 *     strが"-1.234e+3"なら、larged==3, smalld==0になる。
 *     strが"-123e-1"なら、larged==1, smalld==-1になる。
 *     strが"0.000004567"なら、larged==-6, smalld==-9になる。
 *     strが"8030000"なら、larged==6, smalld==4になる。
 *     strが"0.00"なら、larged==0, smalld==0になる。
 *   有効な数値が入っていなければfalseを返す。
 *     strが"-1.234e+"なら、falseを返す。
 *     strが"-1.234e"なら、falseを返す。
 *     strが"e+3"なら、falseを返す。
 *     strが"-e+3"なら、falseを返す。
 */
bool effectiveDigits( int &larged, int &smalld, const std::string &str)
{
	
	static const std::string nonzerostr = "123456789";
	
	int size;
	int p;
	int signifsize; // 実数部の長さ
	bool expexist;   // 指数部が存在するかどうか
	int expval;      // 指数部の数値
	
	bool nonzerofound; // 1～9の数値が見つかったかどうか
	bool dotexist;     // '.'が見つかったかどうか
	bool zerofound;    // 0が見つかったかどうか
	int ld;            // 有効数値が存在する最大の桁は10のld乗の桁
	int sd;            // 有効数値が存在する最小の桁は10のsd乗の桁
	int curd;          // 現在見ている桁は10のcurd乗の桁
	                   // curdはdotexistがtrueのときのみ有効
	
	size = str.size();
	
	// まず指数部があるか探し、あれば指数部整数をexpvalに入れる
	expexist = false;
	p = str.find_first_of( "eE");
	if ( p != std::string :: npos){
		expexist = true;
		bool b = localStoI( expval, str.substr( p + 1));
		if ( b == false){
			return false;
		}
	} else {
		p = size;
	}
	
	// この時点で、pは実数部の文字数になっている
	// here 'p' stands for the length of significand digits

	signifsize = p;

	// 実数部の最大桁と最小桁を識別する
	nonzerofound = false;
	dotexist = false;
	zerofound = false;
	ld = 0;
	sd = 0;
	curd = 0;
	
	for ( int i = 0; i < signifsize; i++){

		// substantially going into four paths
		//  str[ i]: '+' or '-'
		//  str[ i]: '0'
		//  str[ i]: one in 1-9
		//  str[ i]: '.'
		
		if ( str[ i] == '+' || str[ i] == '-'){
			
			if ( i == 0){
				// do nothing
			} else {
				// '+' or '-' appeared in the middle of digits
				return false;
			}
			
		} else if ( str[ i] == '0'){
			
			if ( dotexist == true){
				curd--;
			} else {
				if ( nonzerofound == true){
					ld++;
					sd++;
				} else {
					// do nothing
				}
			}

			zerofound = true;
			
		} else if ( nonzerostr.find( str[ i]) != std::string :: npos){
			
			if ( dotexist == true){
				curd--;
				sd = curd;
				if ( nonzerofound == true){
					// do nothing
				} else {
					ld = curd;
				}
			} else {
				sd = 0;
				if ( nonzerofound == true){
					ld++;
				} else {
					ld = 0;
				}
			}

			nonzerofound = true;
			
		} else if ( str[ i] == '.'){
			
			if ( dotexist == true){
				// '.' appeared twice
				return false;
			} else {
				dotexist = true;
			}
			
		}
		
	}
	
	// 指数部があった場合には、その分だけ桁をずらす。
	if ( expexist == true){
		ld += expval;
		sd += expval;
	}

	if ( nonzerofound == false){

		if ( zerofound == false){
			// there is not any of 0-9 digit in significand
			return false;
		}

		// there are only '0's in significand
		// in this case ld and sd shall be zeros
		ld = 0;
		sd = 0;

	}

	larged = ld;
	smalld = sd;
	return true;
	
}

/*
	"+-0123456789"のみからなるstrを整数に変えてretに格納する。
	"+-"は先頭にのみ許される。"."などは許容しない。
	成功したらtrueを返す。
	
	もしstrが "" か "+" か "-" だったら、falseを返す。
*/
namespace { // only used in this file
bool localStoI( int &ret, const std::string &str)
{
	
	std::string numberstr = "0123456789";
	int size, sign, val, idx;
	
	size = str.size();

	// もしstrが "" か "+" か "-" だったら、falseを返す。
	if ( size < 1 || size == 1 && ( str[ 0] == '+' || str[ 0] == '-')){
		return false;
	}
	
	sign = 1;
	val = 0;
	for ( int i = 0; i < size; i++){
		
		if ( str[ i] == '+' && i == 0){
			sign = 1;
		} else if ( str[ i] == '-' && i == 0){
			sign = -1;
		} else {
			idx = numberstr.find( str[ i]);
			if ( idx != std::string :: npos){
				val *= 10;
				val += idx;
			} else {
				return false;
			}
		}
		
	}
	
	ret = sign * val;
	return true;
	
}
} // end namespace

/* ********** Definitions of Member Functions ********** */

/* ********** Class Dataset ********** */

Dataset ::
Dataset( void)
: title(), nvar( 0), ncase( 0), dc()
{
}

Dataset ::
Dataset( const Dataset &obj)
: title( obj.title), nvar( obj.nvar), ncase( obj.ncase), dc( obj.dc)
{
}

Dataset ::
~Dataset( void)
{
}

Dataset &
Dataset ::
operator=( const Dataset &obj)
{
	
	copyFrom( obj);
	return ( *this);
	
}

void
Dataset ::
copyFrom( const Dataset &obj)
{
	
	title = obj.title;
	nvar = obj.nvar;
	ncase = obj.ncase;
	dc = obj.dc;
	
}


/**
 * @brief        CSVファイルを読み込む関数
 *               (read CSV file)
 * @param[in] fn 対象となるCSVファイル名
 *               (CSV file to read)
 * @return bool  成功か否か
 *               (returns boolean indicating success)
 * @detail       CSVファイルを読み込んで文字列のDatasetにする。この関数では','は常に区切り文字となる。ダブルクォーテーションで囲まれた','であっても区切り文字とみなされる。
 *               (This reads CSV file into dataset object. Note that this function always regard ',' as delimiter; two double-quotations cannot serve as escape)
 */
bool
Dataset ::
readCsvFile( const std::string &fn)
{

	using namespace std;
	
	bool b;
	vector <string> lines; 
	vector < vector <string> > strvecs;

	{
		kinputfile kif;
		kif.setFileName( fn);

		b = kif.open();
		if ( b == false){
			return false;
		}
		
		b = kif.readFileAndSeparateLines( lines);
		if ( b == false){
			return false;
		}
		
		kif.close();
	}
	
	// returns false if there is no line 
	if ( lines.size() < 1){
		return false;
	}

	for ( const auto &line : lines){
		vector <string> toks;
		tokenizeForCsv( toks, line);
		strvecs.push_back( toks);
	}
	
	int ncase0 = 0;
	int nvar0 = 0;
	vector <string> vname0;
	
	// First line is seen as variable names
	nvar0 = strvecs[ 0].size();

	// returns false if there is no variable
	if ( nvar0 < 1){ 
		return false;
	}

	vname0 = strvecs[ 0];
	
	// pick up data
	
	vector < vector <string> > dcols0; // column-major type
	dcols0.resize( nvar0);
	for ( int j = 0; j < nvar0; j++){
		dcols0[ j].reserve( strvecs.size() - 1);
	}
	
	// Empty Line:
	//   there is not string; or
	//   there is only one string and the length of the string is smaller than 1
	// Returns false if valid string AFTER an empty line is found
	// 空行の判定：
	// 文字列が1つもないか、
	// 文字列が1つのみ存在しその文字列のサイズが1より小さい場合は、
	// 空行だと判断する。空行のあとに有効なデータ（区切り文字以外のもの）があるときエラーにする。
	bool emptylineexists = false;
	for ( int i = 1; i < strvecs.size(); i++){
		
		if ( strvecs[ i].size() < 1){
			
			emptylineexists = true;
			
		} else if ( strvecs[ i].size() == 1 && strvecs[ i][ 0].size() < 1){
			
			emptylineexists = true;
			
		} else {
			
			// return false if data structure is non-rectangular
			if ( strvecs[ i].size() != nvar0){
/*
std::cout << std::endl;
std::cout << "i=" << i << std::endl;
for ( auto v : strvecs[ i]){
	std::cout << v << "//";
}
std::cout << std::endl;
*/
				return false;
			}
			
			// returns false if there is valid string AFTER an empty line
			// 空行のあとにデータ（区切り文字以外のもの）がある場合、エラーに
			if ( emptylineexists == true){
				return false;
			}
			
			for ( int j = 0; j < nvar0; j++){
				dcols0[ j].push_back( strvecs[ i][ j]);
			}
			ncase0++;
			
		}
		
		
	}
	
	nvar = nvar0;
	ncase = ncase0;
	dc.resize( nvar);
	for ( int j = 0; j < nvar; j++){
		dc[ j].setData( vname0[ j], dcols0[ j]);
	}
	
	return true;
	
}

/**
 * @brief        TSVファイルを読み込む関数
 *               (read TSV file)
 * @param[in] fn 対象となるTSVファイル名
 *               (TSV file to read)
 * @return bool  成功か否か
 *               (returns boolean indicating success)
 * @detail       TSVファイルを読み込んで文字列のDatasetにする。
 *               (This reads TSV file into dataset object.)
 */
bool
Dataset ::
readTsvFile( const std::string &fn)
{
	
	bool b = readFile( fn, "\t");
	return b;
	
}

/**
 * @brief         区切り文字で区切りながら文字列をファイルから読み込む関数
 *                (read character-separated file)
 * @param[in] fn  対象となるファイル名
 *                (file to read)
 * @param[in] sep 区切り文字（の集合）
 *                ((a set of) delimiter characters)
 * @return bool  成功か否か
 *               (returns boolean indicating success)
 * @detail       sepに含まれるいずれの文字も区切り文字とみなしながら、ファイルを読み込んで文字列のDatasetにする。この関数では、ダブルクォーテーションで囲まれた区切り文字も、通常の区切り文字とみなされる。
 *               (This reads a file into dataset object, using any character in sep as a delimiter. Note that this function always regard a delimiter even if it is in between two double-quotations)
 */
bool
Dataset ::
readFile( const std::string &fn, const std::string &sep)
{
	
	kinputfile kif;
	bool b;
	std::vector < std::vector <std::string> > strvecs;

	kif.setFileName( fn);
	b = kif.open();
	if ( b == false){
		return false;
	}
	
	kif.getSeparatedStrings( strvecs, sep); 

	kif.close();
	
	// returns false if there is no line 
	if ( strvecs.size() < 1){
		return false;
	}
	
	int ncase0 = 0;
	int nvar0 = 0;
	std::vector <std::string> vname0;
	
	// First line is seen as variable names
	
	nvar0 = strvecs[ 0].size();

	// returns false if there is no variable
	if ( nvar0 < 1){ 
		return false;
	}

	vname0 = strvecs[ 0];
	
	// pick up data
	
	std::vector < std::vector <std::string> > dc0; // column-major type
	dc0.resize( nvar0);
	for ( int j = 0; j < nvar0; j++){
		dc0[ j].reserve( strvecs.size() - 1);
	}
	
	// Empty Line:
	//   there is not string; or
	//   there is only one string and the length of the string is smaller than 1
	// Returns false if valid string AFTER an empty line is found
	// 空行の判定：
	// 文字列が1つもないか、
	// 文字列が1つのみ存在しその文字列のサイズが1より小さい場合は、
	// 空行だと判断する。空行のあとに有効なデータ（区切り文字以外のもの）があるときエラーにする。
	bool emptylineexists = false;
	for ( int i = 1; i < strvecs.size(); i++){
		
		if ( strvecs[ i].size() < 1){
			
			emptylineexists = true;
			
		} else if ( strvecs[ i].size() == 1 && strvecs[ i][ 0].size() < 1){
			
			emptylineexists = true;
			
		} else {
			
			// return false if data structure is non-rectangular
			if ( strvecs[ i].size() != nvar0){
std::cout << std::endl;
std::cout << "i=" << i << std::endl;
for ( auto v : strvecs[ i]){
	std::cout << v << "//";
}
std::cout << std::endl;
				return false;
			}
			
			// returns false if there is valid string AFTER an empty line
			// 空行のあとにデータ（区切り文字以外のもの）がある場合、エラーに
			if ( emptylineexists == true){
				return false;
			}
			
			for ( int j = 0; j < nvar0; j++){
				dc0[ j].push_back( strvecs[ i][ j]);
			}
			ncase0++;
			
		}
		
		
	}
	
	nvar = nvar0;
	ncase = ncase0;
	dc.resize( nvar);
	for ( int j = 0; j < nvar; j++){
		dc[ j].setData( vname0[ j], dc0[ j]);
	}
	
	return true;
	
}

/*
区切り文字をsepとしてファイルに書き出す。
数値の表記方式や桁数は考慮せず、単純に << で出力する。
*/
void 
Dataset :: 
writeFile( koutputfile &kof, const std::string &sep)
{

	int nvar0 = dc.size();
	if ( nvar0 < 1){
		return;
	}

	std::ofstream &ofs = kof.stream();

	// write variable names
	std::vector <std::string> vnvec = getVarNames();
	auto it = vnvec.begin();
	ofs << *it;
	++it;
	for ( ; it != vnvec.end(); ++it){
		ofs << sep << *it;
	}
	ofs << std::endl;

	int ncase0 = dc[ 0].getNCase();
	for ( int i = 0; i < ncase0; ++i){

		auto [ b0, d0, s0] = dc[ 0].getValue( i);
		if ( b0 == true){ // numeric variable
			ofs << d0;
		} else {          // string variable
			ofs << s0;
		}

		for ( int j = 1; j < nvar0; ++j){
			ofs << sep;
			auto [ b0, d0, s0] = dc[ j].getValue( i);
			if ( b0 == true){ // numeric variable
				ofs << d0;
			} else {          // string variable
				ofs << s0;
			}
		}

		ofs << std::endl;

	}

}


// ここからやるけど、先にDatacolumnを見る。

/*
	stringのデータ列のうち、numericに変換できるものは変換する。
	この際、numericに変換したデータ列の数をnvarconvに入れて、
	numericに変換したデータ列のうちmissingを含むものの数をnvarmisに入れて、
	返す。
	すでにnumericのデータ列が存在する場合には、それらを無視して処理する。
*/
void
Dataset ::
fixVariableType( int &nvarconv, int &nvarmis)
{
	
	nvarconv = 0;
	nvarmis = 0;
	
	for ( int i = 0; i < dc.size(); i++){
		
		if ( dc[ i].isNumeric() == true){
			
			continue;
			
		} else {
			
			int nmis;
			bool b = dc[ i].convertToNumeric( nmis);
			if ( b == true){
				nvarconv++;
				if ( nmis > 0){
					nvarmis++;
				}
			}
			
		}
		
	}
	
}

/*
	Set title string
*/
void
Dataset ::
setTitle( const std::string &t0)
{
	
	title = t0;
	
}

/*
	Returns the number of variables (columns)
*/
int
Dataset ::
getNVar( void) const
{
	
	return dc.size();
	
}

/*
	Returns the number of cases
*/
int
Dataset ::
getNCase( void) const
{
	
	return ncase;
	
}

/*
	Returns the number of numeric columns
*/
int
Dataset ::
getNNumericColumn( void) const
{
	
	int ret = 0;
	for ( int i = 0; i < dc.size(); i++){
		if ( dc[ i].isNumeric() == true){
			ret++;
		}
	}
	
	return ret;
	
}

/*
	Returns variable names 
*/
std::vector <std::string>
Dataset :: 
getVarNames( void) const
{
	
	int nvar = dc.size();
	std::vector <std::string> ret;
	ret.reserve( nvar);
	for ( const auto &c : dc){
		std::string s = c.getVarName();
		ret.push_back( s);
	}
	return ret;
	
}



/*
	vn0を変数名とするDatacolumnのインデックスをidxに入れて、
	成功の場合trueを返す。
	失敗の場合、idxには-1が入る。
	失敗の場合とは：
	　2つ以上のDatacolumnの名前がvn0に該当するときと、
	　該当するものが1つもないとき。
*/
bool
Dataset ::
getColumnIndex( int &idx, const std::string &vn0) const
{
	
	int ret;
	bool found;
	found = false;
	
	for ( int i = 0; i < nvar; i++){
		
		if ( dc[ i].getVarName() == vn0){
			
			if ( found == true){ // 2つ以上の変数が同一名称
				idx = -1;
				return false;
			} else {
				ret = i;
				found = true;
			}
			
		}
		
	}
	
	if ( found == false){ // vn0を変数名とするものが1つもない
		idx = -1;
		return false;
	}
	
	idx = ret; 
	return true;
	
}

/*
	find the string datacolumn of the name vn0 and copy the contents into vret;
	returns true for success and false for failure;
	failure is issued if there is no variable of the name vn0,
	or there are two or more such variables, or such a variable is not string one.
*/
bool
Dataset ::
getStringVector( std::vector <std::string> &vret, const std::string &vn0) const
{
	
	bool b;
	int idx;
	
	b = getColumnIndex( idx, vn0);
	if ( b == false){ // 変数名vn0の変数がないか、2つ以上ある場合
		vret.clear();
		return false;
	}
	
	b = dc[ idx].getStringVector( vret);
	if ( b == false){ // 変数vn0が文字列ではない場合
		vret.clear();
		return false;
	}
	
	return true;
	
}
	
/*
	find the numeric datacolumn of the name vn0 and copy the contents into vret;
	returns true for success and false for failure;
	failure is issued if there is no variable of the name vn0,
	or there are two or more such variables, or such a variable is not numeric one.
*/
bool
Dataset ::
getNumericVector( std::vector <double> &vret, const std::string &vn0) const
{
	
	bool b;
	int idx;
	
	b = getColumnIndex( idx, vn0);
	if ( b == false){ // 変数名vn0の変数がないか、2つ以上ある場合
		vret.clear();
		return false;
	}
	
	b = dc[ idx].getNumericVector( vret);
	if ( b == false){ // 変数vn0が数値ではない場合
		vret.clear();
		return false;
	}
	
	return true;
	
}

/*
	find the numeric datacolumn of the name vn0 and copy the contents into vret
	 EXCLUDING missing cases;
	returns true for success and false for failure;
	failure is issued if there is no variable of the name vn0,
	or there are two or more such variables, or such a variable is not numeric one.
*/
bool
Dataset ::
getNumericVectorWithoutMissing( std::vector <double> &vret, const std::string &vn0) const
{
	
	bool b;
	int idx;
	
	b = getColumnIndex( idx, vn0);
	if ( b == false){ // 変数名vn0の変数がないか、2つ以上ある場合
		vret.clear();
		return false;
	}
	
	b = dc[ idx].getNumericVectorWithoutMissing( vret);
	if ( b == false){ // 変数vn0が数値ではない場合
		vret.clear();
		return false;
	}
	
	return true;
	
}

/*
	数値コラムだけを抜き出し、その変数名群のvectorをvnamesとして、
	各ケースの値を収めた行ベクトルをケース数分集めた数値のvectorをrowvecsとして、返す。
	数値コラムが1つもない場合にはfalseを、それ以外の場合にはtrueを返す。
*/
bool
Dataset ::
getRowVectors(
 std::vector <std::string> &vnames, std::vector < std::vector <double> > &rowvecs
) const
{
	
	std::vector <int> ids; // index values for numeric columns
	ids.clear();
	
	vnames.clear();
	rowvecs.clear();
	
	for ( int i = 0; i < dc.size(); i++){
		if ( dc[ i].isNumeric() == true){
			ids.push_back( i);
			vnames.push_back( dc[ i].getVarName());
		}
	}
	
	if ( ids.size() < 1){
		return false;
	}
	
	std::vector <double> tempvec;
	for ( int i = 0; i < ncase; i++){
		tempvec.resize( ids.size());
		for ( int j = 0; j < ids.size(); j++){
			tempvec[ j] = dc[ ids[ j]].getNumericValue( i);
		}
		rowvecs.push_back( tempvec);
	}
	
	return true;
	
}

/*
数値コラムだけを抜き出し、その変数名群のvectorをvnamesとして、
各ケースがValidかどうかのビットをdynamic_bitsetを
ケース数分集めた数値のvectorをrowvecsとして、返す。
数値コラムが1つもない場合にはfalseを、それ以外の場合にはtrueを返す。
*/
bool
Dataset ::
getRowValidBits(
 std::vector <std::string> &vnames, std::vector < boost::dynamic_bitset<> > &rowvbits
) const
{
	
	std::vector <int> ids; // index values for numeric columns
	ids.clear();
	
	vnames.clear();
	rowvbits.clear();
	
	for ( int i = 0; i < dc.size(); i++){
		if ( dc[ i].isNumeric() == true){
			ids.push_back( i);
			vnames.push_back( dc[ i].getVarName());
		}
	}
	
	if ( ids.size() < 1){
		return false;
	}
	
	boost::dynamic_bitset<> tempvbits;
	for ( int i = 0; i < ncase; i++){
		tempvbits.resize( ids.size());
		for ( int j = 0; j < ids.size(); j++){
			tempvbits[ j] = dc[ ids[ j]].getValidBit( i);
		}
		rowvbits.push_back( tempvbits);
	}
	
	return true;
	
}

bool
Dataset ::
addColumn( const std::string &vn0, const std::vector <std::string> &vec0)
{
	
	if ( ncase > 0 && ncase != vec0.size()){
		return false;
	}
	
	int idx;
	bool b = getColumnIndex( idx, vn0);
	if ( b == true){
		return false;
	}
	
	Datacolumn dc0;
	dc0.setData( vn0, vec0);
	dc.push_back( dc0);
	
	nvar = dc.size();
	ncase = vec0.size();
	
	return true;
	
}

// 変数vn0のケース番号caseid0の値をstr0にする。
// 型はstringであるという前提で。
// 失敗したらfalseを返す。
bool
Dataset ::
setValue( const std::string &vn0, int caseid0, const std::string &str0)
{

	if ( vn0.size() < 1 || caseid0 < 0 || caseid0 >= ncase){
		return false;
	}

	int idx;
	{
		bool b = getColumnIndex( idx, vn0);
		if ( b == false){
			return false;
		}
	}

	{
		bool b = dc[ idx].setValue( caseid0, str0); // 未実装
		if ( b == false){
			return false;
		}
	}

	return true;

}

// 他のデータセットds0を「右」に追加する。
// 失敗したらfalseを返す。
// 同じ変数名があるかはチェックしていない。。
bool 
Dataset ::
mergeDataset( const Dataset &ds0)
{

	if ( ncase < 1){
		ncase = ds0.ncase;
	} else if ( ncase != ds0.ncase){
		return false;
	}

	for ( const auto dc0 : ds0.dc){
		Datacolumn temp = dc0;
		dc.push_back( temp);
		++nvar;
	}

	return true;

}

/*
	「有効ケース」の条件を指定する。
	変数vnの有効／欠損の判定に、変数vnの情報のみを使う。
	funcは、vnの値によって有効かどうかを判定して、結果をboolで返す関数オブジェクト。
	trueが返れば有効。
	これに当てはまらないものはMissingになる。
*/
bool 
Dataset ::
specifyValid( const std::string &vn0, std::function < bool( double) > func)
{
		
	bool b;
	int idx;
	
	b = getColumnIndex( idx, vn0);
	if ( b == false){ // 変数名vn0の変数がないか、2つ以上ある場合
		return false;
	}
	
	b = dc[ idx].specifyValid( func);
	if ( b == false){ // 変数vn0が数値ではない場合
		return false;
	}
	
	return true;

}

// ↓CaseMapTypeErrorが実際に生じるのかをテストしていない。
// 　また、CaseMapTypeで、map< string,pair<double,bool> >　を使うか、
// 　map <string,ValueValidType>　として、構造体ValueValidTypeを定義するのがいいのかも。
/*
「有効ケース」の条件を指定する。
変数vn0の有効／欠損の判定に、そのケースのNumeric変数をすべて使う。
funcは、各ケースのNumeric変数の値によって有効かどうかを判定して、結果をboolで返す関数オブジェクト。
各ケースのNumeric変数の値は、CaseMapType型でfuncに渡す。
trueが返れば有効。
これに当てはまらないものはMissingになる。
func内でアクセスされた変数が欠損値である場合には、Missingになる。
*/
bool
Dataset ::
specifyValid(
	const std::string &vn0,
	std::function <
		bool( const CaseMapType &)
	> func
)
{

	bool b;
	int idx;
	
	b = getColumnIndex( idx, vn0);
	if ( b == false){ // 変数名vn0の変数がないか、2つ以上ある場合
		return false;
	}
	if ( dc[ idx].isNumeric() == false){ // 変数名vn0の変数がNumericではない場合
		return false;
	}

	boost::dynamic_bitset<> tempvbits( ncase);
	tempvbits.set(); // Sets every bit in this bitset to 1.

	std::vector <std::string> vnames;
	std::vector < std::vector <double> > rvecs;
	getRowVectors( vnames, rvecs);

	std::vector < boost::dynamic_bitset<> > rvbits;
	getRowValidBits( vnames, rvbits);

	CaseMapType casemap; 

	for ( int i = 0; i < ncase; i++){

		for ( int j = 0; j < vnames.size(); j++){
			casemap.vals[ vnames[ j]] = rvecs[ i][ j];
			casemap.vbits[ vnames[ j]] = rvbits[ i][ j];
		}

		bool valid;

		try {

			valid = func( casemap);

		} catch( std::out_of_range &e){

			// func()内で、存在しない変数にアクセスしようとした場合には、
			// その旨を表示して例外を上に投げる。
			alert( "Dataset::specifyValid()", "Tried to access a variable that does not exist");
			throw;

		} catch( CaseMapType :: CaseMapTypeError &e){

			// func()の中で指定された変数の値が欠損
			// →判定結果も欠損にする。
			valid = false;

		}

		if ( valid == true){
			// do nothing
		} else {
			tempvbits[ i] = false; 
		}

	}

	dc[ idx].setValidBits( tempvbits); 
	
	return true;

}

/*
	simply print the contents of the dataset, including the title,
	on the console (cout).
*/
void Dataset :: print( void) const
{
	
	using namespace std;

	std::vector < std::vector <std::string> > strcols;
	std::vector <double> numvec;
	std::stringstream ss;
	std::vector <int> width;

	if ( dc.size() < 1){
		alert( "Dataset :: print()");
		return;
	}

	if ( dc[ 0].getNCase() < 1){
		alert( "Dataset :: print()");
		return;
	}
	
	// preparation
	
	strcols.resize( dc.size());
	
	for ( int i = 0; i < dc.size(); i++){
		
		if ( dc[ i].isNumeric() == true){
			
			dc[ i].getNumericVector( numvec);
			
			strcols[ i].resize( numvec.size());
			for ( int j = 0; j < numvec.size(); j++){
				ss.str( "");
				ss << numvec[ j];
				strcols[ i][ j] = ss.str();
			}
			
		} else {
			
			dc[ i].getStringVector( strcols[ i]);
			
		}
		
		strcols[ i].insert( strcols[ i].begin(), dc[ i].getVarName());
		
	}
	
	width.resize( dc.size());
	for ( int i = 0; i < strcols.size(); i++){
		int w0 = 0;
		for ( auto v : strcols[ i]){
			if ( w0 < v.size()){
				w0 = v.size();
			}
		}
		width[ i] = w0;
	}
	
	
	// display
	
	cout << setfill( ' ');
	
	if ( title.size() > 0){
		cout << "***** " << title << " *****" << endl;
	}
	
	for ( int j = 0; j < strcols[ 0].size(); j++){
		for ( int i = 0; i < strcols.size(); i++){
			cout << setw( width[ i]);
			cout << strcols[ i][ j];
			cout << " ";
		}
		cout << endl;
	}
	
}

#ifdef kdataset_cpp_test
void Dataset :: printTest( void) // to test
{
	
	for ( int i = 0; i < dc.size(); i++){
		
		dc[ i].print();
		std::cout << std::endl;
		
	}
	
}
#endif


/* ********** Class Datacolumn ********** */

Datacolumn ::
Datacolumn( void)
: isnumeric( false), vname(), ncase( 0), nmissing( 0), strs(), vals(),
  maxwidthvalid( false), maxwidth( -1), 
  digitsvalid( false), maxdigit( -1), mindigit( -1), maxprec( -1)
{
}

Datacolumn ::
Datacolumn( const Datacolumn &dc0)
: isnumeric( dc0.isnumeric), vname( dc0.vname), ncase( dc0.ncase),
  nmissing( dc0.nmissing), strs( dc0.strs), vals( dc0.vals),
  maxwidthvalid( dc0.maxwidthvalid), maxwidth( dc0.maxwidth),
  digitsvalid( dc0.digitsvalid),
  maxdigit( dc0.maxdigit), mindigit( dc0.mindigit), maxprec( dc0.maxprec),
  vbits( dc0.vbits)
{
}

Datacolumn ::
~Datacolumn( void)
{
}

Datacolumn &
Datacolumn ::
operator=( const Datacolumn &dc0)
{
	
	copyFrom( dc0);
	return ( *this);
	
}

void
Datacolumn ::
copyFrom( const Datacolumn &dc0)
{
	
	isnumeric = dc0.isnumeric;
	vname = dc0.vname;
	ncase = dc0.ncase;
	nmissing = dc0.nmissing;
	strs = dc0.strs;
	vals = dc0.vals;
	maxwidthvalid = dc0.maxwidthvalid;
	maxwidth = dc0.maxwidth;
	digitsvalid = dc0.digitsvalid;
	maxdigit = dc0.maxdigit;
	mindigit = dc0.mindigit;
	maxprec = dc0.maxprec;
	vbits = dc0.vbits;
	
}

void
Datacolumn ::
setData( const std::string &vn0, const std::vector <std::string> &strs0)
{
	
	isnumeric = false;
	vname = vn0;
	ncase = strs0.size();
	nmissing = 0;
	strs = strs0;
	vals.clear();
	
	int maxw0 = 0;
	for ( auto v : strs){
		if ( v.size() > maxw0){
			maxw0 = v.size();
		}
	}
	maxwidthvalid = true;
	maxwidth = maxw0;
	
	digitsvalid = false;
	maxdigit = -1;
	mindigit = -1;
	maxprec = -1;

	vbits.clear();
	
}

// Datacolumn
// ここまで見た。

/*
	stringのcolumnをnumericに変換する。
	成功したらtrue、失敗したらfalseを返す。
	nmisretにはmissing caseの数を返す。
	各stringの中で、数値の前後に' 'か'\t'があればそれを除外して変換する。
	''か'.'であれば、missingと見なし、NaNを入れる。
	同時に、実数部の有効桁数も算出する。
*/
bool
Datacolumn ::
convertToNumeric( int &nmisret)
{
	
	if ( ncase < 1 || strs.size() < 1 || isnumeric == true){
		alert( "Datacolumn :: convertToNumeric()");
		return false;
	}
	
	vals.clear();
	vals.resize( strs.size());
	
	int maxprec0 = 0;
	int maxdigit0 = 0;
	int mindigit0 = 0;
	bool dvalid = false;
	int nmis0 = 0;
	for ( int i = 0; i < strs.size(); i++){
		
		// if strs[ i] is empty, then vals[ i] should be NaN (missing)
		if ( strs[ i].size() < 1){ 
			
			vals[ i] = std::numeric_limits <double> :: quiet_NaN();
			nmis0++;
			continue;
			
		}

		std::string tempstr;
		std::string delim;

		delim.assign( " \t");

		// erase ' ' and '\t' in the first and the last part of strs[ i]
		tempstr = boost::trim_copy_if( strs[ i], boost::is_any_of( delim));

		if ( tempstr == "."){
			
			vals[ i] = std::numeric_limits <double> :: quiet_NaN();
			nmis0++;
			
		} else {
			
			vals[ i] = stringToDouble( tempstr); 
			
			if ( std::isnan( vals[ i]) == true){
				vals.clear();
				return false;
			}

			int larged, smalld;
			bool b = effectiveDigits( larged, smalld, tempstr);
			if ( b == false){
				vals.clear();
				return false;
			}
			
			if ( dvalid == false){
				
				maxdigit0 = larged;
				mindigit0 = smalld;
				dvalid = true;
				
			} else {
				
				if ( larged > maxdigit0){
					maxdigit0 = larged;
				}
				
				if ( smalld < mindigit0){
					mindigit0 = smalld;
				}
				
			}
			
			int prec = larged - smalld + 1;
			if ( prec > maxprec0){
				maxprec0 = prec;
			}
			
		}
		
	}
	
	isnumeric = true;
	strs.clear();
	
	digitsvalid = dvalid;
	maxdigit = maxdigit0;
	mindigit = mindigit0;
	maxprec = maxprec0;
	nmissing = nmis0;
	
	nmisret = nmis0;
	return true;
	
}

/*
caseid0番目のケースの値を返す。
{ isnumeric, 数値, 文字列}のタプルを返す。
isnumericがtrueなら数値を、falseなら文字列を使うべし。
*/
std::tuple <bool, double, std::string>
Datacolumn ::
getValue( int caseid0)
{

	if ( caseid0 >= ncase){
		throwMsgExcept( "Datacolumn::getValue()");
	}

	double d;
	std::string s;

	if ( isnumeric == true){
		d = vals[ caseid0];
	} else {
		s = strs[ caseid0];
	}

	return { isnumeric, d, s};

}


// caseid0番目のケースの値をstr0にする。
// 型がstringである前提で。
// 失敗したらfalseを返す。
bool 
Datacolumn :: 
setValue( int caseid0, const std::string &str0)
{

	if ( isnumeric == true){
		return false;
	}

	if ( caseid0 < 0 || caseid0 >= ncase){
		return false;
	}

	strs[ caseid0] = str0;
	return true;

}

// sets vbits
void 
Datacolumn :: 
setValidBits( const boost::dynamic_bitset<> &vbits0)
{

	vbits = vbits0;

}

// Returns true if the datacolumn is numeric
bool
Datacolumn ::
isNumeric( void)
const
{
	
	return isnumeric;
	
}

// Returns true if the datacolumn is numeric and only contains integers.
// Note that this returns false if digitsvalid == false, which might be caused by 
// all values being missing
bool
Datacolumn ::
isInteger( void)
const
{
	
	if ( isnumeric == false){
		return false;
	}
	
	if ( digitsvalid == false){
		return false;
	}
	
	if ( mindigit < 0){
		return false;
	}
	
	return true;
	
}

/*
	returns vname
*/
std::string
Datacolumn ::
getVarName( void) const
{
	
	return vname;
	
}

/*
	returns number of cases
*/
int 
Datacolumn ::
getNCase( void) const
{

	int ret;
	
	if ( isnumeric == true){
		ret = vals.size();
	} else {
		ret = strs.size();
	}
	
	return ret;
	
}

/*
	インデックスがrのケースの数値を返す。
	数値コラムではない場合とインデックスがrのケースがない場合にはNaNを返す。
*/
double
Datacolumn ::
getNumericValue( int r) const
{
	
	if ( isnumeric == false || vals.size() < 1){
		return std::numeric_limits <double> :: quiet_NaN();
	}
	
	return vals[ r];
	
}

/*
インデックスがrのケースがValidかどうかのビットをboolで返す。
数値コラムではない場合とインデックスがrのケースがない場合には例外を投げる。
そうではないが、vbitsが空の場合は全部がValidだとみなす。
*/
bool
Datacolumn ::
getValidBit( int r) const
{
	
	if ( isnumeric == false || vals.size() < 1){
		throw std::runtime_error( "Datacolumn::getValidBit()");
	}
	
	if ( vbits.size() < 1){
		return true;
	}

	return vbits[ r];
	
}

/*
	copies the contents of vals into vret;
	returns true for success; false for failure;
	failure is issued if the column is not numeric
*/
bool 
Datacolumn ::
getNumericVector( std::vector <double> &vret) const
{
	
	if ( isnumeric == false){ // 数値のデータコラムではない場合
		vret.clear();
		return false;
	}
	
	vret = vals;
	return true;
	
}

/*
	copies the contents of vals into vret
	 EXCLUDING missing cases (i.e. quiet_NaN and user-missing);
	returns true for success; false for failure;
	failure is issued if the column is not numeric
*/
bool 
Datacolumn ::
getNumericVectorWithoutMissing( std::vector <double> &vret) const
{
	
	vret.clear();

	if ( isnumeric == false){ // 数値のデータコラムではない場合
		vret.clear();
		return false;
	}
	
	int nc = vals.size();

	vret.reserve( nc);

	for ( int i = 0; i < nc; i++){
		const double &v = vals[ i];
		if ( std::isnan( v)){
			// do nothing
		} else if ( vbits.size() > 0 && vbits[ i] == false){
			// do nothing
		} else {
			vret.push_back( v);
		}
	}

	return true;
	
}

/*
	copies the contents of strs into sret;
	returns true for success; false for failure;
	failure is issued if the column is numeric
*/
bool 
Datacolumn ::
getStringVector( std::vector <std::string> &sret) const
{
	
	if ( isnumeric == true){ // 数値のデータコラムの場合
		sret.clear();
		return false;
	}
	
	sret = strs;
	return true;
	
}

/*
	「有効ケース」の条件を指定する。
	これに当てはまらないケースはUser-Missing扱いになる。
	User-Missingはvbitsで表現される。
	vbitsがまだ使われていない（sizeがゼロの）場合は、sizeをケース数にする。
	funcは値が有効かどうかを判定してboolで返す関数オブジェクト。
*/
bool 
Datacolumn ::
specifyValid( std::function < bool( double) > func)
{
	
	if ( isnumeric == false){ // 数値のデータコラムではない場合
		return false;
	}

	int nc = vals.size();

	if ( vbits.size() < 1){
		vbits.resize( nc);
		vbits.set();
	}
	
	for ( int i = 0; i < nc; i++){
		const double &v = vals[ i];
		if ( std::isnan( v)){
			// do nothing
		} else {
			if ( func( v)){
				// do nothing
			} else {
				vbits[ i] = false;
			}
		}
	}

	return true;
	
}

#endif // kdataset_cpp_include_guard



// 以下は、あとで消す関数

// countPrec()は、effectiveDigits()で代用できるので、そのうち消す。
/*
	s0に含まれている浮動小数点数表現（10進数）について、
	その実数部の有効桁数を数えて返す。
	仮にs0が"0.123000"なら、3を返す。
	有効な数値が入っていなければ0を返す。
*/
/*
int countPrec( const std::string &s0)
{
	
	int ret;
	int p1, p2, p3;
	std::string numberstr = "0123456789";
	
	p1 = s0.find_first_not_of( "+-0.");
	p2 = s0.find_first_of( "eE");
	if ( p2 != std::string :: npos){
		p2--;
	}
	p3 = s0.find_last_not_of( "0", p2);
	if ( p1 == std::string :: npos || p3 == std::string :: npos || p1 > p3){
		return 0;
	}
	
	ret = 0;
	for ( int i = p1; i <= p3; i++){
		if ( numberstr.find( s0[ i]) != std::string :: npos){
			ret++;
		}
	}
	
	return ret;
	
}
*/


// ↓isIntegerは、effectiveDigits()で代用できるので、そのうち消す。
/*
	s0に含まれている浮動小数点数表現（10進数）について、
	それが整数かどうかを判定して、整数ならtrueを返す。
	仮にs0が"-1.234e+3"なら、trueを返す。
	仮にs0が"-123e-1"なら、falseを返す。
	有効な数値が入っていなければfalseを返す。
*/
/*
bool isInteger( const std::string &s0)
{
	
	std::string nonzerostr = "123456789";
	
	int size;
	int p;
	bool expexist;
	int expval;
	
	bool nonzerofound;
	int ld; // 有効数値が存在する最大の桁は10のld乗の桁
	int sd; // 有効数値が存在する最小の桁は10のsd乗の桁
	int curd; // 現在見ている桁は10のcurd乗の桁；これはdotexistがtrueのときのみ有効
	bool dotexist; // '.'が見つかったかどうか
	
	size = s0.size();
	
	// まず指数部があるか探し、あれば指数部整数をexpvalに入れる。
	expexist = false;
	p = s0.find_first_of( "eE");
	if ( p != std::string :: npos){
		expexist = true;
		bool b = localStoI( expval, s0.substr( p + 1));
		if ( b == false){
			return false;
		}
	} else {
		p = size;
	}
	
	// 実数部の最大桁と最小桁を識別する。
	nonzerofound = false;
	ld = 0;
	sd = 0;
	curd = 0;
	dotexist = false;
	
	for ( int i = 0; i < p; i++){
		
		if ( s0[ i] == '+' || s0[ i] == '-'){
			
			if ( i == 0){
				// do nothing
			} else {
				return false;
			}
			
		} else if ( s0[ i] == '0'){
			
			if ( nonzerofound == false){
				// do nothing
			} else {
				if ( dotexist == true){
					curd--;
				} else {
					ld++;
					sd++;
				}
			}
			
		} else if ( nonzerostr.find( s0[ i]) != std::string :: npos){
			
			if ( dotexist == true){
				curd--;
				sd = curd;
			} else {
				ld++;
				sd = 0;
				if ( nonzerofound == false){
					nonzerofound = true;
				}
			}
			
		} else if ( s0[ i] == '.'){
			
			if ( dotexist == true){
				return false;
			} else {
				dotexist = true;
			}
			
		}
		
	}
	
	// 指数部があった場合には、その分だけ桁をずらす。
	if ( expexist == true){
		ld += expval;
		sd += expval;
	}
	
	bool ret;
	ret = false;
	if ( sd >= 0){
		ret = true;
	}
	return ret;
	
}
*/

/*

旧版のkdataset.cpp参照。
Ver.0.2ぐらい。

旧版のkdatacolumn.cpp参照。
Ver.0.2ぐらい。

*/

