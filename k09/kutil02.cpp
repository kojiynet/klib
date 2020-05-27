
/*
	kutil
	Ver. k09.02
	
	Written by Koji Yamamoto
	Copyright (C) Koji Yamamoto 2013-2020
	In using this, please read the document which states terms of use.
	
	Tiny Utilities
	
	When functions dealing with characters,
	they are applicable only to ASCII characters.

    ktimerOldは非推奨。
    k10にするときに削除する。
	
	TODO:
	clArgsの名称をkargsにする？そしてファイルを独立させる？
	ktimerも独立させる？
	
	C++11標準に直していく？
	→マクロ __cplusplus が 201103L とかより大きい、で判別？
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kutil_cpp_include_guard
#define kutil_cpp_include_guard

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <chrono>

#include <ctime>
#include <cstdlib>
#include <cmath>
#include <cctype>

#include <k09/kexcept00.cpp>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Type Declarations: enum, class, etc. ********** */

enum compilerId {
	MSC = 1,
	GCC = 2,
	DMC = 3,
	BCC = 4,
	OTHERS = 5
};

class Timer;
class ktimerOld;
class clArgs;


/* ********** Function Declarations ********** */

void setAlertOff( void);
void setAlertOn( void);
void setAlertExceptOn( void);
void setAlertExceptOff( void);
void alert( void);
void alert( const std::string &);
void alert( const std::string &, const std::string &);
void alertExit( const std::string &);
void alertExit( const std::string &, const std::string &);
bool alertYesNo( const std::string &, const std::string &);

void intToString( std::string &, int, int);
void doubleToString( std::string &, double, int = 6, int = 4);
double stringToDouble( const std::string &);
int roundToInt( double);

bool virtuallyEqual( double, double, double = 1e-5);
bool isNaN( const double &);

compilerId getCompilerId( void);
void getCompilerName( std::string &);


/* ********** Type Definitions: enum, class, etc. ********** */

class Timer {

private:
    
	using time_point = std::chrono::system_clock::time_point;

    time_point startt;
    time_point endt;
    bool endtValid;
    
public:

    Timer( void);
    ~Timer( void);
    
    void restart( void);
    void markEnd( void);
    long int getInterval( void);
    
};

// 非推奨。k10では削除。
class ktimerOld {
	
	private:
		
		time_t startt, endt;
		
	public:
		
		inline ktimerOld( void);
		inline ~ktimerOld( void);
		
		inline void start( void);
		inline void stop( void);
		inline long int getInterval( void);
		
		inline static void getActualTime( std::string &);
		
};

// Command Line Arguments
class clArgs {
	
	private:
		
		std::vector <char> keys; // assigned '\0' for no-key argument
		std::vector <std::string> data;
		
		static char nokey; // assigned '\0' to indicate "no-key" status
	
	public:
		
		clArgs( void);
		~clArgs( void);
		
		void initialize( int, char **);
		int countAllArgs( void) const;
		bool argExistsCns( char) const;
		bool argWithoutKeyExists( void) const;
		int countArgsCns( char) const;
		int countArgsWithoutKey( void) const;
		void getStringsCns( std::vector <std::string> &, char) const; // 「入れ物となる引数を先に書く」の原則から、追加された。
		void getStringsWithoutKey( std::vector <std::string> &) const;
		
};


/* ********** Global Variables ********** */

bool alertMode = true;
bool alertExcept = false;


/* ********** Definitions of Static Member Variables ********** */

char clArgs :: nokey = '\0'; // dummy value for "no-key".


/* ********** Function Definitions ********** */

/*
	alert系の出力を抑制する。
	ただし、alertYesNoのメッセージ出力と入力の回収は行なう。
*/
void setAlertOff( void)
{
	
	alertMode = false;
	
}

/*
	alert系の出力の抑制を解除する。
*/
void setAlertOn( void)
{
	
	alertMode = true;
	
}

