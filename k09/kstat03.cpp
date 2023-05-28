
/*
	kstat
	Ver. k09.03
	
	Written by Koji Yamamoto
	Copyright (C) 2015-2023 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Module for Statistical Computations
	
	k10になるときに以下の関数を消す。
	double unbiasedVar()　→kstatboostのものを推奨。
	double boostMean( const std::vector <double> &dv0)

	■検討課題？
	・今のRecodeTableには、左端・右端がない（無限大）という指定ができない。
	・FreqTypeにはすごく小さい機能だけを持たせることにして、
	　別にFreqTableTypeか何かをつくって、そこに、RecodeTableを持たせたり、
	　それをもとにしたFreqを作らせたりしてもよいかも。

*/


/* ********** Preprocessor Directives ********** */

#ifndef kstat_cpp_include_guard
#define kstat_cpp_include_guard

#include <memory>
#include <map>
#include <vector>
#include <limits>
#include <functional>
#include <cmath>

#include <k09/kutil02.cpp>
#include <k09/kalgo03.cpp>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Type Declarations: enum, class, etc. ********** */

// TCount is int by default
template <typename T, typename TCount = int>
class FreqType;

// TCode is int by default
template <typename T, typename TCode = int>
class RecodeTable;

/*
// This class is declared in RecodeType
template <typename T, typename TCode>
struct
RecodeTable ::
CodeType;
*/

template <typename T>
class JudgeEqual;

template <typename T>
class JudgeEqualTol;


/* ********** Function Declarations ********** */

template <typename T>
int countUniqueValues( const std::vector <T> &);

template <typename TOrigin, typename TCode, typename TCount>
void
createFreqFromRecodeTable( 
	FreqType <TCode, TCount> &,
	const std::vector <TOrigin> &,
	const RecodeTable <TOrigin, TCode> &
);

template <typename T>
std::vector <T> omitNan( const std::vector <T> &);

double sum( const std::vector <double> &);
double mean( const std::vector <double> &);
double median( const std::vector <double> &);
double unbiasedVar( const std::vector <double> &); // 非推奨。deprecated. 
double boostMean( const std::vector <double> &); // 非推奨。deprecated. 
double sum( const double *, int);
double mean( const double *, int);
double unbiasedVar( const double *, int);

// このファイル内でよいのか？
template <typename T>
std::string toString( const T &);


/* ********** Type Definitions: enum, class, etc. ********** */

// frequency type
// T is type for key, and TCount is type for count
// This type is essentially for discrete variables,
// with Tolerance for equality judgment being very small.
// When applied to continuous variables, 
// special methods should be used.
// 基本的に離散変数用。
// doubleなどに使う場合、等値判断のための
// toleranceは0か非常に小さい値である、
// という前提がある。そうでないとカテゴリ間の
// 重なりが生じてしまう。
// 連続変数に用いる場合には、特定の機能を使うべし。
// TCount is int by default
template <typename T, typename TCount /* = int */>
class FreqType {
	
private:
	
	map <T, TCount> freqmap;
	std::function < bool( const T &, const T &) > areEqual;
	TCount sumcount;

	// Note that we specify double as TOrigin for RecodeTable
	// RecodeTableのTOriginとして、doubleを指定してしまっている。
	// ここに柔軟性を持たせたいならば、rtablepがないクラスをつくり、
	// それを基底クラスにして、派生クラスでテンプレート引数を増やしてrtablepを持たせるのが、素直だろう。
	std::unique_ptr < RecodeTable<double, T> > rtablep;
	
public:
	
	FreqType( void);
	~FreqType( void);
	
	void clear( void);
	void setEqualExactly( void);
	void setEqualWithTol( const T &);
	bool addPossibleKey( const T &);
	bool addPossibleKeys( const std::vector <T> &);
	
	void increment( const T &);
	void addCount( const T &, const TCount &);
	void addFreqType( const FreqType <T, TCount> &);
	
	void clearCount( void);
	
	TCount getSumCount( void) const;
	void getVectors( std::vector <T> &, std::vector <TCount> &) const;
	double meanFromFreq( void) const;
	double medianFromFreq( void) const;
	void modeFromFreq( std::vector <T> &) const;

