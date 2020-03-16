
/*
	kstat
	Ver. k09.02
	
	Written by Koji Yamamoto
	Copyright (C) 2015-2020 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Module for Statistical Computations
	
	Note:
	As for FreqType, possibly in the future we should refer the following libraries:
	　GSL's Histogram
	
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

#include <k09/kutil01.cpp>
#include <k09/kalgo00.cpp>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Type Declarations: enum, class, etc. ********** */

template <typename T>
class JudgeEqual;

template <typename T>
class JudgeEqualTol;

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
double boostMean( const std::vector <double> &); // 非推奨。deprecated. 
double sum( const double *, int);
double mean( const double *, int);
double unbiasedVar( const double *, int);


/* ********** Type Definitions: enum, class, etc. ********** */

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

	// Note that we specify int as TCode for RecodeTable
	std::unique_ptr < RecodeTable<double, int> > rtablep;
	
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

	void setFreqFromRecodeTable( const std::vector <double> &, const RecodeTable <double, int> &);

};


// TCode is int by default
template <typename T, typename TCode /* = int */>
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

	void setAutoTableFromContVar( const std::vector <T> &);
	std::vector <TCode> getPossibleCodeVec( void) const;
	TCode getCodeForValue( T) const;
	void print( ostream &, string = ","s) const;

};

template <typename T, typename TCode>
struct
RecodeTable <T, TCode> ::
CodeType {

// Fields:

	T left; // the left-handside endpoint of the class
	bool leftIn; // whether the endpoint value is inclusive

	T right; // the right-handside endpoint of the class
	bool rightIn; // whether the endpoint value is inclusive

	TCode codeAssigned; // the code assigned to this class

// Methods:

	CodeType( T l0, bool li0, T r0, bool ri0, TCode ca0)
	: left( l0), leftIn( li0), right( r0), rightIn( ri0), codeAssigned( ca0)
	{}

	~CodeType( void){};

	bool correspond( T v) const
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

// This function is more general than FreqType::setFreqFromRecodeType()
// in the sense that this can allow <TOrigin, TCode> different than <double, int>.
// But this function does not make FreqType have rtable itself.
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

// Note that rt0 should be of type RecodeTable <double, int>
template <typename T, typename TCount>
void 
FreqType <T, TCount> :: 
setFreqFromRecodeTable( const std::vector <double> &vec0, const RecodeTable <double, int> &rt0)
{

	clear();
	rtablep.reset( new RecodeTable<double, int>( rt0));
	std::vector <int> codeVec = rtablep->getPossibleCodeVec();
	addPossibleKeys( codeVec);
	for ( auto v : vec0){
		if ( std::isnan( v)){
			// do nothing
		} else {
			int codeToAdd = rtablep->getCodeForValue( v);
			increment( codeToAdd);
		}
	}

}


/* ---------- class RecodeTable ---------- */

// 自動で階級を作成する。
// Nから階級の数を設定する。Stataの方式で。
// 最小値と最大値の幅を出して、そこから階級幅を出す。
// 各階級は、左端点を含み、右端点を含まない。
template <typename T, typename TCode>
void 
RecodeTable <T, TCode> :: 
setAutoTableFromContVar( const std::vector <T> &var0)
{
	
	std::vector <T> vec = omitNan( var0);
	std::sort( vec.begin(), vec.end());
	
	T min = *( vec.begin());
	T max = *( vec.end() - 1);
	int nobs = vec.size();

	// This way to determine the number of classes is
	// from Stata's histogram command.
	int nclasses = std::round( std::min( std::sqrt( nobs), 10 * log10( nobs)));

	T width = static_cast<T>( std::ceil( ( long double)( max - min) / ( long double)nclasses));

	T left = min;
	T right;
	codes.clear();
	for ( int i = 1; left <= max; i++, left = right){

		right = left + width;

		CodeType ct = 
			CodeType (
				left, true,
				right, false,
				i
			);
		
		codes.push_back( ct);

	}

	toDoForElse = ElseType :: AssignValue;
	codeForElse = std::numeric_limits<TCode>::quiet_NaN();

}

// codeになりうる値のvectorを返す。
// ソートして返す。
// ただし、Elseの場合のcodeは含めない。
template <typename T, typename TCode>
std::vector <TCode> 
RecodeTable <T, TCode> :: 
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
template <typename T, typename TCode>
TCode
RecodeTable <T, TCode> :: 
getCodeForValue( T v)
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

// ストリームに出力する。
// スペースでパディングはしない。sepで区切る。
template <typename T, typename TCode>
void
RecodeTable <T, TCode> :: 
print( ostream &os, string sep = ","s)
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
