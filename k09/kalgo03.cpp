
/*
	kalgo
	Ver. k09.03
	
	Written by Koji Yamamoto
	Copyright (C) 2014-2021 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Library for Algorithmic Solution
	(possibly useful for simulations)
*/


/* ********** Preprocessor Directives ********** */

#ifndef kalgo_cpp_include_guard
#define kalgo_cpp_include_guard

#include <vector>
#include <algorithm>
#include <k09/kutil02.cpp>
#include <k09/krand02.cpp>


/* ********** Using Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */

template <typename TKey, typename T>
class Paravec;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

template <typename T>
void calcRanksFromValues( vector <int> &, const vector <T> &, RandomNumberEngine &);
template <typename T>
void randomizeOrder( vector <T> &, RandomNumberEngine &);
template <typename T>
void getIndexOfMax( vector <int> &, const vector <T> &);
template <typename T>
int getIndexOfMaxRandomized( const vector <T> &, RandomNumberEngine &);
template <typename T>
void getIndexOfMin( vector <int> &, const vector <T> &);
template <typename T>
int getIndexOfMinRandomized( const vector <T> &, RandomNumberEngine &);
template <typename T>
int getIndexOfElement( const vector <T> &, T);
template <typename TData, typename TKey>
bool getCopyOfElementForKey( TData &, const vector <TData> &, const vector <TKey> &, TKey);
template <typename TData, typename TKey>
bool getPointerToElementForKey( TData * &, const vector <TData> &, const vector <TKey> &, TKey);
template <typename T>
T getSumOfElements( const vector <T> &, T);

template <typename T> 
std::vector <T> getSample( const std::vector <T> &, RandomNumberEngine &, int);


/* ********** Class Definitions ********** */

// Contains two parallel vectors, one of which is a Key
template <typename TKey, typename T>
class Paravec {
	
private:
	
public:
	
	vector <TKey> keys;
	vector <T> values;
	
	Paravec( void);
	~Paravec( void);
	
	void copyFrom( const Paravec &);
	void initialize( const vector <TKey> &, const vector <T> &);
	bool addValue( TKey, T);
	bool keyExists( TKey) const;
	bool checkUniqueKeys( void) const;
	int getSize( void) const;
	bool getValueOfKey( T &, TKey) const;
	
