
/*
	ksimpleds
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2022 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Simple Dataset 

	Comile Options: -EHsc -utf-8 -std:c++17 
	
	事実上、dev01.cppがテストプログラムであった。
	
	
	※SimpleDatasetを使うのがよいとも限らない。
	　CaseTypeをそのたびに定義してその中で操作する方が効率的な場合があるかも。
	　以下の書き方など。
	std::vector<int> result( v.size());
	std::copy_if(
		v.begin(), v.end(), std::back_inserter( result),
		[](int x) { return x < 5; }
	);
	std::transform(
		result.begin(), result.end(), result.begin(),
		[](int x) { return x + 3; }
	);
https://ja.stackoverflow.com/questions/61829/c%E3%81%A7%E3%81%AE%E9%96%A2%E6%95%B0%E5%9E%8B%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0%E3%81%AF%E8%89%AF%E3%81%84%E7%B5%90%E6%9E%9C%E3%82%92%E3%82%82%E3%81%9F%E3%82%89%E3%81%97%E3%81%BE%E3%81%99%E3%81%8B

*/


/* ********** Preprocessor Directives ********** */

#ifndef ksimpledataset_cpp_include_guard
#define ksimpledataset_cpp_include_guard

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <sstream>
#include <numeric>
#include <k09/koutputfile01.cpp>
#include <k09/krand02.cpp>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Type Declarations: enum, class, etc. ********** */

class SimpleDataset;


/* ********** Function Declarations ********** */


/* ********** Type Definitions: enum, class, etc. ********** */

class SimpleDataset {

public:

	/* ***** Internal Classes ***** */

	struct SimpleDataColumn {
		
		std::string vname;
		std::vector <double> vals;
		
		SimpleDataColumn( void) : vname(), vals(){}
		~SimpleDataColumn( void){}

		std::string getValueStringFormat( int r0)
		{
			
			if ( r0 >= vals.size()){
				throwMsgExcept( "", "index out of bound");
			}

			if ( isnan( vals[ r0])){
				return ".";
			}

			std::stringstream ss;
			ss << vals[ r0];
			return ss.str();

		}

	};
	
	/* ***** Data Fields ***** */

	std::vector <SimpleDataColumn> dcvec;

	/* ***** Methods ***** */

	SimpleDataset( void) : dcvec(){};
	SimpleDataset( const SimpleDataset &obj0) : dcvec()
	{
		copyFrom( obj0);
	}

	~SimpleDataset( void){};

	SimpleDataset & operator=( const SimpleDataset &obj0)
	{
		copyFrom( obj0);
		return *this; 
	}

	void copyFrom( const SimpleDataset &obj0)
	{
		dcvec = obj0.dcvec; 
	}

	void setVarNames( const std::vector <std::string> &vec0)
	{
		dcvec.clear();
		dcvec.resize( vec0.size());
		for ( int i = 0; i < vec0.size(); i++){
			dcvec[ i].vname = vec0[ i];
		}
	}

