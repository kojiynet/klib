
/*
	
	pclub03.cpp
	
	pclub01.cppから派生。01は以下のもの。
	@Programming Club, Imaplus, Nov 16, 2019
	Instant Test for kdatasettest00.cpp
	
	Written by Koji Yamamoto
	Copyright (C) 2019-2020 Koji Yamamoto
	
	TODO:　
	度数分布表をつくる。kstatを見て。
	　別に、連続変数用の機能をつける。
	　　start/end, width, bin を指定する方式。
	　　自動で、スタージェスの公式を使う方式。
	　　階級の端点の表を与える方式。
	ヒストグラムを描く。
	SVGにする。
	
*/


/* ********** Preprocessor Directives ********** */

#include <k09/kdataset01.cpp>
#include <k09/kstat02.cpp>
#include <k09/koutputfile00.cpp>
#include <iostream> 
#include <iomanip>
#include <algorithm>

#include <boost/algorithm/string.hpp>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

// [min0, max0]に、いい感じの間隔で点をとる。
// k0個以上で最小の点を返す。
// newminがtrueのとき、得られた間隔に乗る新しいminも返す。
// newmaxがtrueのとき、得られた間隔に乗る新しいmaxも返す。
std::vector <double>
getGridPoints( double min0, double max0, int k0 = 4, bool newmin = true, bool newmax = true)
{

	using namespace std;

	vector <double> ret;
	
	// この数以上の最小の点を返すようにする。
	int minnpoints = k0; 

	// error
	if ( min0 >= max0){ 
		alert( "getGripPoints()");
		return ret;
	}

	double max0ab = abs( max0);
	double min0ab = abs( min0);

	// max0abとmin0abのうち大きい方は何桁？（その値マイナス1）
	double digits_m1 = floor( log10( max( max0ab, min0ab)));

	// 基準となる10のべき乗値
	double base10val = pow( 10.0, digits_m1);

	// 候補となる、intervalの先頭の桁の値
	vector <double> headcands = { 5.0, 2.5, 2.0, 1.0};

	double interval;

	bool loop = true;
	while ( loop){

		for ( auto h : headcands){

			ret.clear();
			interval = base10val * h;

			// setting startpoint; to avoid startpoint being "-0", we do a little trick.
			double startpoint = ceil( min0 / interval);
			if ( startpoint > -1.0 && startpoint < 1.0){
				startpoint = 0.0;
			}
			startpoint *= interval;

			for ( double p = startpoint; p <= max0; p += interval){
				ret.push_back( p);
			}
			if ( ret.size() >= minnpoints){
				loop = false;
				break;
			}

		}

		base10val /= 10.0;

	}

	if ( newmin == true){
		double oldmin = ret.front();
		if ( oldmin == min0){
			// if the first point already obtained is equal to min0
			// do nothing
		} else {
			ret.insert( ret.begin(), oldmin - interval);
		}
	}

	if ( newmax == true){
		double oldmax = ret.back();
		if ( oldmax == max0){
			// if the last point already obtained is equal to max0
			// do nothing
		} else {
			ret.push_back( oldmax + interval);
		}
	}

	return ret;

}