	const vector <TKey> & getKeyVecRef( void) const;
	const vector <T> & getValueVecRef( void) const;
	T & getElementRefOfKey( TKey);
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

/*
	dataの各要素の大小を比較し、対応する順位を算出し、dataのk番目の要素の順位をretrank[ k]に入れる。
	値の等しい要素の集合に対しては、乱数を振って、ランダムに順位を付す。
	乱数の生成のためのRandomNumberEngineインスタンスを必要とする。
	型Tに対して、不等号・等号・代入演算子が定義されている必要がある。
*/
template <typename T>
void calcRanksFromValues( vector <int> &retranks, const vector <T> &data, RandomNumberEngine &rne)
{
	
	int n;
	vector <T> v0;
	vector <int> ids;
	bool loop;
	T vtemp;
	int idtemp;
	
	v0 = data;
	n = v0.size();
	
	ids.clear();
	retranks.clear();
	for ( int i = 0; i < n; i++){
		ids.push_back( i);
		retranks.push_back( i + 1);
	}
	
	loop = true;
	while ( loop){
		
		loop = false;
		for ( int i = 0; i < n - 1; i++){
			if ( v0[ i] < v0[ i + 1]){
				vtemp = v0[ i];
				v0[ i] = v0[ i + 1];
				v0[ i + 1] = vtemp;
				idtemp = ids[ i];
				ids[ i] = ids[ i + 1];
				ids[ i + 1] = idtemp;
				loop = true;
			}
		}
		
	}
	
	// Randomize ties below
	int p1, p2;
	vector <int> tiepart;
	p1 = 0;
	p2 = 0;
	while ( p1 < n && p2 < n){
		
		while ( p2 < n && v0[ p1] == v0[ p2]){
			p2++;
		}
		p2--;
		
		if ( p1 < p2){ // when there is a tie
			
			tiepart.assign( ids.begin() + p1, ids.begin() + p2 + 1);
			randomizeOrder( tiepart, rne);
			for ( int j = 0; j <= p2 - p1; j++){
				ids[ p1 + j] = tiepart[ j];
			}
			
			p1 = p2 + 1;
			p2 = p1;
			
		} else { // when there is no tie
			
			p1++;
			p2 = p1;
			
		}
		
	}
	
	for ( int j = 0; j < n; j++){
		
		retranks[ ids[ j]] = j + 1;
		
	}
	
}

/*
	dataの要素の順番を乱数によってランダマイズする。
	dataの内容が書き換えられる。
	乱数生成のためのRandomNumberEngineインスタンスを必要とする。
	型Tに対して、代入演算子が定義されている必要がある。
*/
template <typename T>
void randomizeOrder( vector <T> &data, RandomNumberEngine &rne)
{
	
	int n;
	vector <double> seq;
	bool loop;
	T temp;
	double tempd;
	
	n = data.size();
	seq.clear();
	seq.resize( n);
	for ( int i = 0; i < n; i++){
		seq[ i] = rne.getRealUniform( 0.0, 1.0);
	}
	
	loop = true;
	while ( loop){
		
		loop = false;
		for ( int i = 0; i < n - 1; i++){
			if ( seq[ i] < seq[ i + 1]){
				tempd = seq[ i];
				seq[ i] = seq[ i + 1];
				seq[ i + 1] = tempd;
				temp = data[ i];
				data[ i] = data[ i + 1];
				data[ i + 1] = temp;
				loop = true;
			}
		}
		
	}	
	
}

/*
	dataの要素のうち、最大の要素の添え字（0～(size()-1)）の集合をretに格納する。
	最大の要素が複数ある場合には、retのsizeが2以上になる。
	型Tに対して、不等号と等号が定義されている必要がある。
*/
template <typename T>
void getIndexOfMax( vector <int> &ret, const vector <T> &data)
{
	
	int n;
	
	n = data.size();
	ret.clear();
	if ( n < 1){
		alert( "getIndexOfMax()");
		return;
	}
	
	ret.push_back( 0);
	for ( int i = 1; i < n; i++){
		
		if ( data[ i] > data[ ret[ 0]]){
			ret.clear();
			ret.push_back( i);
		} else if ( data[ i] == data[ ret[ 0]]){
			ret.push_back( i);
		}
		
	}
	
}

/*
	dataの要素のうち、最大の要素の添え字（0～(size()-1)）を返す。
	最大の要素が複数ある場合には、ランダムに等確率でいずれかを選んで添え字を返す。
	その際のランダムネスはrneによってもたらされる。
	型Tに対して、不等号と等号が定義されている必要がある。
*/
template <typename T>
int getIndexOfMaxRandomized( const vector <T> &data, RandomNumberEngine &rne)
{
	
	vector <int> vec;
	
	getIndexOfMax( vec, data);
	if ( vec.size() > 1){
		randomizeOrder( vec, rne);
	} else if ( vec.size() < 1){
		alert( "getIndexOfMaxRandomized()");
		return -1;
	}
	
	return vec[ 0];
	
}

/*
	dataの要素のうち、最小の要素の添え字（0～(size()-1)）の集合をretに格納する。
	最小の要素が複数ある場合には、retのsizeが2以上になる。
	型Tに対して、不等号と等号が定義されている必要がある。
*/
template <typename T>
void getIndexOfMin( vector <int> &ret, const vector <T> &data)
{
	
	int n;
	
	n = data.size();
	ret.clear();
	if ( n < 1){
		alert( "getIndexOfMin()");
		return;
	}
	
	ret.push_back( 0);
	for ( int i = 1; i < n; i++){
		
		if ( data[ i] < data[ ret[ 0]]){
			ret.clear();
			ret.push_back( i);
		} else if ( data[ i] == data[ ret[ 0]]){
			ret.push_back( i);
		}
		
	}
	
}

/*
	dataの要素のうち、最小の要素の添え字（0～(size()-1)）を返す。
	最大の要素が複数ある場合には、ランダムに等確率でいずれかを選んで添え字を返す。
	その際のランダムネスはrneによってもたらされる。
	型Tに対して、不等号と等号が定義されている必要がある。
*/
template <typename T>
int getIndexOfMinRandomized( const vector <T> &data, RandomNumberEngine &rne)
{
	
	vector <int> vec;
	
	getIndexOfMin( vec, data);
	if ( vec.size() > 1){
		randomizeOrder( vec, rne);
	} else if ( vec.size() < 1){
		alert( "getIndexOfMinRandomized()");
		return -1;
	}
	
	return vec[ 0];
	
}

/*
	dataの要素のうち、前方から探索して最初に見つかるeleと等しい要素のIndex（0はじまり）を返す。
	見つからない場合は、-1を返す。
	型Tに対して等号が定義されている必要がある。
*/
template <typename T>
int getIndexOfElement( const vector <T> &data, T ele)
{
	
	typename vector <T> :: const_iterator pos;
	
	if ( data.size() < 1){
		return -1;
	}
	
	pos = find( data.begin(), data.end(), ele);
	if ( pos == data.end()){
		return -1;
	}
	
	return ( pos - data.begin());
	
}

/*
	datavecとkeyvecはパラレルベクターであると見なし、
	keyvec内を前方から探索し、最初に見つかるkeyと等しい要素の添え字に対応する、
	datavecの要素を、ansにコピーする（代入演算子を用いる）。
	見つかった場合はtrue、見つからない場合は、falseを返す。
	型TKeyに等号が定義されている必要がある。
	型TDataに代入演算子が定義されている必要がある。
*/
template <typename TData, typename TKey>
bool getCopyOfElementForKey(
	TData &ans,
	const vector <TData> &datavec, const vector <TKey> &keyvec, TKey key
)
{
	
	int idx;
	
	if ( datavec.size() != keyvec.size()){
		alert( "getCopyOfElementForKey()");
		ans = TData();
		return false;
	}
	
	idx = getIndexOfElement( keyvec, key);
	if ( idx == -1){
		alert( "getCopyOfElementForKey()");
		ans = TData();
		return false;
	}
	
	ans = datavec[ idx];
	return true;
	
}

/*
	datavecとkeyvecはパラレルベクターであると見なし、
	keyvec内を前方から探索し、最初に見つかるkeyと等しい要素の添え字に対応する、
	datavecの要素のアドレスを、ptrに代入する。
	見つかった場合はtrue、見つからない場合は、falseを返す。
	型TKeyに等号が定義されている必要がある。
*/
template <typename TData, typename TKey>
bool getPointerToElementForKey( TData * &ptr, const vector <TData> &datavec, const vector <TKey> &keyvec, TKey key){
	
	int idx;
	
	if ( datavec.size() != keyvec.size()){
		alert( "getPointerToElementForKey()");
		ptr = NULL;
		return false;
	}
	
	idx = getIndexOfElement( keyvec, key);
	if ( idx == -1){
		alert( "getPointerToElementForKey()");
		ptr = NULL;
		return false;
	}
	
	ptr = &( datavec[ idx]);
	return true;
	
}

/*
	vecの要素の合計を返す。
	合計とは、offsetの値に演算子+=を逐次適用していった結果である。
	通常、offsetは0や0.0などの意味の値を指定する。
	型Tに対して演算子=と+=が定義されている必要がある。
*/
template <typename T>
T getSumOfElements( const vector <T> &vec, T offset)
{
	
	int n;
	T ret;
	
	n = vec.size();
	ret = offset;
	
	for ( int i = 0; i < n; i++){
		ret += vec[ i];
	}
	
	return ret;
	
}

/*
	母集団から標本を返す。非復元抽出による。
	popvec0の要素から、ランダムに（rneを用いて）、nobsだけの要素を選び、それをvectorに入れて返す。
	型Tに対して代入演算子が定義されている必要がある。
*/
template <typename T> 
std::vector <T> getSample( const std::vector <T> &popvec0, RandomNumberEngine &rne, int nobs)
{

	int npop = popvec0.size();

	std::map <int, bool> idmap;

	int ndrawn = 0;
	while ( ndrawn < nobs){
		int randomid = rne.getDiscreteUniform( 0, npop - 1);
		auto it = idmap.find( randomid);
		if ( it == idmap.end()){
			idmap[ randomid] = true;
			ndrawn++;
		}
	}
	
	vector <T> ret( nobs);
	auto it = idmap.begin();
	for ( int i = 0; i < nobs; i++){
		ret[ i] = popvec0[ it -> first];
		it++;
	}

	return ret;
	 
}


/* ********** Definitions of Member Functions ********** */

template <typename TKey, typename T>
Paravec <TKey, T> :: Paravec( void)
: keys(), values()
{
}

template <typename TKey, typename T>
Paravec <TKey, T> :: ~Paravec( void)
{
}

/*
	Copies data from other object
*/
template <typename TKey, typename T>
void Paravec <TKey, T> :: copyFrom( const Paravec &obj)
{
	
	keys = obj.keys;
	values = obj.values;
	
}

/*
	Initializes pararell vectors by two vectors
*/
template <typename TKey, typename T>
void Paravec <TKey, T> :: initialize( const vector <TKey> &keys0, const vector <T> &v0)
{
	
	int n;
	bool b;
	
	n = keys0.size();
	if ( v0.size() != n){
		alert( "Paravec :: initialize()");
		return;
	}
	
	// check about uniqueness of values in keys0
	b = true;
	for ( int i = 0; i < n - 1; i++){
		for ( int j = i + 1; j < n; j++){
			if ( keys0[ i] == keys0[ j]){
				b = false;
			}
		}
	}
	if ( b == false){
		alert( "Paravec :: initialize()");
		return;
	}
	
	keys = keys0;
	values = v0;
	
}

// returns true if the addition is done successfully
// false otherwise.
template <typename TKey, typename T>
bool Paravec <TKey, T> :: addValue( TKey key0, T v0)
{
	
	int n;
	
	n = values.size();
	if ( n != keys.size()){
		alert( "Paravec :: addValue()");
		return false;
	}
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == key0){
			alert( "Paravec :: addValue()");
			return false;
		}
	}
	
	keys.push_back( key0);
	values.push_back( v0);
	return true;
	
}