	void setFreqFromRecodeTable( const std::vector <double> &, const RecodeTable <double, T> &);

	void printPadding( std::ostream &) const;

	// Note that obtained vectors should be
	// parallel to those obtained by getVectors() above
	void getRangeVectors( std::vector <double> &, std::vector <double> &) const;

};


// TCode is int by default
template <typename TOrigin, typename TCode /* = int */>
class RecodeTable {

private:

	struct CodeType; // inner class; defined below

	// 1要素はRecodeTableの1行分
	std::vector <CodeType> codes; 

	// "else"の場合の処理方法　（スコープを持つ列挙型）
	enum class ElseType { Copy, AssignValue};
	
	ElseType toDoForElse; // "else"の場合の処理方法
	TCode codeForElse; // "else"の場合に値を埋める場合の値（NaN以外）

public:

	RecodeTable( void){}
	~RecodeTable( void){}

	void setAutoTableFromContVar( const std::vector <TOrigin> &);
	std::vector <TCode> getPossibleCodeVec( void) const;
	TCode getCodeForValue( TOrigin) const;

	void getLeftRightForCode( TOrigin &, TOrigin &, TCode) const; 

	std::string getRangeLabelForCode( TCode) const;
	void print( ostream &, string = ","s) const;

};


template <typename TOrigin, typename TCode>
struct
RecodeTable <TOrigin, TCode> ::
CodeType {

// Fields:

	TOrigin left; // the left-handside endpoint of the class
	bool leftIn; // whether the endpoint value is inclusive

	TOrigin right; // the right-handside endpoint of the class
	bool rightIn; // whether the endpoint value is inclusive

	TCode codeAssigned; // the code assigned to this class

// Methods:

	CodeType( TOrigin l0, bool li0, TOrigin r0, bool ri0, TCode ca0)
	: left( l0), leftIn( li0), right( r0), rightIn( ri0), codeAssigned( ca0)
	{}

	~CodeType( void){};

	bool correspond( TOrigin v) const
	{
		if ( left < v && v < right){
			return true;
		}
		if ( leftIn == true && left == v){
			return true;
		}
		if ( rightIn == true && right == v){
			return true;
		}
		return false;
	}

	string getRangeLabel( void) const
	{

		stringstream ss;
		ss << left << " ";
		if ( leftIn == true){
			ss << "<=";
		} else {
			ss << "<";
		}
		ss << " x ";
		if ( rightIn == true){
			ss << "<=";
		} else {
			ss << "<";
		}
		ss << " " << right;

		return ss.str();

	}

};


// 等値判断のためのファンクタクラス：==演算子で判断する。
template <typename T>
class JudgeEqual {
	
public:
	
	JudgeEqual( void){}

	~JudgeEqual( void){}
	
	bool operator()( const T &a, const T &b)
	{
		
		return ( a == b);
		
	}
	
};


// 等値判断のためのファンクタクラス：Toleranceを含んで判断する。
// class to judge equality with tolerance
// i.e. regard a and b equal when a < b + tol and a > b - tol
// thus if T is double and a = 1.0 and tol = 0.1,
// then b can be ( 0.9, 1.1)
template <typename T>
class JudgeEqualTol {
	
private:
	
	T tol; // this is assumed to be non-negative and very small
	
public:
	
	JudgeEqualTol( void){}
	
	JudgeEqualTol( const T &tol0) : tol( tol0){}
	
	~JudgeEqualTol( void){}
	
	void setTol( const T &tol0)
	{
		tol = tol0;
	}

