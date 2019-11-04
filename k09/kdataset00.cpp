
/*
	
	kdataset
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2016-2019 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Dataset and Data column Classes
	
	When functions dealing with characters,
	they are applicable only to ASCII characters.
	
	文字列→数値の変換の際に、もとの桁数を保持したい。
	その際、有効桁数が、doubleの有効桁の限界を超えていたらどうするかを考える。
	
	
	k07/kdataset00.cppまでは旧版だった。
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kdataset_cpp_include_guard
#define kdataset_cpp_include_guard

#include <string>
#include <vector>
#include <cmath>
#include <k09/kutil00.cpp>
#include <k09/kinputfile00.cpp>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */

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
	bool readFile( const std::string &, const std::string &); // fixvariabletypeはしない。
	void fixVariableType( int &, int &); // 変換結果として、numericの変数の数と、numericのうちmissingの存在する変数の数を返す。
	
	void setTitle( const std::string &);
	
	int getNVar( void) const;
	int getNCase( void) const;
	int getNNumericColumn( void) const;
	
	bool getColumnIndex( int &, const std::string &) const;
	
	bool getNumericVector( std::vector <double> &, const std::string &) const;
	bool getRowVectors(
	 std::vector <std::string> &, std::vector < std::vector <double> > &
	) const;
	
	void print( void) const;
	
	// ↓以下未実装
	void printStream( ostream &, const std::string);
	// ↑ストリームに区切り文字を指定して単純に出力。
	// titleは出さない。
	
	void printStream( ostream &, const std::string, int k, int el);
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
	
	std::vector <std::string> strs; // emtpy if "isnumeric" is true
	std::vector <double> vals;      // empty if "isnumeric" is false
	// For missing cases of numeric column,
	// "numeric_limits <double> :: quiet_NaN()" is assigned in "vals" elements
	
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
	
public:
	
	Datacolumn( void);
	Datacolumn( const Datacolumn &);
	~Datacolumn( void);
	
	Datacolumn & operator=( const Datacolumn &);
	
	void copyFrom( const Datacolumn &);
	void setData( const std::string &, const std::vector <std::string> &);
	bool convertToNumeric( int &);
	
	bool isNumeric( void) const;
	bool isInteger( void) const;
	std::string getVarName( void) const;
	int getNCase( void) const;
	double getNumericValue( int) const;
	bool getNumericVector( std::vector <double> &) const;
	bool getStringVector( std::vector <std::string> &) const;
	
	void print( void) // to test
	{
		
		cout << "vname: " << vname << endl;
		
		if ( isnumeric == true ){
			for ( auto v : vals){
				cout << "vals: " << v << endl;
			}
		} else {
			for ( auto s : strs){
				cout << "strs: " << s << endl;
			}
		}
		
		cout << "ncase: " << ncase << endl;
		cout << "nmissing: " << nmissing << endl;
		cout << "maxwidthvalid: " << maxwidthvalid << endl;
		cout << "maxwidth: " << maxwidth << endl;
		cout << "digitsvalid: " << digitsvalid << endl;
		cout << "maxdigit: " << maxdigit << endl;
		cout << "mindigit: " << mindigit << endl;
		cout << "maxprec: " << maxprec << endl;
		cout << "isInteger(): " << isInteger() << endl;
		
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

// ここからやるけど、先にDatacolumnを見る。

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

// ファイル名fnのCSVファイルを読み込む。
// 成功の場合true、失敗の場合falseを返す。
bool
Dataset ::
readCsvFile( const std::string &fn)
{
	
	bool b = readFile( fn, ",");
	return b;
	
}

/*
	ファイル名fnのファイルから、
	文字列sepに含まれるいずれかの文字で区切りながらデータを読み込む。
	sepには複数文字が含まれていてよい。
	成功の場合true、失敗の場合falseを返す。
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
	
	if ( strvecs.size() < 1){ // ファイル内容が1行もない場合、失敗
		return false;
	}
	
	int ncase0 = 0;
	int nvar0 = 0;
	std::vector <std::string> vname0;
	
	// First line is seen as variable names
	
	nvar0 = strvecs[ 0].size();
	if ( nvar0 < 1){ // 変数名が1つもない場合、失敗
		return false;
	}
	vname0.resize( nvar0);
	for ( int j = 0; j < nvar0; j++){
		vname0[ j] = strvecs[ 0][ j];
	}
	
	// pick up data
	
	std::vector < std::vector <std::string> > dc0; // 「列メジャー」なvector
	dc0.resize( nvar0);
	for ( int j = 0; j < nvar0; j++){
		dc0[ j].reserve( strvecs.size() - 1);
	}
	
	bool emptylineexists = false;
	for ( int i = 1; i < strvecs.size(); i++){
		
		// 空行の判定：
		// 文字列が1つもないか、
		// 文字列が1つのみ存在しその文字列のサイズが1より小さい場合は、
		// 空行だと判断する。空行のあとに有効なデータ（区切り文字以外のもの）があるときエラーにする。
		if ( strvecs[ i].size() < 1){
			
			emptylineexists = true;
			
		} else if ( strvecs[ i].size() == 1 && strvecs[ i][ 0].size() < 1){
			
			emptylineexists = true;
			
		} else {
			
			// return false if data structure is non-rectangular
			if ( strvecs[ i].size() != nvar0){
				return false;
			}
			
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
	simply print the contents of the dataset, including the title,
	on the console (cout).
*/
void Dataset :: print( void) const
{
	
	std::vector < std::vector <std::string> > strcols;
	std::vector <double> numvec;
	stringstream ss;
	std::vector <int> width;
	
	if ( dc.size() < 1 || dc[ 0].getNCase() < 1){
		alert( "Dataset :: print()");
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
		cout << endl;
		
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
  maxdigit( dc0.maxdigit), mindigit( dc0.mindigit), maxprec( dc0.maxprec)
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
		
		if ( strs[ i].size() < 1){
			
			vals[ i] = numeric_limits <double> :: quiet_NaN();
			nmis0++;
			continue;
			
		}
		
		int p1, p2;
		p1 = strs[ i].find_first_not_of( " \t");
		p2 = strs[ i].find_last_not_of( " \t");
		if ( p1 == std::string :: npos || p2 == std::string :: npos){
			
			vals[ i] = numeric_limits <double> :: quiet_NaN();
			nmis0++;
			continue;
			
		}
		std::string tempstr = strs[ i].substr( p1, p2 - p1 + 1);
		
		if ( tempstr == "."){
			
			vals[ i] = numeric_limits <double> :: quiet_NaN();
			nmis0++;
			
		} else {
			
			vals[ i] = stringToDouble( tempstr);
			
			if ( isnan( vals[ i]) == true){
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
		return numeric_limits <double> :: quiet_NaN();
	}
	
	return vals[ r];
	
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