/*
	alert系の関数が呼ばれた際に、例外を送出する。
	ただし、alertYesNoのメッセージ出力と入力の回収は行なう。
*/
void setAlertExceptOn( void)
{
	
	alertExcept = true;
	
}

/*
	alert系の関数の呼び出し時に例外を送出しない。
*/
void setAlertExceptOff( void)
{
	
	alertExcept = false;
	
}

/*
	"Alert."と表示する。
*/
void alert( void)
{
	
	using namespace std;

	if ( alertMode == true){
		if ( alertExcept == false){
			cerr << "Alert." << endl << endl;
		} else {
			throwMsgExcept( "unknown");
		}
	}
	
}

/*
	from（関数名など）からのものとして、アラートを表示する。
*/
void alert( const std::string &from)
{
	
	using namespace std;

	if ( alertMode == true){
		if ( alertExcept == false){
			cerr << "Alert from: " << from << endl << endl;
		} else {
			throwMsgExcept( from);
		}
	}
	
}

/*
	from（関数名など）からのメッセージmsgを伴うアラートを表示する。
*/
void alert( const std::string &from, const std::string &msg)
{

	using namespace std;
	
	if ( alertMode == true){
		if ( alertExcept == false){
			cerr << "Alert from: " << from << endl
			     << "Message: " << msg << endl << endl;
		} else {
			throwMsgExcept( from, msg);
		}
	}
	
}

/*
	from（関数名など）からのアラートを表示し、EXIT_FAILUREを返しながらプログラムを終了する。
*/
void alertExit( const std::string &from)
{
	
	using namespace std;

	if ( alertMode == true){
		if ( alertExcept == false){
			cerr << "Alert from: " << from << endl
			     << "The program will be terminated due to error." << endl << endl;
		} else {
			throwMsgExcept( from);
		}
	}
	
	exit( EXIT_FAILURE);
	
}

/*
	from（関数名など）からのメッセージmsgを伴うアラートを表示し、
	EXIT_FAILUREを返しながらプログラムを終了する。
*/
void alertExit( const std::string &from, const std::string &msg)
{
	
	using namespace std;

	if ( alertMode == true){
		if ( alertExcept == false){
			cerr << "Alert from: " << from << endl
			     << "Message: " << msg << endl
			     << "The program will be terminated due to error." << endl << endl;
		} else {
			throwMsgExcept( from, msg);
		}
	}
	
	exit( EXIT_FAILURE);
	
}

/*
	from（関数名など）からのメッセージmsgを伴うアラートを表示し、
	Yes/Noの入力を求め、Yesならtrueを返す。
*/
bool alertYesNo( const std::string &from, const std::string &msg)
{
	
	using namespace std;

	string inp;
	char c;
	
	if ( alertMode == true){
		cerr << "Alert from: " << from << endl;
	}
	
	cerr << msg << endl;
	
	while ( true) {
		cerr << "Input Y or N > ";
		cin >> inp;
		cerr << endl;
		
		if ( inp.length() == 1){
			c = inp[ 0];
			if ( c == 'Y' || c == 'y'){
				return true;
			} else if ( c == 'N' || c == 'n'){
				break;
			}
		}
	}
	
	return false;
	
}

/*
	vを、最低k桁になるように0で満たして文字列にして、retに格納する。
	k == -1のときには、fillingしない。
	intToString( s, 1, 4)　→　s == "0001"
*/
void intToString( std::string &ret, int v, int k)
{
	
	using namespace std;

	ostringstream oss;
	
	oss.str( "");
	oss.clear();
	if ( k == -1){
		oss << v;
	} else {
		oss << setfill( '0') << setw( k) << v;
	}
	
	ret.assign( oss.str());
	
}

