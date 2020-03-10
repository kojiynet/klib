
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
template <typename T, typename TCount>
class FreqType;


/* ********** Function Declarations ********** */

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
template <typename T, typename TCount = int>
class FreqType {
	
private:
	
	map <T, TCount> freqmap;
	std::function < bool( const T &, const T &) > areEqual;
	TCount sumcount;
	
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
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

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


#endif /* kstat_cpp_include_guard */