	// add only a value for one-case one-variable 
	// throws exception if the variable does not exist 
	// this may cause the dataset non-rectangular 
	// using only doubles as far 
	void addCase( const std::string &vn0, double v0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == true){
			dcvec[ idx].vals.push_back( v0);
		} else {
			throwMsgExcept( "", "variable not found");
		}

	}

	// add a vector for one-variable 
	// throws exception if the variable does not exist 
	// this will usually cause the dataset non-rectangular 
	// using only doubles as far 
	void addVector( const std::string &vn0, const std::vector <double> &vec0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == true){
			auto &alias = dcvec[ idx].vals;
			alias.insert( alias.end(), vec0.begin(), vec0.end());
		} else {
			throwMsgExcept( "", "variable not found");
		}

	}

	// add a set of sequntial numbers
	// starting "s0", of size "len0"
	// throws exception if the variable does not exist 
	// this will usually cause the dataset non-rectangular 
	// using only doubles as far 
	void addSequentialNumber( const std::string &vn0, double s0, int len0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);

		if ( b == true){

			std::vector <double> vec0( len0);
			std::iota( vec0.begin(), vec0.end(), s0);
			auto &alias = dcvec[ idx].vals;
			alias.insert( alias.end(), vec0.begin(), vec0.end());

		} else {

			throwMsgExcept( "", "variable not found");

		}

	}

	// add a vector of a constatn "c0" of the specified size "len0" 
	// throws exception if the variable does not exist 
	// this will usually cause the dataset non-rectangular 
	// using only doubles as far 
	void addConstant( const std::string &vn0, double c0, int len0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);

		if ( b == true){

			std::vector <double> vec0( len0, c0);
			auto &alias = dcvec[ idx].vals;
			alias.insert( alias.end(), vec0.begin(), vec0.end());

		} else {

			throwMsgExcept( "", "variable not found");

		}

	}

	// returns the vector of the variable named "vn0"
	// throws exception if the variable does not exist 
	std::vector <double> getVector( const std::string &vn0)
	{

		auto [ b, idx] = getColumnIndex( vn0);

		if ( b == false){
			throwMsgExcept( "", "variable not found");
		}

		return dcvec[ idx].vals;

	}

	// returns the vector of the variable named "vn0", only for the cases where 
	// the value of "ifvn0" satisfies the condition given by the function "func0"
	// throws exception if the variable does not exist 
	std::vector <double> getVectorIf( const std::string &vn0, const std::string &ifvn0, std::function <bool(double)> func0)
	{

		auto [ b1, idx1] = getColumnIndex( vn0);
		if ( b1 == false){
			throwMsgExcept( "", "variable not found: " + vn0);
		}

		auto [ b2, idx2] = getColumnIndex( ifvn0);
		if ( b2 == false){
			throwMsgExcept( "", "variable not found: " + ifvn0);
		}

		const auto &vals_to_return = dcvec[ idx1].vals;
		const auto &vals_for_cond = dcvec[ idx2].vals;

		int len = vals_to_return.size();
		if ( len != vals_for_cond.size()){
			throwMsgExcept( "", "sizes of two variables not equal");
		}

		std::vector <double> ret;
		for ( int i = 0; i < len; i++){
			if ( func0( vals_for_cond[ i]) == true){
				ret.push_back( vals_to_return[ i]);
			}
		}

		return ret;
		
	}

	// returns the vector of the variable named "vn0", only for the cases where 
	// the certain condition given by "func0" is satifcied
	// "func0" will return bool taking as input the map
	// which includes varnames and values
	// throws exception if the variable to use does not exist 
	// 
	// inside "func0" we should use map::at() to throw exception for erroneous varname
	// CAUTION: very slow because for each case one map object will be created
	std::vector <double>
	getVectorIf( 
		const std::string &vn0,
		std::function < bool( std::map <std::string, double> ) > func0
	)
	{

		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == false){
			throwMsgExcept( "", "variable not found: " + vn0);
		}

		const auto &vals_to_return = dcvec[ idx].vals; // vector <double> & 

		int len = vals_to_return.size();

		std::vector <double> ret;
		ret.reserve( len);

		std::map <std::string, double> casemap;
		for ( const auto &vec : dcvec){
			casemap.insert( std::make_pair( vec.vname, 0.0));
		}

		try {

			for ( int i = 0; i < len; i++){

				for ( const auto &vec : dcvec){
					casemap.at( vec.vname) = vec.vals[ i];
				}

				if ( func0( casemap) == true){
					ret.push_back( vals_to_return[ i]);
				}

			}

		} catch ( const std::out_of_range &e) {

			throwMsgExcept( "", "variable used in condition func was not found");

		}

		return ret;
		
	}	

	// returns the vector of the variable named "vn0", only for the cases 
	// where the certain condition given by "func0" is satifcied
	// "func0" will return bool taking as input varname->double func object
	// throws exception if the variable to use does not exist 
	std::vector <double>
	getVectorIf( 
		const std::string &vn0,
		std::function < bool( std::function <double(std::string)> ) > func0
	)
	{
		
		// この中で、変数名とindexの対応のmapをつくり、accessorもつくっておけば、もっと速くなりそう。
		// →やってみたが、むしろ遅くなった。。。
		
		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == false){
			throwMsgExcept( "", "variable not found: " + vn0);
		}

		const auto &vals_to_return = dcvec[ idx].vals; // vector <double> & 

		int len = vals_to_return.size();

		std::vector <double> ret;
		ret.reserve( len);

		for ( int i = 0; i < len; i++){

			std::function < double( std::string) > casevaluefunc =
				[&]( std::string vn0)->double
				{
					return value( vn0, i);
				};
			
			if ( func0( casevaluefunc) == true){
				ret.push_back( vals_to_return[ i]);
			}

		}

		return ret;

	}

	// accessor
	double & value( const std::string &vn0, int caseid0)
	{

		auto [b, idx] = getColumnIndex( vn0);
		if ( b == false){
			throwMsgExcept( "", "variable not found");
		}
		auto &alias = dcvec[ idx].vals;
		if ( caseid0 < 0 || caseid0 >= alias.size()){
			throwMsgExcept( "", "case id out of bound");
		}
		
		return alias[ caseid0];

	} 

	// returns:
	//   bool: true if the variable exists
	//   int: index of the column for the variable if it exists; 0 if not
	std::tuple <bool, int> getColumnIndex( const std::string &vn0)
	{

		for ( int i = 0; i < dcvec.size(); i++){
			if ( dcvec[ i].vname == vn0){
				return { true, i};
			}
		}

		return { false, 0};

	}

	// check if dataset is recutanglar
	// returns true if reculangular (or there is no column)
	// typically useful after calling addCase() 
	bool isRectanglar( void)
	{

		if ( dcvec.size() < 1){
			return true;
		}

		int n = dcvec[ 0].vals.size();
		for ( int i = 1; i < dcvec.size(); i++){
			if ( dcvec[ i].vals.size() != n){
				return false;
			}
		}

		return true;

	}
	
	// throws exception if non-rectangular 
	void assertRecutangular( void)
	{
		if ( isRectanglar() == false){
			throwMsgExcept( "", "dataset is not rectangular");
		}
	}

	void writeToFile( koutputfile &kof0, const std::string &sep0 = "\t")
	{

		if ( dcvec.size() < 1){
			return;
		}

		// write variable names 
		{
			std::string s;
			s = dcvec[ 0].vname;
			for ( int i = 1; i < dcvec.size(); i++){
				s += sep0;
				s += dcvec[ i].vname;
			}
			kof0.writeLine( s);
		}

		// write values
		{
			for ( int i_row = 0; i_row < dcvec[ 0].vals.size(); i_row++){

				int i_col = 0;

				auto &stream = kof0.stream();

				// std::stringstream ss;
				stream << dcvec[ i_col].getValueStringFormat( i_row);

				for ( i_col = 1; i_col < dcvec.size(); i_col++){
					stream << sep0;
					stream << dcvec[ i_col].getValueStringFormat( i_row);
				}

				stream << std::endl;

				// kof0.writeLine( ss.str());

			}
		}

	}

	// returns the length of the longetst column
	// this works even if no variable exists 
	// or the dataset is non-rectangular
	int size( void) const 
	{
		
		if ( dcvec.size() < 1){
			return 0;
		}

		int ret = dcvec[ 0].vals.size();
		for ( int i = 1; i < dcvec.size(); i++){
			int n = dcvec[ i].vals.size();
			if ( n > ret){
				ret = n;
			}
		}

		return ret;

	}

	// *thisを母集団として、標本を返す。非復元抽出による。
	// *thisの要素から、ランダムに（rneを用いて）、ssizeだけの要素を選び、同じ型にして返す。
	// kalgo.cppにあるgetSample()と同様のアルゴリズムで実装されている。
	SimpleDataset getSample( RandomNumberEngine &rne, int ssize)
	{

		int npop = size();

		if ( ssize > npop){
			throwMsgExcept( "sample size should not exceed population size");
		}

		std::map <int, bool> idmap;

		int ndrawn = 0;
		while ( ndrawn < ssize){
			int randomid = rne.getDiscreteUniform( 0, npop - 1);
			auto it = idmap.find( randomid);
			if ( it == idmap.end()){
				idmap[ randomid] = true;
				ndrawn++;
			}
		}

		SimpleDataset ret{};

		for ( const auto &dc : dcvec){

			SimpleDataColumn newdc{};
			newdc.vname = dc.vname;
			newdc.vals.resize( ssize);

			auto it = idmap.begin();
			for ( int i = 0; i < ssize; i++){
				newdc.vals[ i] = dc.vals[ it -> first];
				it++;
			}

			ret.dcvec.push_back( newdc);

		}

		return ret;

	}

};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */


/* ********** Definitions of Member Functions ********** */


#endif /* ksimpledataset_cpp_include_guard */
