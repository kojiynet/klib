
/*
	
	stest01.cpp
	
	pclub01～04から派生。
	
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

	-std:c++17でコンパイルが通るかやってみる。
	gccでも。
	
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

class SvgFile;
class SvgRect;
class SvgLine;
class SvgText;
class SvgGroup;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);

void drawHistogramToSvg(
	const std::string &,
	const std::vector <double> &, const std::vector <double> &,
	const std::vector <int> &,
	bool = false
);

std::vector <double>
getGridPoints( double, double, int = 4, bool = true, bool = true);


/* ********** Class Definitions ********** */

class SvgFile {

private:

	double width;
	double height;
	double viewBoxX1; 
	double viewBoxY1; 
	double viewBoxX2; 
	double viewBoxY2; 

	std::vector <std::string> filecontent;

	// This line should be exactly in the first line in SVG file.
	const std::string headline1   
	 = { R"(<?xml version="1.0" encoding="UTF-8" ?>)"};


public:

	SvgFile() = delete;

	SvgFile( double w0, double h0, double x1, double y1, double x2, double y2)
	 : width( w0), height( h0),
	   viewBoxX1( x1), viewBoxY1( y1), viewBoxX2( x2), viewBoxY2( y2),
	   filecontent()
	{}

	~SvgFile( void){}

	// これは最終的に他のメソッドで代替される予定。
	void addFileContent( const std::string &s0){
		filecontent.push_back( s0);
	}

	// defined later because of dependency problem
	void addRect( const SvgRect &); 
	void addLine( const SvgLine &); 
	void addText( const SvgText &);

/*
	これは優先順位が低い。
	void addGroup( const SvgGroup &);
*/
	std::vector <std::string> getFileContent( void)
	{
		std::vector <std::string> ret;
		ret.reserve( filecontent.size() + 2);
		ret.push_back( headline1);
		stringstream ss;
		ss << R"(<svg width=")" << width << R"(px")" << " " 
		   << R"(height=")" << height << R"(px")" << " " 
		   << R"(viewBox=")" << viewBoxX1 << " " << viewBoxY1 << " " << viewBoxX2 << " " << viewBoxY2 << R"(")" << " " 
		   << R"(xmlns="http://www.w3.org/2000/svg">)";
		ret.push_back( ss.str());
		ret.insert( ret.end(), filecontent.begin(), filecontent.end());
		return ret;
	}

	// ファイルに書き出すメソッド
	// 書き込み用ファイルを開く。

	// 正常終了でtrue、異常終了でfalseを返す。