/*
	vをstringに変換して、retに格納する。
	k桁（以上）の精度で表現する。
	指数形式にした場合に指数の絶対値がel以下なら、
	指数形式ではなく固定小数点形式で表現する。
	初期値としてk=6、el=4が与えられる。
*/
void doubleToString( std::string &ret, double v, int k, int el)
{
	
	using namespace std;

	int e10;
	ostringstream oss;
	
	oss.str( "");
	oss.clear();
	
	e10 = 0;
	if ( v > 0)
		e10 = floor( log10( v));
	else if ( v < 0)
		e10 = floor( log10( -v));
	
	if ( abs( e10) <= el){
		// fixedの場合はsetprecisionは小数点とそれ以下の表示桁数を指すらしいので、
		// e10が正でも負でも下の計算が機能する。
		k -= e10 + 1;
		if ( k < 0)
			k = 0;
		oss << setprecision( k);
		oss << fixed;
	} else {
		oss << setprecision( k - 1);
		oss << scientific;
	}
	
	oss << v;
	ret.assign( oss.str());
	
}

/*
	文字列sをdoubleに変換する。
	数値で始まらない場合と、数値で始まるが無意味な文字列が続く場合は、quiet_NaNを返す。
*/
double stringToDouble( const std::string &s)
{
	
	using namespace std;

	double ret;
	
	try {
		
		// stod() may throw exception 
		ret = stod( s);

	} catch( const exception &e) {
		
		if ( typeid( e) == typeid( invalid_argument) || typeid( e) == typeid( out_of_range) ){
			ret = numeric_limits <double> :: quiet_NaN();
		} else {
			throw e;
		}

	}

	return ret;
	
}

/*
	xに最も近い整数を返す。四捨五入のようなもの。
	ただし、xが小数の場合、「四捨五入らしくない」挙動をする。
	（-34.5は-34に丸められる）
	C++11にはround関数があるらしい。
*/
int roundToInt( double x)
{
	
	int ret;
	double diff;
	
	ret = ( int)floor( x);
	diff = x - ( double)ret;
	
	if ( diff < 0.5)
		return ( int)ret;
	
	return ( ( int)ret + 1);
	
}

/*
	aとbがほぼ等しいと言えるのかを、相対誤差をepsrとして調べる。
	epsrはaとbのうち絶対値の大きい方の値に対しての誤差。
	epsrの初期値として1e-5が与えられている。
*/
bool virtuallyEqual( double a, double b, double epsr)
{
	
	using namespace std;

	double absa, absb;
	double diff;
	double eps;
	bool ret;
	
	if ( isNaN( a) || isNaN( b) || isNaN( epsr) || epsr > 1.0 || epsr <= 0){
		alert( "virtuallyEqual()");
		return false;
	}
	
	absa = fabs( a);
	absb = fabs( b);
	eps = max( absa, absb) * epsr;
	diff = fabs( a - b);
	
	if ( eps == 0.0)
		ret = true; // Both of a and b are zero.
	else if ( diff < eps)
		ret = true;
	else
		ret = false;
	
	return ret;
	
}

/*
	xがNaNかどうかを返す。
	※C++11の<cmath>にはNaN処理関数とかがあるらしい。
*/
bool isNaN( const double &x)
{
	
	using namespace std;

	int dbyte;
	char *ptr1, *ptr2;
	double qnan;
	bool ret;
	
	if ( numeric_limits<double> :: has_quiet_NaN == false){
		alert( "isNaN()", "There does not exist double NaN.");
		return false;
	}
	
	dbyte = sizeof( double);
	
	qnan = numeric_limits<double> :: quiet_NaN();
	ptr1 = (char *)(&qnan);
	ptr2 = (char *)(&x);
	
	ret = true;
	for ( int i = 0; i < dbyte; i++){
		if ( ptr1[ i] != ptr2[ i]){
			ret = false;
			break;
		}
	}
	
	return ret;
	
}