	bool operator()( const T &a, const T &b)
	{		
		return ( ( a < b + tol) && ( a > b - tol));		
	}
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

// ==演算子を用いて、ユニークな値が何個あるかを返す。
// var0に欠損値は含まないと仮定して、ケース数を算出している。
// We assume var0 does not contain invalid values.
template <typename T>
int countUniqueValues( const std::vector <T> &vec0)
{

	if ( vec0.size() < 1){
		return 0;
	}

	std::vector <T> vec( vec0);

	std::sort( vec.begin(), vec.end());

	T prev = vec[ 0];
	int ret = 1;
	for ( auto v : vec){
		if ( v != prev){
			ret++;
			prev = v;
		}
	}

	return ret;

}

// This function is more flexible than FreqType::setFreqFromRecodeType()
// in the sense that this can allow <TOrigin> different from <double>.
// But this function does not make FreqType store rtable itself.
template <typename TOrigin, typename TCode, typename TCount>
void
createFreqFromRecodeTable( 
	FreqType <TCode, TCount> &ret,
	const std::vector <TOrigin> &var,
	const RecodeTable <TOrigin, TCode> &rtable
)
{

	ret.clear();
	std::vector <TCode> codeVec = rtable.getPossibleCodeVec();
	ret.addPossibleKeys( codeVec);
	for ( auto v : var){
		if ( std::isnan( v)){
			// do nothing
		} else {
			TCode codeToAdd = rtable.getCodeForValue( v);
			ret.increment( codeToAdd);
		}
	}

}

template <typename T>
std::vector <T> omitNan( const std::vector <T> &vec0)
{

	std::vector <T> ret;
	ret.reserve( vec0.size());

	for ( const auto &v : vec0){
		if ( std::isnan( v)){
			// do nothing
		} else {
			ret.push_back( v);
		}
	}

	return ret;

}

// returns the sum of all the elements
double sum( const std::vector <double> &dv0)
{

	double s;
	s = 0.0;
	for ( auto v : dv0){
		s += v;
	}
	return s;

}

// returns the mean value
inline double mean( const std::vector <double> &dv0)
{

	double s, ret;
	s = sum( dv0);
	ret = s / ( double)( dv0.size());
	return ret;

}

// returns the median value
double median( const std::vector <double> &dv0)
{
	int n;
	std::vector <double> sorted;
	double ret;

	n = dv0.size();
	sorted = dv0;
	sort( sorted.begin(), sorted.end());

	if ( n % 2 == 0){
		// n is an even number
		ret = ( sorted[ n / 2 - 1] + sorted[ n / 2]) / 2.0;
	} else {
		// n is an odd number
		ret = sorted[ n / 2];
	}

	return ret;
	
}

// DEPRECATED 非推奨
// このアルゴリズムでは精度が落ちるときがあるらしい。
// kstatboostのものを推奨する。
// returns "unbiased" variance
inline double unbiasedVar( const std::vector <double> &v0)
{
	
	int n = v0.size();
	
	double m = mean( v0);
	
	double s2 = 0.0;
	for ( auto &d : v0){
		s2 += d * d;
	}
	
	return ( s2 / ( double)n - m * m) * ( double)n / ( double)( n - 1);
	
}


// DEPRECATED 非推奨
// boost version of mean calculation
// 
// I would not prefer this series,
// because the algorithm for median value is just estimating it;
// not returining accurate values
#include <boost/accumulators/accumulators.hpp> 
#include <boost/accumulators/statistics.hpp> // for stats<> template, tag::mean
double boostMean( const std::vector <double> &dv0)
{

	using namespace boost::accumulators;

	accumulator_set< double, stats< tag::mean > > acc;

	acc = std::for_each( dv0.begin(), dv0.end(), acc);

	return extract::mean( acc);

}

// 長さnの配列へのポインタpを得て、合計を返す。
double sum( const double *p, int n)
{
	
	double s;
	s = 0.0;
	for ( int i = 0; i < n; i++){
		s += p[ i];
	}
	return s;
	
}

// 長さnの配列へのポインタpを得て、平均を返す。
inline double mean( const double *p, int n)
{
	
	return ( sum( p, n) / ( double)n);
	
}

// 長さnの配列へのポインタpを得て、不偏分散を返す。
inline double unbiasedVar( const double *p, int n)
{
	
	double m, s2;
	
	m = mean( p, n);
	
	s2 = 0.0;
	for ( int i = 0; i < n; i++){
		s2 += p[ i] * p[ i];
	}
	
	return ( s2 / ( double)n - m * m) * ( double)n / ( double)( n - 1);
	
}

// convert any type T into string
// using stringstream
template <typename T>
std::string toString( const T &origin)
{
	std::stringstream ss;
	ss.str( "");
	ss << origin;
	return ss.str();
}


/* ********** Definitions of Member Functions ********** */

/* ---------- class FreqType ---------- */

template <typename T, typename TCount>
FreqType <T, TCount> ::
FreqType( void)
: freqmap(),
  areEqual( JudgeEqual<T>()),
  sumcount( static_cast<TCount>( 0))
{
}

template <typename T, typename TCount>
FreqType <T, TCount> ::
~FreqType( void)
{
}

template <typename T, typename TCount>
void
FreqType <T, TCount> ::
clear( void)
{
	
	freqmap.clear();
	sumcount = static_cast<TCount>( 0);
	
}

template <typename T, typename TCount>
void 
FreqType <T, TCount> ::
setEqualExactly( void)
{
	
	areEqual = JudgeEqual<T>();
	
}

template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
setEqualWithTol( const T &tol)
{
	
	areEqual = JudgeEqualTol<T>( tol);
	
}

// add v as a key with no freq when such a key does not exist.
// if the "same" key exists, this does nothing.
// returns true if such a key does not exist.
// returns false if such a key does exist.
// Note that the judgment about equivalence is made 
// simply using "map::find()" function; not using areEqual()
template <typename T, typename TCount>
bool 
FreqType <T, TCount> :: 
addPossibleKey( const T &v)
{
	
	auto it = freqmap.find( v); // ここでfindを使う。areEqualを使っていない。
	if ( it == freqmap.end()){
		freqmap.insert( { v, 0});
		return true;
	}

	return false;
	
}

// adds values in vec as a key with no freq when such a key does not exist.
// if the same key exists, this does nothing.
// returns true if such a key does not exist.
// returns false if such a key does exist.
// We can use this function like below.
//   addPossibleKeys( { 1.0, 2.0, 3.0, 4.0, 5.0});
//   addPossibleKeys( vector <double> ( { 1.0, 2.0, 3.0, 4.0, 5.0}));
template <typename T, typename TCount>
bool 
FreqType <T, TCount> :: 
addPossibleKeys( const std::vector <T> &vec)
{
	
	bool ret = true;

	for ( const T &v : vec){
		bool b = addPossibleKey( v);
		if ( b == false){
			ret = false;
		}

	}

	return ret;
	
}

// increment the count for the key "v"
template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
increment( const T &v)
{
	
	bool exist = false;
	for ( auto &pair : freqmap){
		
		if ( areEqual( pair.first, v) == true){
			( pair.second)++;
			exist = true;
			break;
		}
		
	}
	
	if ( exist == false){
		freqmap.insert( { v, 1});
	}
	
	sumcount++;
	
}

template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
addCount( const T &key, const TCount &count)
{
	
	bool exist = false;
	for ( auto &pair : freqmap){
		
		if ( areEqual( pair.first, key) == true){
			pair.second += count;
			exist = true;
			break;
		}
		
	}
	
	if ( exist == false){
		freqmap.insert( { key, count});
	}
	
	sumcount += count;
	
}

template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
addFreqType( const FreqType <T, TCount> &freqobj)
{
	
	for ( const auto &pair : freqobj.freqmap){
		addCount( pair.first, pair.second);
	}
	
}

// Clears counts into zeros.
// Possible keys and setting for equality judgement are retained.
template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
clearCount( void)
{
	
	for ( auto &pair : freqmap){
		( pair.second) = static_cast<TCount>( 0);
	}
	
	sumcount = static_cast<TCount>( 0);
	
}

template <typename T, typename TCount>
TCount 
FreqType <T, TCount> :: 
getSumCount( void)
const
{
	
	return sumcount;
	
}

// returns vectors of keys and frequency counts
template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
getVectors( std::vector <T> &keyvec, std::vector <TCount> &frevec)
const
{
	
	keyvec.resize( freqmap.size());
	frevec.resize( freqmap.size());
	
	int i = 0;
	for ( const auto &pair : freqmap){
		keyvec[ i] = pair.first;
		frevec[ i] = pair.second;
		i++;
	}
	
}

template <typename T, typename TCount>
double 
FreqType <T, TCount> :: 
meanFromFreq( void)
const
{

	// if sumcount == 0, then return NaN
	if ( sumcount == static_cast<TCount>( 0)){
		alert( "FreqType :: meanFromFreq()");
		return numeric_limits<double>::signaling_NaN();
	}	
	
	double sumval = 0.0;
	
	for ( const auto &pair : freqmap){
		sumval += static_cast<double>( pair.first)
		          * static_cast<double>( pair.second);
	}
	
	return ( sumval / static_cast<double>( sumcount));
	
}

template <typename T, typename TCount>
double 
FreqType <T, TCount> :: 
medianFromFreq( void)
const
{

	// we are using class "map" because it stores data sorted by key
	
	// if sumcount == 0, then return NaN
	if ( sumcount == static_cast<TCount>( 0)){
		alert( "FreqType :: medianFromFreq()");
		return numeric_limits<double>::signaling_NaN();
	}
	
	TCount half;
	half = static_cast<TCount>( floor( static_cast<double>( sumcount) / 2.0));
	bool even = false;
	if ( half * 2 == sumcount){
		even = true;
	}
	
	TCount cumcount = 0;
	double ret1;
	bool waitfornext = false;
	int i = 0;
	
	// in class "map" data are sorted by the key
	for ( const auto &pair : freqmap){
		
		cumcount += pair.second;

		if ( waitfornext == true){

			if ( pair.second > static_cast<TCount>( 0)){
				
				// 頻度総計が偶数で、「累積頻度50%」が2値の間になっており、
				// 大きい方を待っていた状態のとき
				
				return ( ( ret1 + ( double)pair.first) / 2.0);

			}
			
		} else {
			
			if ( even == true){
				
				if ( cumcount > half){
					return static_cast<double>( pair.first);
				} else if ( cumcount == half){
					ret1 = static_cast<double>( pair.first);
					waitfornext = true;
				}

			} else {
				
				if ( cumcount > half){
					return static_cast<double>( pair.first);
				}
				
			}
			
		}
		
	}
	
	alert( "FreqType :: medianFromFreq()");
	return numeric_limits<double>::signaling_NaN();
	
}

template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
modeFromFreq( std::vector <T> &ret)
const
{
		
	// if sumcount == 0, then return a vector of size 0
	if ( sumcount == static_cast<TCount>( 0)){
		alert( "FreqType :: modeFromFreq()");
		ret.clear();
		return;
	}
	
	std::vector <T> keyvec;
	std::vector <TCount> frevec;
	
	getVectors( keyvec, frevec);
	
	std::vector <int> idmax;
	getIndexOfMax( idmax, frevec);
	
	ret.clear();
	for ( const auto &id : idmax){
		ret.push_back( keyvec[ id]);
	}
	
}

// Note that rt0 should be of RecodeTable <double, T>
template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
setFreqFromRecodeTable( const std::vector <double> &vec0, const RecodeTable <double, T> &rt0)
{

	clear();
	rtablep.reset( new RecodeTable<double, T>( rt0));
	std::vector <T> codeVec = rtablep->getPossibleCodeVec();
	addPossibleKeys( codeVec);
	for ( auto v : vec0){
		if ( std::isnan( v)){
			// do nothing
		} else {
			T codeToAdd = rtablep->getCodeForValue( v);
			increment( codeToAdd);
		}
	}

}

// 本来は、Datasetに入れて表示したいが、また今度。
template <typename T, typename TCount>
void
FreqType <T, TCount> :: 
printPadding( std::ostream &os)
const
{

	using namespace std;

	vector < vector <string> > strcols;
	vector <int> width;
	int ncol;

	if ( freqmap.size() < 1){
		alert( "FreqType :: printPadding()");
	}

	// preparation

	ncol = 2;
	if ( bool( rtablep) == true){
		// we will add range label column
		ncol = 3;
	}
	strcols.resize( ncol);
	
	for ( auto &col : strcols){
		col.reserve( freqmap.size() + 1);
	}

	{
		int j = 0;
		strcols[ j].push_back( "Code");
		j++;
		if ( bool( rtablep) == true){
			strcols[ j].push_back( "Label");
			j++;
		}
		strcols[ j].push_back( "Freq");
	}

	for ( auto pair : freqmap){

		const T &key = pair.first;
		const TCount &fre = pair.second;
		
		int j_col = 0;

		// Code
		strcols[ j_col].push_back( toString( key));
		j_col++;

		// Range Label (optional)
		if ( bool( rtablep) == true){
			string labelstr = rtablep->getRangeLabelForCode( pair.first);
			strcols[ j_col].push_back( labelstr);
			j_col++;
		}

		// Freq
		strcols[ j_col].push_back( toString( fre));

	}

	width.resize( strcols.size());
	for ( int i = 0; i < strcols.size(); i++){
		int w0 = 0;
		for ( const auto &s : strcols[ i]){
			if ( w0 < s.size()){
				w0 = s.size();
			}
		}
		width[ i] = w0;
	}
	
	// display
	
	os << setfill( ' ');	
	for ( int i_row = 0; i_row < strcols[ 0].size(); i_row++){
		for ( int j_col = 0; j_col < strcols.size(); j_col++){
			os << setw( width[ j_col]);
			os << strcols[ j_col][ i_row];
			os << " ";
		}
		os << endl;
	}

}

// 各階級の左端と右端のVectorを得る。
// RecodeTypeがない場合には空のVectorが返る。
// Note that obtained vectors should be
// parallel to those obtained by getVectors() above
// これも、上記のprintPadding()と同様に、
// 本来はDatasetに入れて処理したいが、また今度。
template <typename T, typename TCount>
void
FreqType <T, TCount> :: 
getRangeVectors( std::vector <double> &lvec, std::vector <double> &rvec)
const
{

	lvec.clear();
	rvec.clear();

	if ( bool( rtablep) == false){
		return;
	}

	lvec.reserve( freqmap.size());
	rvec.reserve( freqmap.size());

	for ( const auto &pair : freqmap){

		const T &key = pair.first;
		
		double left, right;
		rtablep->getLeftRightForCode( left, right, key);
		lvec.push_back( left);
		rvec.push_back( right);

	}

}


/* ---------- class RecodeTable ---------- */

// 自動で階級を作成する。
// Nから階級の数を設定する。Stataの方式で。
// 最小値と最大値の幅を出して、そこから階級幅を出す。
// 各階級は、左端点を含み、右端点を含まない。
// 有効ケース数が1未満のとき、エラー。
// 有効ケース数が1のとき、階級数は1。
// 最大値と最小値の差がゼロの場合、次のように最大・最小を修正。
// 　値がゼロなら、最小値を-1に、最大値を1にする。
// 　値が正なら、最小値を0に、最大値をそのままにする。
// 　値が負なら、最大値を0に、最小値をそのままにする。
template <typename TOrigin, typename TCode>
void 
RecodeTable <TOrigin, TCode> :: 
setAutoTableFromContVar( const std::vector <TOrigin> &var0)
{
	
	codes.clear();

	std::vector <TOrigin> vec = omitNan( var0);
	int nobs = vec.size();
	if ( nobs < 1){
		alert( "RecodeTable :: setAutoTableFromContVar()");
		return;
	}

	std::sort( vec.begin(), vec.end());
	TOrigin truemin = vec.front();
	TOrigin truemax = vec.back();

	// This way to determine the number of classes is
	// from Stata's histogram command.
	long double log10n = log10( nobs);
	int nclasses = std::round( std::min( ( long double)( std::sqrt( nobs)), 10.0 * log10n));
	if ( nclasses < 1){ // This will occur when nobs==1
		nclasses = 1;
	}

	// Manipulation for the case where width is zero 
	if ( ( long double)truemax - ( long double)truemin <= 0.0){
		nclasses = 1;
		if ( ( long double)truemin == 0.0){
			truemin = static_cast<TOrigin>( -1.0);
			truemax = static_cast<TOrigin>(  1.0);
		} else {
			if ( truemin > 0){
				truemin = static_cast<TOrigin>(  0.0);
			} else {
				truemax = static_cast<TOrigin>(  0.0);
			}
		}
	}

	// Adjustment to make sure all cases would fall inside one of ranges, 
	// even if rounding errors occur
	// adjmin and adjmax are rounded into values with 3-digits accuracy 
	TOrigin truewidth = static_cast<TOrigin>(
		( ( long double)truemax - (long double)truemin) / ( long double)nclasses
	);
	int log10truewidth = std::floor( log10( truewidth));
	long double base = std::pow( 10.0, log10truewidth - 3);
	long double adjmin = std::floor( ( long double)truemin / base - 0.5) * base;
	long double adjmax = std::ceil( ( long double)truemax / base + 0.5) * base;
	long double adjwidth = ( adjmax - adjmin) / ( long double)nclasses;

	for ( int i = 0; i < nclasses; i++){

		TOrigin left  = adjmin + adjwidth * ( long double)i; 
		TOrigin right = adjmin + adjwidth * ( long double)( i + 1);

		CodeType ct = 
			CodeType (
				left, true,
				right, false,
				i + 1
			);
		
		codes.push_back( ct);

	}

	toDoForElse = ElseType :: AssignValue;
	codeForElse = std::numeric_limits<TCode>::quiet_NaN();

}

// codeになりうる値のvectorを返す。
// ソートして返す。
// ただし、Elseの場合のcodeは含めない。
template <typename TOrigin, typename TCode>
std::vector <TCode> 
RecodeTable <TOrigin, TCode> :: 
getPossibleCodeVec( void) const
{

	std::vector <TCode> ret;
	ret.clear();

	for ( auto c : codes){
		ret.push_back( c.codeAssigned);
	}
	std::sort( ret.begin(), ret.end());

	return ret;

}

// vに対応するコードを返す。
template <typename TOrigin, typename TCode>
TCode
RecodeTable <TOrigin, TCode> :: 
getCodeForValue( TOrigin v)
const
{

	for ( auto c : codes){
		if ( c.correspond( v) == true){
			return c.codeAssigned;
		}
	}

	TCode ret;

	switch( toDoForElse){
		case ElseType :: Copy:
			ret = v;
			break;
		case ElseType :: AssignValue:
			ret = codeForElse;
			break;
	}

	return ret;

}

// code0に対応するleftとrightを返す。
// code0に対応する範囲が登録されていなければ、nanを返す。
template <typename TOrigin, typename TCode>
void
RecodeTable <TOrigin, TCode> :: 
getLeftRightForCode( TOrigin &lret, TOrigin &rret, TCode code0)
const
{

	for ( const auto &c : codes){
		if ( c.codeAssigned == code0){
			lret = c.left;
			rret = c.right;
			return; 
		}
	}

	lret = std::numeric_limits<TOrigin>::quiet_NaN();
	rret = std::numeric_limits<TOrigin>::quiet_NaN();

}

// code0に対応するラベルを返す。
// code0に対応する範囲が登録されていなければ、空のstringを返す。
template <typename TOrigin, typename TCode>
std::string
RecodeTable <TOrigin, TCode> :: 
getRangeLabelForCode( TCode code0)
const
{

	for ( const auto &c : codes){
		if ( c.codeAssigned == code0){
			return c.getRangeLabel();
		}
	}

	return string( "");

}

// ストリームに出力する。
// スペースでパディングはしない。sepで区切る。
template <typename TOrigin, typename TCode>
void
RecodeTable <TOrigin, TCode> :: 
print(
	ostream &os,
	string sep // = ","s
)
const
{

	os << "Code" << sep << "Range" << std::endl;
	for ( auto c : codes){
		os << c.codeAssigned;
		os << sep;
		os << c.getRangeLabel();
		os << endl;
	}

}

#endif /* kstat_cpp_include_guard */