	// appendのデフォルト値はtrue、overwriteのデフォルト値はfalse、askのデフォルト値はtrue。
	// 同じファイル名のファイルがすでに存在する場合の挙動は以下のとおり。
	// appendがtrueなら、追加書き込みモードで開く。overwriteとaskは無視する。
	// appendがfalseで、overwriteがtrueなら、上書きする。
	// appendがfalseで、overwriteがfalseで、askがtrueなら、上書きするかどうかをコンソールで尋ねる。
	// appendがfalseで、overwriteがfalseで、askがfalseなら、ファイルを開かない。
	bool writeFile( const string &fn0, bool append = false, bool overwrite = false, bool ask = true)
	{

		std::vector <std::string> svglines = getFileContent();
		koutputfile outsvg( fn0);

		bool ret = outsvg.open( append, overwrite, ask);
		if ( ret == false){
			return false;
		}

		ret = outsvg.writeLines( svglines);
		if ( ret == false){
			return false;
		}

		outsvg.close();
		return true;

	}

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgRect {

private:
	
	std::vector <std::string> attrvec;

public:

	SvgRect( double x0, double y0, double w0, double h0)
	 : attrvec()
	{
		std::stringstream ss;
		ss << R"(x=")"      << x0 << R"(")" << " "
		   << R"(y=")"      << y0 << R"(")" << " "
		   << R"(width=")"  << w0 << R"(")" << " " 
		   << R"(height=")" << h0 << R"(")";
		attrvec.push_back( ss.str());
	}

	~SvgRect( void){}

	void addFill( const std::string &s0)
	{
		attrvec.push_back( R"(fill=")" + s0 + R"(")");
	}

	void addStroke( const std::string &s0)
	{
		attrvec.push_back( R"(stroke=")" + s0 + R"(")");
	}

	void addStrokewidth( double v0)
	{
		std::stringstream ss;
		ss << R"(stroke-width=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	void addFillopacity( double v0)
	{
		std::stringstream ss;
		ss << R"(fill-opacity=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	std::string getContent( void) const
	{
		std::string ret = "<rect";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += " />";
		return ret;
	}

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgLine {

private:
	
	std::vector <std::string> attrvec;

public:

	SvgLine( double x1, double y1, double x2, double y2)
	 : attrvec()
	{
		std::stringstream ss;
		ss << R"(x1=")" << x1 << R"(")" << " "
		   << R"(y1=")" << y1 << R"(")" << " "
		   << R"(x2=")" << x2 << R"(")" << " "
		   << R"(y2=")" << y2 << R"(")";
		attrvec.push_back( ss.str());
	}

	~SvgLine( void){}

	void addFill( const std::string &s0)
	{
		attrvec.push_back( R"(fill=")" + s0 + R"(")");
	}

	void addStroke( const std::string &s0)
	{
		attrvec.push_back( R"(stroke=")" + s0 + R"(")");
	}

	void addStrokewidth( double v0)
	{
		std::stringstream ss;
		ss << R"(stroke-width=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	std::string getContent( void) const
	{
		std::string ret = "<line";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += " />";
		return ret;
	}

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgText {

private:
	
	std::string text; 
	std::vector <std::string> attrvec;

public:

	SvgText( double x0, double y0, const string &s0)
	 : text( s0), attrvec()
	{
		std::stringstream ss;
		ss << R"(x=")" << x0 << R"(")" << " "
		   << R"(y=")" << y0 << R"(")";
		attrvec.push_back( ss.str());
	}

	SvgText( double x0, double y0, double v0)
	 : text(), attrvec()
	{
		{
			std::stringstream ss;
			ss << v0;
			text.assign( ss.str());
		}
		{
			std::stringstream ss;
			ss << R"(x=")" << x0 << R"(")" << " "
			   << R"(y=")" << y0 << R"(")";
			attrvec.push_back( ss.str());
		}
	}

	~SvgText( void){}

	void addFontfamily( const std::string &s0)
	{
		attrvec.push_back( R"(font-family=")" + s0 + R"(")");
	}	

	void addFontsize( double v0)
	{
		std::stringstream ss;
		ss << v0;
		attrvec.push_back( R"(font-size=")" + ss.str() + R"(")");
	}	

	void addTextanchor( const std::string &s0)
	{
		attrvec.push_back( R"(text-anchor=")" + s0 + R"(")");
	}	

	void addDominantbaseline( const std::string &s0)
	{
		attrvec.push_back( R"(dominant-baseline=")" + s0 + R"(")");
	}	

	void addRotate( double d0, double x0, double y0)
	{
		std::stringstream ss;
		ss << R"(transform="rotate)" << "(" << d0 << " " << x0 << " " << y0 << ")" << R"(")";
		attrvec.push_back( ss.str());
	}	

	std::string getContent( void) const
	{
		std::string ret = "<text";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += ">";
		ret += text;
		ret += "</text>";
		return ret;
	}

};


/*
以下未実装
class SvgGroup {} ;
*/


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

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
		
		cout << "Specifying missing cases and excluding very big values...";
		ds.specifyValid( 
			"v403",
			[]( double v)->bool{ return ( v < 99999.0 && v < 2500.0);}
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

	vector <int> codes;
	vector <int> counts;
	vector <double> leftvec;
	vector <double> rightvec;
	ft.getVectors( codes, counts);
	ft.getRangeVectors( leftvec, rightvec);

	drawHistogramToSvg( "stest01out01.svg", leftvec, rightvec, counts);
	drawHistogramToSvg( "stest01out02.svg", leftvec, rightvec, counts, true); // アニメバージョン

	return 0;



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
}

void drawHistogramToSvg(
	const std::string &fn,
	const std::vector <double> &leftvec, const std::vector <double> &rightvec,
	const std::vector <int> &counts,
	bool animated /*= false*/
)
{

	using namespace std;

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

		// x座標のみを算出→論理座標系表現から実際の座標系表現を作成。
		double getXActualFromTheoretical( double x0)
		{

			double retx = ( x0 - theoXMin) / theoWidth  * actuWidth  + actuXMin; 
			return retx;

		}

		// y座標のみを算出→論理座標系表現から実際の座標系表現を作成。
		double getYActualFromTheoretical( double y0)
		{
			
			double rety = ( theoYMax - y0) / theoHeight * actuHeight + actuYMin; 
			return rety;

		}

		// 実際の座標系での中点のxを返す。
		double getActualMidX( void)
		{

			return ( actuXMin + actuWidth / 2);

		}
	
		// 実際の座標系での中点のyを返す。
		double getActualMidY( void)
		{

			return ( actuYMin + actuHeight / 2);

		}
	
	};

	
	Cambus cam;


	// SVG領域の大きさと、座標系のある領域の大きさを指定することで、それらしく計算してほしい。
	double svgwidth = 500;
	double svgheight = 500;
	double outermargin = 20;
	double graph_title_fontsize = 20;
	double graph_title_margin = 10; // グラフタイトルの下のマージン
	double axis_title_fontsize = 20;
	double axis_title_margin = 5; // 軸タイトルと軸ラベルの間のマージン
	double axis_label_fontsize = 14;
	double axis_ticklength = 5;

	// グラフ描画領域の座標（左上、右下）
	double graphpane_actuX1 = outermargin + axis_title_fontsize + axis_title_margin + axis_label_fontsize + axis_ticklength; 
	double graphpane_actuY1 = outermargin + graph_title_fontsize + graph_title_margin;
	double graphpane_actuX2 = svgwidth - outermargin; 
	double graphpane_actuY2 = svgheight - ( outermargin + axis_title_fontsize + axis_title_margin + axis_label_fontsize + axis_ticklength);

	double graphwidth = graphpane_actuX2 - graphpane_actuX1;
	double graphheight = graphpane_actuY2 - graphpane_actuY1;

	


	// ちょうどいい間隔のグリッド線の点と、範囲を得る。

	// x軸
	double xminval = leftvec.front();
	double xmaxval = rightvec.back();
	vector <double> xgridpoints = getGridPoints( xminval, xmaxval);
	for ( auto d : xgridpoints){
		cout << d << endl;
	}
	cout << endl;
	// y軸
	double ymaxval = *( max_element( counts.begin(), counts.end()));
	vector <double> ygridpoints = getGridPoints( 0, ymaxval);
	for ( auto d : ygridpoints){
		cout << d << endl;
	}
	cout << endl;
	
	// 描画範囲は、Gridpointsのさらに5%外側にする。
	double theoWidthTemp = xgridpoints.back() - xgridpoints.front();
	double theoXMin = xgridpoints.front() - 0.05 * theoWidthTemp;
	double theoXMax = xgridpoints.back() + 0.05 * theoWidthTemp;
	
	double theoHeightTemp = ygridpoints.back() - ygridpoints.front();
	double theoYMin = ygridpoints.front() - 0.05 * theoHeightTemp;
	double theoYMax = ygridpoints.back() + 0.05 * theoHeightTemp;
	

	// 大きさや位置を変数で指定するようにする。


	// SVGファイル化の開始

	cam.setActual( graphpane_actuX1, graphpane_actuY1, graphpane_actuX2, graphpane_actuY2);
	cam.setTheoretical( theoXMin, theoYMin, theoXMax, theoYMax);

	// このグラフではSVGファイル内で座標変換をするわけではない。
	SvgFile svgf( svgwidth, svgheight, 0, 0, svgwidth, svgheight);

	{
		SvgRect r1( 0, 0, svgwidth, svgheight);
		r1.addFill( "whitesmoke");
		r1.addStroke( "whitesmoke");
		svgf.addRect( r1);
	}

	// 背景の描画開始

	// 背景色だけ塗る。
	{
		SvgRect r2( graphpane_actuX1, graphpane_actuY1, graphwidth, graphheight);
		r2.addFill( "gainsboro");
		r2.addStroke( "gainsboro");
		svgf.addRect( r2);
	}

	// 以下、<g>での一括指定ができるところが多いが、とりあえず無視する。

	// x軸の目盛を示すグリッド線
	for ( auto v : xgridpoints){

		Point theoP1( v, theoYMax); // top
		Point theoP2( v, theoYMin); // bottom 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		SvgLine l1( actuP1.x, actuP1.y, actuP2.x, actuP2.y);
		l1.addStroke( "silver");
		l1.addStrokewidth( 1);
		svgf.addLine( l1);;

	}


	// y軸の目盛を示すグリッド線
	for ( auto v : ygridpoints){

		Point theoP1( theoXMin, v); // left
		Point theoP2( theoXMax, v); // right 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		SvgLine l2( actuP1.x, actuP1.y, actuP2.x, actuP2.y);
		l2.addStroke( "silver");
		l2.addStrokewidth( 1);
		svgf.addLine( l2);;

	}


	// 背景の描画終了

	// メインの情報の描画開始

	// 度数を示すバー。
	// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
	if ( animated == true){
		
		// 以下はアニメ用
		// SVGアニメをパワポに貼っても動かないらしい。

		// <g>で属性一括指定：開始
		{
			stringstream ss;
			ss << "  "
			<< R"(<g)"
			<< " "
			<< R"(stroke=")" << "Gray" << R"(")"
			<< " "
			<< R"(fill=")" << "Gray" << R"(")" 
			<< R"(>)";
			svgf.addFileContent( ss.str());
		}		


		for ( int i = 0; i < counts.size(); i++){

			Point theoP1( leftvec[ i], counts[ i]); // left-top
			Point theoP2( rightvec[ i], 0); // right-bottom 
			Point actuP1 = cam.getActualFromTheoretical( theoP1);
			Point actuP2 = cam.getActualFromTheoretical( theoP2);

			
			stringstream ss;
			
			ss << "    "
			<< R"(<rect)"
			<< " "
			<< R"(x=")" << actuP1.x << R"(")"
			<< " "
			<< R"(y=")" << actuP1.y << R"(")"
			<< " "
			<< R"(width=")" << ( actuP2.x - actuP1.x) << R"(")"
			<< " "
			<< R"(height=")" << ( actuP2.y - actuP1.y) << R"(")"
			<< " "
			<< R"(>)";
			svgf.addFileContent( ss.str());
			
			ss.str( "");
			ss << R"(      <animate attributeName="height" begin="0s" dur="1s" from="0" to=")" << ( actuP2.y - actuP1.y) << R"(" repeatCount="1"/>)";
			svgf.addFileContent( ss.str());

			ss.str( "");
			ss << R"(      <animate attributeName="y" begin="0s" dur="1s" from=")" << actuP2.y << R"(" to=")" << actuP1.y << R"(" repeatCount="1"/>)";
			svgf.addFileContent( ss.str());
			
			svgf.addFileContent( R"(</rect>)");

		}

		// <g>で属性一括指定：終了
		{
			svgf.addFileContent( "  </g>");
		}

	} else {

		for ( int i = 0; i < counts.size(); i++){

			Point theoP1( leftvec[ i], counts[ i]); // left-top
			Point theoP2( rightvec[ i], 0); // right-bottom 
			Point actuP1 = cam.getActualFromTheoretical( theoP1);
			Point actuP2 = cam.getActualFromTheoretical( theoP2);

			SvgRect rect( actuP1.x, actuP1.y,actuP2.x - actuP1.x, actuP2.y - actuP1.y); 
			rect.addFill( "Gray");
			rect.addStroke( "Gray");
			svgf.addRect( rect);

		}

	}

	// メインの情報の描画終了



	// 周辺情報記載の開始


	// x軸の目盛のヒゲ
	for ( auto v : xgridpoints){

		double actuX = cam.getXActualFromTheoretical( v);

		double x1 = actuX;
		double y1 = cam.actuYMax;
		double x2 = actuX;
		double y2 = cam.actuYMax + axis_ticklength; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( "black"); // 一括指定対象
		li.addStrokewidth( 1); // 一括指定対象
		svgf.addLine( li);		

	}


	// TODO: 軸の単位の記載→優先順位は低い。

	// textタグで、IEやWordはdominant-baselineが効かないらしい。
	// （指定してもdominant-baseline="alphabetic"扱いになる。）

	// x軸の目盛のラベル
	for ( auto v : xgridpoints){

		double actuX = cam.getXActualFromTheoretical( v);

		// 描画領域の下端からmarginだけ離す。
		// alphabeticの基線は、このフォントの場合、本当のフォント下端より20%上なので、その分をずらしている。
		double actuY = cam.actuYMax + axis_ticklength + axis_label_fontsize * 0.8;

		// vの桁数はどうなるのか。。 

		SvgText t0( actuX, actuY, v); // vはstringに変換される。
		t0.addFontfamily( "Arial,san-serif");
		t0.addFontsize( axis_label_fontsize);
		t0.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		t0.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
		svgf.addText( t0);		

	}

	// y軸の目盛のヒゲ
	for ( auto v : ygridpoints){

		double actuY = cam.getYActualFromTheoretical( v);

		double x1 = cam.actuXMin;
		double y1 = actuY;
		double x2 = cam.actuXMin - axis_ticklength; 
		double y2 = actuY; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( "black"); // 一括指定対象
		li.addStrokewidth( 1); // 一括指定対象
		svgf.addLine( li);		

	}

	// y軸の目盛ラベル
	/*
	文字列を回転させる方法を探った→svgtest04.svgとsvgtest05.svg
	　svgtest04.svgで2つの方法を試したが、もっとシンプルにしたかった。
	　svgtest05.svgで、transform属性を使えばよいことがわかった。
	*/
	for ( auto v : ygridpoints){

		double actuY = cam.getYActualFromTheoretical( v);

		// alphabetic基線に合わせるために20%ずらしている。
		double actuX = cam.actuXMin - axis_ticklength - axis_label_fontsize * 0.2;
		
		SvgText t0( actuX, actuY, v); // vはstringに変換される。
		t0.addFontfamily( "Arial,san-serif");
		t0.addFontsize( axis_label_fontsize);
		t0.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		t0.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
		t0.addRotate( 270, actuX, actuY); // 回転の中心が各点で異なるので、一括指定できない。
		svgf.addText( t0);				

	}

	// Title 

	// TODO: フォントサイズを自動調節する？

	// グラフタイトル
	string title = "Frequency - restricted to v less than 2500"s; // "<"とかを自動でエスケープしたい。
	{
//		double fontsize = std::floor( cam.actuWidth * 0.7 / title.size() * 2.0); // 描画領域の幅のうち、7割を占めるぐらいのサイズ
//		if ( fontsize >= cam.actuYMin * 0.7){ // 余白の高さの70%より大きいのはダメ
//			fontsize = cam.actuYMin * 0.7;
//		}
		double fontsize = graph_title_fontsize; 
		double margin = 10;

		double actuX = std::round( svgwidth / 2);
		double actuY = outermargin + fontsize * 0.8;

		SvgText t0( actuX, actuY, title);
		t0.addFontfamily( "Arial,san-serif");
		t0.addFontsize( fontsize);
		t0.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		t0.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
		svgf.addText( t0);	

	}


	// x軸タイトルを書く。
	string xaxistitle = "Household Income";
	{

		double fontsize = axis_title_fontsize; 

		double actuX = std::round( cam.getActualMidX());
		double actuY = svgheight - outermargin - fontsize * 0.2; 

		SvgText t0( actuX, actuY, xaxistitle);
		t0.addFontfamily( "Arial,san-serif");
		t0.addFontsize( fontsize);
		t0.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		t0.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
		svgf.addText( t0);	

	}


	// y軸タイトルを書く。
	string yaxistitle = "#Cases";
	{

		double fontsize = axis_title_fontsize; 
		
		double actuX = outermargin + fontsize * 0.8;
		double actuY = std::round( cam.getActualMidY());

		SvgText t0( actuX, actuY, yaxistitle);
		t0.addFontfamily( "Arial,san-serif");
		t0.addFontsize( fontsize);
		t0.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		t0.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
		t0.addRotate( 270, actuX, actuY); // 回転の中心が各点で異なるので、一括指定できない。
		svgf.addText( t0);	

	}

	// 周辺情報記載の終了



	// 枠線を描く。最後にすべき。
	// fillは透過させる。
	{
		SvgRect r( graphpane_actuX1, graphpane_actuY1, graphwidth, graphheight);
		r.addFillopacity( 0);
		r.addStroke( "black");
		r.addStrokewidth( 1);
		svgf.addRect( r);
	}


	svgf.addFileContent( R"(</svg>)");

	string detector = "<!-- " 
	                  u8"\u6587\u5B57\u30B3\u30FC\u30C9\u8B58\u5225\u7528" // 「文字コード識別用」というUTF-8文字列
	                  " -->";
	svgf.addFileContent( detector);

	/*
	vector <string> svglines = svgf.getFileContent();

	koutputfile outsvg( fn);
	outsvg.open( false, false, true);
	outsvg.writeLines( svglines);
	outsvg.close();
	*/
	svgf.writeFile( fn);

}

// [min0, max0]に、いい感じの間隔で点をとる。
// k0個以上で最小の点を返す。
// newminがtrueのとき、得られた間隔に乗る新しいminも返す。
// newmaxがtrueのとき、得られた間隔に乗る新しいmaxも返す。
// 以下のサイトのアルゴリズムに近そう。
// https://imagingsolution.net/program/autocalcgraphstep/
std::vector <double>
getGridPoints( double min0, double max0, int k0 /*= 4*/, bool newmin /*= true*/, bool newmax /*= true*/)
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


/* ********** Definitions of Member Functions ********** */

/* ***** class SvgFile ***** */

void SvgFile :: addRect( const SvgRect &r0)
{
	filecontent.push_back( r0.getContent());
}

void SvgFile :: addLine( const SvgLine &l0)
{
	filecontent.push_back( l0.getContent());
}

void SvgFile :: addText( const SvgText &t0)
{
	filecontent.push_back( t0.getContent());
}

