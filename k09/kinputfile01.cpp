
/*
	
	kinputfile
	Ver. k09.01
	
	Written by Koji Yamamoto
	Copyright (C) 2016-2020 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	ファイル読み込み
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kinputfile_cpp_include_guard
#define kinputfile_cpp_include_guard

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <k09/kutil00.cpp>
#include <k09/kstr00.cpp>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class kinputfile;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */


/* ********** Class Definitions ********** */

class kinputfile {
	
private:
	
	std::ifstream ifs;
	std::string fn;
	
public:
	
	kinputfile( void) : ifs(), fn(){}
	kinputfile( const std::string &);
	~kinputfile( void);
	
	void setFileName( const std::string &);
	bool open( void);
	void close( void);
	bool readFile( std::string &);
	bool readFileAndSeparateLines( std::vector <std::string> &);
	bool readLine( std::string &);
	void readAllLines( std::vector <std::string> &);
	void getSeparatedStrings( std::vector < std::vector <std::string> > &, const std::string &);
	
//		char ***getSeparatedStrings( const char *sep, int *nlinep, int **ncolpp);
		
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */


/* ********** Definitions of Member Functions ********** */

kinputfile :: kinputfile( const std::string &n)
{
	
	setFileName( n);
	open();
	
}

kinputfile :: ~kinputfile( void)
{
	
	close();
	
}

void kinputfile :: setFileName( const std::string &n)
{
	
	fn.assign( n);
	
}

// 正常終了でtrue、異常終了でfalseを返す。
bool kinputfile :: open( void)
{
	
	// このオブジェクトで前に別のファイルをopenしたことがある場合に備えて、clear()する。
	ifs.clear();
	
	// バイナリモードで開かないと、UNIX形式のファイルをうまく扱えない。
	ifs.open( fn.c_str(), std::ios_base :: in | std::ios_base :: binary);
	
	if ( ifs.fail() == true){
		alert( "kinputfile :: open()");
		return false;
	}
	
	return true;
	
}

void kinputfile :: close( void)
{
	
	ifs.close();
	
}

// ファイルを全部読んでretに入れる。
// ファイルが読めない状態だったらfalseを返す。
bool kinputfile :: readFile( std::string &ret)
{
	
	std::stringstream ss;
	
	if ( ifs.good() == false){
		alert( "kinputfile :: readFile()", "File stream is not good.");
		return false;
	}
	
	ret.clear();

	do {
		ifs >> ss.rdbuf();
	} while ( ifs.eof() == false);

	ret.assign( ss.str()) ;

	return true;

}

// ファイルを全部読み込んで、各行をvector <string> retに入れて返す。
bool kinputfile :: readFileAndSeparateLines( std::vector <std::string> &ret)
{
	
	std::string temp;
	bool b;
	
	ret.clear();

	ifs.seekg( 0);

	std::string str;

	b = readFile( str);
	if ( b = false){
		alert( "kinputfile :: readFileAndSeparateLines()", "Cannot read the file.");
		return false;		
	}

	std::string delim;
	delim.assign( "\r\n");

	// erase CR/LF codes in the first and the last part of str
	boost::trim_if( str, boost::is_any_of( delim));

	// split by CR/LF codes
	boost::split( ret, str, boost::is_any_of( delim), boost::algorithm::token_compress_on);

	return true;

}

/*
※この関数は、Windows上では、'\r'のみの改行には反応してくれない。
※一般にはreadFile()を推奨するが、大きなファイルを読む場合にはreadLine()の方が速いかもしれない。
*/
// ファイルから1行読み込んで、渡された文字列オブジェクトretに入れる。
// EOFに達したらfalseを返す。
bool kinputfile :: readLine( std::string &ret)
{

	std::stringstream ss;
	
	if ( ifs.good() == false){
		alert( "kinputfile :: readLine()", "File stream is not good.");
		return false;
	}
	
	ret.clear();

	std::string buf;
	std::getline( ifs, buf);
	
	// erase CR/LF codes in the first and the last part of str
	std::string delim;
	delim.assign( "\r\n");
	boost::trim_if( buf, boost::is_any_of( delim));

	ret.assign( buf);

	return ( !( ifs.eof()));
	
}

// ファイルを全部読み込んで、各行をvector <string> retに入れて返す。
void kinputfile :: readAllLines( std::vector <std::string> &ret)
{
	
	std::string temp;
	bool b;
	
	ret.clear();

	ifs.seekg( 0);

	do {
		b = readLine( temp);
		ret.push_back( temp);
	} while ( b == true);
	
}


// （現在位置以降の）ファイルを読み込んで、
// 文字列sepで各行を区切って、vector <string>にして、
// それをさらにvectorにしたもの（要素は行数だけある）を
// retに入れる。
// 行によって列数がまちまちになっている可能性がある。
void
kinputfile :: 
getSeparatedStrings(
	std::vector < std::vector <std::string> > &ret, const std::string &sep
)
{
	
	std::vector <std::string> lines;
	std::vector <std::string> tempwords;
	
	ret.clear();
	
	readFileAndSeparateLines( lines);
	
	ret.resize( lines.size());
	
	for ( int i = 0; i < lines.size(); i++){
		tokenize( tempwords, lines[ i], sep);
		ret[ i] = tempwords;
	}

}


#endif /* kinputfile_cpp_include_guard */