int main( int, char *[])
{
	
	vector <double> dvec;
	vector <double> dvecclean;

	{
		Dataset ds;
		bool b;

		cout << "Reading data...";
		b = ds.readCsvFile( "jhpsmerged_191029_v403.csv");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;

		cout << "Fixing variable types...";
		int nnum, nmis;
		ds.fixVariableType( nnum, nmis);	
		cout << "Done." << endl;
		
		cout << "Getting numeric vector before specifying missing...";
		b = ds.getNumericVectorWithoutMissing( dvec, "v403");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;
		
		cout << "Specifying missing cases...";
		ds.specifyValid( 
			"v403",
			[]( double v)->bool{ return ( v < 99999.0);}
		);
		cout << "Done." << endl;

		cout << "Getting numeric vector excl. missing...";
		b = ds.getNumericVectorWithoutMissing( dvecclean, "v403");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;
		
	}

	cout << endl;
	cout << "***************************************************" << endl;
	cout << "JHPS 2009 Household Income incl. Tax" << endl;
	cout << "Calculated by mean() and median()" << endl;
	cout << "Mean:   " << setprecision( 15) << mean( dvecclean)   << " (Ten Thousand Yen)" << endl;
	cout << "Median: " << median( dvecclean) << " (Ten Thousand Yen)" << endl;
	cout << "***************************************************" << endl;
	cout << "FYI: Mean from \"dirty\" data: " << setprecision( 15) << mean( dvec) << endl;


	// 度数分布表

	cout << endl;
	cout << "Number of unique values: " << countUniqueValues( dvecclean) << endl;
	cout << "FYI Number of unique values in \"dirty\" vector: " << countUniqueValues( dvec) << endl << endl;


	RecodeTable <double, int> rt;
	rt.setAutoTableFromContVar( dvecclean); 

	cout << "RecodeTable:" << endl;
	rt.print( cout, ","); 
	cout << endl;

	FreqType <int, int> ft;
	ft.setFreqFromRecodeTable( dvecclean, rt);

	ft.printPadding( cout);


	// ヒストグラムをつくりたい。
	// SVGの座標系をいじるところから。
	// SVGのviewBoxについて：アスペクト比が違っているとわかりにくい。
	// （強制的に余白がつくられたりするか、強制的に拡大縮小して円が歪んだりする）ので、
	// svgタグのサイズとviewBoxのサイズを合わせたい。

	struct Cambus;
	struct Point;
	
	struct Point {

		double x, y;

		Point( void)
		 : x( std::numeric_limits<double>::quiet_NaN()),
		   y( std::numeric_limits<double>::quiet_NaN())
		{}
		
		Point( double x0, double y0) : x( x0), y( y0)
		{}

	};

	struct Cambus {

		// 実際の座標系では、y座標は大きいほど「下」の位置を示す。
		// 論理座標系では、y座標は大きいほど「上」の位置を示す。

		double actuXMin, actuYMin, actuXMax, actuYMax; // 実際の座標系での、枠の範囲
		double actuWidth, actuHeight; // 同上

		double theoXMin, theoYMin, theoXMax, theoYMax; // 論理座標系での、枠の範囲
		double theoWidth, theoHeight; // 同上

		void setTheoretical( double xmin0, double ymin0, double xmax0, double ymax0)
		{
			theoXMin = xmin0; theoYMin = ymin0; theoXMax = xmax0; theoYMax = ymax0; 
			theoWidth = xmax0 - xmin0; theoHeight = ymax0 - ymin0; 
		}

		void setActual( double xmin0, double ymin0, double xmax0, double ymax0)
		{
			actuXMin = xmin0; actuYMin = ymin0; actuXMax = xmax0; actuYMax = ymax0; 
			actuWidth = xmax0 - xmin0; actuHeight = ymax0 - ymin0; 
		}

		// 論理座標系表現から実際の座標系表現を作成。
		Point getActualFromTheoretical( const Point &poi0)
		{
			
			double x0 = poi0.x;
			double y0 = poi0.y;
			Point ret;
			ret.x = ( x0 - theoXMin) / theoWidth  * actuWidth  + actuXMin; 
			ret.y = ( theoYMax - y0) / theoHeight * actuHeight + actuYMin; 
			return ret;

		}

		// 実際の座標系での中点のxを返す。
		double getActualMidX( void)
		{

			return ( actuXMin + actuWidth / 2);

		}
	
	};

	vector <string> svglines;

	vector <int> codes;
	vector <int> counts;
	vector <double> leftvec;
	vector <double> rightvec;
	ft.getVectors( codes, counts);
	ft.getRangeVectors( leftvec, rightvec);
	Cambus cam;

	double minval = leftvec.front();
	double maxval = rightvec.back();

	// SVG領域の大きさと、座標系のある領域の大きさを指定することで、それらしく計算してほしい。

	// ちょうどいい間隔のグリッド線の点と、範囲を得る。
	// TODO: 範囲を、5%ぐらい外側にしたい。描画と枠線が重なる場合がありうるので。
	vector <double> gridpoints = getGridPoints( minval, maxval);
	for ( auto d : gridpoints){
		cout << d << endl;
	}
	cout << endl;



	// SVGファイル化の開始

	cam.setActual( 50, 50, 450, 450);
	cam.setTheoretical( gridpoints.front(), 0, gridpoints.back(), 1200);

	svglines.push_back( R"(<?xml version="1.0" encoding="UTF-8" ?>)"); // This should be exactly in the first line.
	svglines.push_back( R"(<svg width="500px" height="500px" viewBox="0 0 500 500" xmlns="http://www.w3.org/2000/svg">)");


	// 背景の描画開始

	// 背景色だけ塗る。
	svglines.push_back( R"(  <rect x="50" y="50" width="400" height="400" fill="Whitesmoke" />)");

	// strokeとかの属性は、<g>で一括指定できる？

	// x軸の目盛を示すグリッド線
	for ( auto v : gridpoints){

		Point theoP1( v, 1200); // left-top
		Point theoP2( v, 0); // right-bottom 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		stringstream ss;
		ss << "  "
		<< R"(<line)"
		<< " "
		<< R"(x1=")" << actuP1.x << R"(")"
		<< " "
		<< R"(y1=")" << actuP1.y << R"(")"
		<< " "
		<< R"(x2=")" << actuP2.x << R"(")"
		<< " "
		<< R"(y2=")" << actuP2.y << R"(")"
		<< " "
		<< R"(stroke=")" << "Lightgray" << R"(")"
		<< " "
		<< R"(stroke-width=")" << 1 << R"(")"
		<< " "
		<< R"(/>)";

		svglines.push_back( ss.str());

	}


	// 同様に、y軸も。。
	

	// 背景の描画終了


	// 度数を示すバー。
	// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
	for ( int i = 0; i < codes.size(); i++){

		Point theoP1( leftvec[ i], counts[ i]); // left-top
		Point theoP2( rightvec[ i], 0); // right-bottom 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		stringstream ss;
		ss << "  "s
		<< R"(<rect)"s // tried to use string literal
		<< " "
		<< R"(x=")" << actuP1.x << R"(")"
		<< " "
	    << R"(y=")" << actuP1.y << R"(")"
		<< " "
		<< R"(width=")" << ( actuP2.x - actuP1.x) << R"(")"
		<< " "
		<< R"(height=")" << ( actuP2.y - actuP1.y) << R"(")"
		<< " "
		<< R"(stroke=")" << "Gray" << R"(")"
		<< " "
		<< R"(fill=")" << "Gray" << R"(")" 
		<< " "
		<< R"(/>)";
		
		svglines.push_back( ss.str());

	}


	// 周辺情報記載の開始

	// x軸の目盛のラベルとヒゲ
	for ( auto v : gridpoints){

		Point theoP( v, 0); // 本当はy軸の座標は要らないのだが。。

		Point actuP = cam.getActualFromTheoretical( theoP);

		double fontsize = 20; // とりあえずの値。
		double ticklabelmargin = 10; // とりあえずの値。
		double tickheight = 5; // とりあえずの値。
		
		stringstream ss;

		ss << "  "
		<< R"(<text)"
		<< " "
		<< R"(x=")" << actuP.x << R"(")" // 中央揃えをする前提で座標を指定。
		<< " "
		<< R"(y=")" << ( std::round( cam.actuYMax + ticklabelmargin)) << R"(")" // 描画領域の下端からmarginだけ離す。
		<< " "
		<< R"(font-family=")" << "Arial,san-serif" << R"(")"
		<< " "
		<< R"(font-size=")" << fontsize << R"(")" 
		<< " "
		<< R"(text-anchor="middle")"
		<< " "
		<< R"(dominant-baseline="text-before-edge")"
		<< ">"
		<< v // 桁数はどうなるのか。。 
		<< R"(</text>)";

		svglines.push_back( ss.str());

		// 目盛ラベルの上の「ヒゲ」をつける。

		ss.str( "");

		ss << "  "
		<< R"(<line)"
		<< " "
		<< R"(x1=")" << actuP.x << R"(")"
		<< " "
		<< R"(y1=")" << cam.actuYMax << R"(")"
		<< " "
		<< R"(x2=")" << actuP.x << R"(")"
		<< " "
		<< R"(y2=")" << ( cam.actuYMax + tickheight) << R"(")"
		<< " "
		<< R"(stroke=")" << "Black" << R"(")"
		<< " "
		<< R"(stroke-width=")" << 1 << R"(")"
		<< " "
		<< R"(/>)";

		svglines.push_back( ss.str());

	}





/*
	// ちょうどいい間隔と基準点の実験。
	{
		vector <double> gridpoints = getGridPoints( -12.34, 567.8);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}
	
	{
		vector <double> gridpoints = getGridPoints( -12.34, 567.8, 4, false, false);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}
	
	{
		vector <double> gridpoints = getGridPoints( -1234.5, 567.8);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( -1234.5, 567.8, 5);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( 123.5, 5678.9);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( -80001.0, -299.9);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( -80001.0, -299.9, 5);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}
*/


	// Title 

	string title = "Frequency from pclub03.cpp"s;

	{
		stringstream ss;
		double fontsize = std::floor( cam.actuWidth * 0.7 / title.size() * 2.0); // 描画領域の幅のうち、7割を占めるぐらいのサイズ
		if ( fontsize >= cam.actuYMin * 0.7){ // 余白の70%より大きいのはダメ
			fontsize = cam.actuYMin * 0.7;
		}
		ss << "  "
		<< R"(<text)"
		<< " "
		<< R"(x=")" << ( std::round( cam.getActualMidX())) << R"(")" // 中央揃えをするので。
		<< " "
		<< R"(y=")" << ( std::round( cam.actuYMin * 0.9)) << R"(")" // 余白のうち10%浮かせる。
		<< " "
		<< R"(font-family=")" << "Arial,san-serif" << R"(")"
		<< " "
		<< R"(font-size=")" << fontsize << R"(")" 
		<< " "
		<< R"(text-anchor="middle")"
		<< " "
		<< R"(dominant-baseline="text-after-edge")"
		<< ">"
		<< title 
		<< R"(</text>)";

		svglines.push_back( ss.str());
	}


	// x軸タイトルを書く。

	string axislabel = "Household Income";

	{

		double fontsize = 20; // とりあえずの値。
		double axislabelmargin = 30; // とりあえずの値。
		
		stringstream ss;

		ss << "  "
		<< R"(<text)"
		<< " "
		<< R"(x=")" << ( std::round( cam.getActualMidX())) << R"(")" // 中央揃えをするので。
		<< " "
		<< R"(y=")" << ( std::round( cam.actuYMax + axislabelmargin)) << R"(")" // 描画領域の下端からmarginだけ離す。
		<< " "
		<< R"(font-family=")" << "Arial,san-serif" << R"(")"
		<< " "
		<< R"(font-size=")" << fontsize << R"(")" 
		<< " "
		<< R"(text-anchor="middle")"
		<< " "
		<< R"(dominant-baseline="text-before-edge")"
		<< ">"
		<< axislabel 
		<< R"(</text>)";

		svglines.push_back( ss.str());

	}


	// 周辺情報記載の終了


	// 枠線を描く。最後にすべき。
	// fillは透過させる。
	svglines.push_back( R"(  <rect x="50" y="50" width="400" height="400" fill-opacity="0" stroke="Black" stroke-width="1" />)");


	svglines.push_back( R"(</svg>)");

	string detector = "<!-- " 
	                  u8"\u6587\u5B57\u30B3\u30FC\u30C9\u8B58\u5225\u7528" // 「文字コード識別用」というUTF-8文字列
	                  " -->";
	svglines.push_back( detector);


	koutputfile outsvg( "pclub03out.svg");
	outsvg.open( false, false, true);
	outsvg.writeLines( svglines);
	outsvg.close();


	// 今のRecodeTableには、左端・右端がない（無限大）という指定ができない。

	// FreqTypeにはすごく小さい機能だけを持たせることにして、
	// 別にFreqTableTypeか何かをつくって、そこに、RecodeTableを持たせたり、
	// それをもとにしたFreqを作らせたりしてもよいかも。

	/*
	度数分布表をつくる。kstatを見て。
	　別に、連続変数用の機能をつける。
	　　start/end, width, bin を指定する方式。
	　　自動で、スタージェスの公式を使う方式？
	　　※Stataでは、min{ sqrt(N), 10*ln(N)/ln(10)}らしいので、それでいく。
	　　階級の端点の表を与える方式。
	*/

	return 0;

}


/* ********** Definitions of Member Functions ********** */