/*
	実行コードのコンパイルに用いられたコンパイラ名のコードを返す。
	数値はenumで定義されている。
*/
compilerId getCompilerId( void)
{
	
	compilerId ret;
	int msc, bcc, dmc, gcc;
	
	msc = 0;
	bcc = 0;
	dmc = 0;
	gcc = 0;
	
#ifdef _MSC_VER
	msc = 1;
#endif
	
#ifdef __GNUC__
	gcc = 1;
#endif
	
#ifdef __DMC__
	dmc = 1;
#endif
	
#ifdef __BORLANDC__
	bcc = 1;
#endif
	
	if ( msc + bcc + dmc + gcc > 1){
		alert( "getCompilerId()", "We can not identify the compiler type.");
		ret = OTHERS;
	} else {
		if ( msc == 1){
			ret = MSC;
		} else if ( gcc == 1){
			ret = GCC;
		} else if ( dmc == 1){
			ret = DMC;
		} else if ( bcc == 1){
			ret = BCC;
		} else 
			ret = OTHERS;
		}
	
	return ret;
	
}

/*
	コンパイラ名をretに入れる。
	具体的には、{ "MSC", "BCC", "DMC", "GCC", "OTHERS"}のいずれかを返す。
*/
void getCompilerName( std::string &ret)
{
	
	compilerId ci;
	
	ci = getCompilerId();
	switch ( ci){
		case MSC:
			ret.assign( "MSC");
			return;
		case GCC:
			ret.assign( "GCC");
			return;
		case DMC:
			ret.assign( "DMC");
			return;
		case BCC:
			ret.assign( "BCC");
			return;
		case OTHERS:
			ret.assign( "OTHERS");
			return;
	}
	
	ret.assign( "");
	
}


/* ********** Definitions of Member Functions ********** */


/* ***** Member Functions of class "Timer" ***** */

// Automatically get the current time point and store it in startt
Timer :: Timer( void)
: startt( std::chrono::system_clock::now()),
  endt(), endtValid( false)
{}

Timer :: ~Timer( void)
{}

// reset the start time point
void Timer :: restart( void)
{
    startt = std::chrono::system_clock::now();
    endtValid = false;
}

// gets the current time point and stores it in endt
void Timer :: markEnd( void)
{
    endt = std::chrono::system_clock::now();
    endtValid = true;
}

// returns the interval by milliseconds 
// returns -1 if endt is not marked yet
long int Timer :: getInterval( void)
{

    using namespace std::chrono;

    if ( endtValid == false){
        return -1;
    }

    auto millisec = duration_cast <milliseconds> ( endt - startt);

    return millisec.count();

}


/* ***** Member Functions of class "ktimerOld" ***** */

ktimerOld :: ktimerOld( void)
{
	
	startt = -1;
	endt = -1;
	
}

ktimerOld :: ~ktimerOld( void)
{
	
}

/*
	時間の計測を開始する。
*/
inline void ktimerOld :: start( void)
{
	
	time( &startt);
	
}

/*
	時間の計測を終了する。
*/
inline void ktimerOld :: stop( void)
{
	
	time( &endt);
	
}

/*
	秒単位で経過時間を返す。エラーコードは-1。
	C++11ではstd::chronoで細かく測れるらしい。
*/
inline long int ktimerOld :: getInterval( void)
{
	
	if ( startt < 0 || endt < 0 || startt > endt)
		return -1;
	else
		return ( endt - startt);
	
}

/*
	現在時刻を文字列でretに入れて返す。
*/
inline void ktimerOld :: getActualTime( std::string &ret)
{
	
	time_t tt;
	tm *tp;
	
	time( &tt);
	tp = localtime( &tt);
	ret.assign( asctime( tp));
	
}

/* ***** Member Functions of class "clArgs" ***** */

clArgs :: clArgs( void)
{
	
	keys.clear();
	data.clear();
	
}

clArgs :: ~clArgs( void)
{
	
	keys.clear();
	data.clear();
	
}