// returns true if "key0" exists in the key vector
template <typename TKey, typename T>
bool Paravec <TKey, T> :: keyExists( TKey key0) const
{
	
	int n;
	
	n = values.size();
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == key0){
			return true;
		}
	}
	
	return false;
	
}

// returns true if all the stored keys are unique
template <typename TKey, typename T>
bool Paravec <TKey, T> :: checkUniqueKeys( void) const
{
	
	int n;
	bool b;
	
	n = keys.size();
	if ( values.size() != n){
		alert( "Paravec :: checkUniqueKeys()");
		return false;
	}
	
	// check about uniqueness of values in keys
	b = true;
	for ( int i = 0; i < n - 1; i++){
		for ( int j = i + 1; j < n; j++){
			if ( keys[ i] == keys[ j]){
				b = false;
			}
		}
	}
	
	return b;
	
}

// returns the size of vectors
template <typename TKey, typename T>
int Paravec <TKey, T> :: getSize( void) const
{
	
	int n;
	n = keys.size();
	if ( n != values.size()){
		alert( "Paravec :: getSize()");
	}
	
	return n;
	
}

// get "ret" contain the value corresponding to "key0"
// returns true if it successfully found the key
// returns false if it did not
template <typename TKey, typename T>
bool Paravec <TKey, T> :: getValueOfKey( T &ret, TKey key0) const
{
	
	int n;
	
	n = keys.size();
	if ( n != values.size()){
		alert( "Paravec :: getValueOfKey()");
	}
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == key0){
			ret = values[ i];
			return true;
		}
	}
	
	return false;
	
}

// returns const reference to vector "keys" itself
template <typename TKey, typename T>
const vector <TKey> &
Paravec <TKey, T> :: getKeyVecRef( void) const
{
	
	return keys;
	
}

// returns const reference to vector "values" itself
template <typename TKey, typename T>
const vector <T> &
Paravec <TKey, T> :: getValueVecRef( void) const
{
	
	return values;
	
}

// returns the reference to the element corresponding to "key0"
// if failed, returns the reference to the first element
template <typename TKey, typename T>
T & Paravec <TKey, T> :: getElementRefOfKey( TKey key0)
{
	
	int n;
	
	n = keys.size();
	if ( n != values.size()){
		alert( "Paravec :: getElementRefOfKey()");
	}
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == key0){
			return values[ i];
		}
	}
	
	alert( "Paravec :: getElementRefOfKey()");
	return values[ 0];
	
}


#endif /* kalgo_cpp_include_guard */