/*
	main()の引数であるargcとargvをそのまま受け取って、keyとdataに格納する。
	"-Xyyy"か"/Xyyy"の形式のオプションであれば、'X'をkey、"yyy"をdataとする。
	それ以外の引数については、keyなしの引数として、すべての文字列をdataとし、
	これに対応するkeyにはnokey ('\0')が入る。
	渡されるav[ 0]は実行ファイル名であると見なして無視する。
	avにconst修飾をすると、char *[]が渡せず、コンパイルエラーになる。
*/
void clArgs :: initialize( int ac, char **av)
{
	
	keys.clear();
	data.clear();
	
	for ( int i = 1; i < ac; i++){
		
		if ( av[ i][ 0] != '-' && av[ i][ 0] != '/'){ // keyなしの引数の場合
			
			keys.push_back( nokey);
			data.push_back( av[ i]);
			
		} else {
			
			keys.push_back( av[ i][ 1]); // 2文字目をkeyとする
			data.push_back( av[ i] + 2); // 3文字目以降をdataとする
			
		}
		
	}
	
}

/*
	引数の数を返す。(argc - 1)と同じ値になるはず。
*/
int clArgs :: countAllArgs( void) const
{
	
	return data.size();
	
}

/*
	keyに該当するキーを持つオプションが存在するかどうかを返す。
	keyが大文字であるか小文字であるかは区別しない。
	Cnsは"Case Non-Sensitive"の意味
*/
bool clArgs :: argExistsCns( char key) const
{
	
	char ukey, lkey;
	int n;
	
	ukey = ( char)( toupper( key));
	lkey = ( char)( tolower( key));
	n = keys.size();
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == lkey || keys[ i] == ukey){
			return true;
		}
	}
	
	return false;
	
}

/*
	keyを持たないオプションが存在するかどうかを返す。
*/
bool clArgs :: argWithoutKeyExists( void) const
{
	
	int n;
	
	n = keys.size();
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == nokey){
			return true;
		}
	}
	
	return false;
	
}

/*
	keyに該当するキーを持つオプションがいくつ存在するかを返す。
	keyが大文字であるか小文字であるかは区別しない。
	Cnsは"Case Non-Sensitive"の意味
*/
int clArgs :: countArgsCns( char key) const
{
	
	char ukey, lkey;
	int n, ret;
	
	ukey = ( char)( toupper( key));
	lkey = ( char)( tolower( key));
	n = keys.size();
	ret = 0;
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == lkey || keys[ i] == ukey){
			ret++;
		}
	}
	
	return ret;
	
}

/*
	keyを持たないオプションがいくつ存在するかを返す。
*/
int clArgs :: countArgsWithoutKey( void) const
{
	
	int n, ret;
	
	n = keys.size();
	ret = 0;
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == nokey){
			ret++;
		}
	}
	
	return ret;
	
}

/*
	keyに該当するキーを持つオプションに付随する文字列の集合をretに入れる。
	keyが大文字であるか小文字であるかは区別しない。
	Cnsは"Case Non-Sensitive"の意味
*/
void clArgs :: getStringsCns( std::vector <std::string> &ret, char key) const
{
	
	char ukey, lkey;
	int n;
	
	ukey = ( char)( toupper( key));
	lkey = ( char)( tolower( key));
	n = keys.size();
	ret.clear();
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == lkey || keys[ i] == ukey){
			ret.push_back( data[ i]);
		}
	}
	
}

/*
	keyを持たないオプションに付随する文字列の集合をretに入れる。
*/
void clArgs :: getStringsWithoutKey( std::vector <std::string> &ret) const
{
	
	int n;
	
	n = keys.size();
	ret.clear();
	
	for ( int i = 0; i < n; i++){
		if ( keys[ i] == nokey){
			ret.push_back( data[ i]);
		}
	}
	
}

#endif /* kutil_cpp_include_guard */
