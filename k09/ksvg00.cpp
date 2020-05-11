
/*
	
	ksvg00.cpp
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto
	
*/


/* ********** Preprocessor Directives ********** */

#include <vector>
#include <string>
#include <sstream>

#include <k09/koutputfile00.cpp>


/* ********** Namespace Declarations/Directives ********** */


/* ********** Class Declarations ********** */

struct Cambus;
struct Point;

class SvgFile;

class SvgAddable;
class SvgBasicShape;
class SvgRect;
class SvgCircle;
class SvgEllipse; 
class SvgLine;
class SvgText;

class SvgGraph;
class GraphAddable;
class GraphBasicShape;
class GraphRect;
class GraphLine;

class GraphRectAnimator;
class BuildupGraphRectAnimator;

class SvgHistogramMaker;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

std::vector <double>
getGridPoints( double, double, int = 4, bool = true, bool = true);


/* ********** Class Definitions ********** */

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
	Point getActualFromTheoretical( const Point &poi0) const
	{
		
		double x0 = poi0.x;
		double y0 = poi0.y;
		Point ret;
		ret.x = ( x0 - theoXMin) / theoWidth  * actuWidth  + actuXMin; 
		ret.y = ( theoYMax - y0) / theoHeight * actuHeight + actuYMin; 
		return ret;

	}

	// x座標のみを算出→論理座標系表現から実際の座標系表現を作成。
	double getXActualFromTheoretical( double x0) const
	{

		double retx = ( x0 - theoXMin) / theoWidth  * actuWidth  + actuXMin; 
		return retx;

	}

	// y座標のみを算出→論理座標系表現から実際の座標系表現を作成。
	double getYActualFromTheoretical( double y0) const
	{
		
		double rety = ( theoYMax - y0) / theoHeight * actuHeight + actuYMin; 
		return rety;

	}

	// 実際の座標系での中点のxを返す。
	double getActualMidX( void) const
	{

		return ( actuXMin + actuWidth / 2);

	}

	// 実際の座標系での中点のyを返す。
	double getActualMidY( void) const
	{

		return ( actuYMin + actuHeight / 2);

	}

};

class SvgFile {

private:

	double width;
	double height;
	double viewBoxX1; 
	double viewBoxY1; 
	double viewBoxX2; 
	double viewBoxY2; 

	std::vector <std::string> filecontent;

public:

	SvgFile() = delete;
	SvgFile( double w0, double h0, double x1, double y1, double x2, double y2);
	~SvgFile( void);

	void addFileContent( const std::string &s0);
	void addElement( const SvgAddable &);

	std::vector <std::string> getFileContent( void);

	bool writeFile(
		const std::string &fn0,
		bool append    = false,
		bool overwrite = false,
		bool ask       = true
	);

};

// interface for addable svg element classes
class SvgAddable {
public:
	virtual std::string getContent( void) const = 0;
};

// Base class for Rect, Circle, Ellipse, Line
class SvgBasicShape {

protected:

	std::vector <std::string> attrvec;

public:

	SvgBasicShape( void);
	virtual ~SvgBasicShape( void);

	virtual void addFill( const std::string &s0);
	virtual void addStroke( const std::string &s0);
	virtual void addStrokewidth( double v0);
	virtual void addFillopacity( double v0);

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgRect : public SvgBasicShape, public SvgAddable {

private:
	
	std::vector <std::string> animatevec;

public:

	SvgRect( double x0, double y0, double w0, double h0);
	~SvgRect( void);

	void addAnimate( const std::string &attr, double v1, double v2, double sec);

	std::string getContent( void) const;

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgCircle : public SvgBasicShape, public SvgAddable {

private:

public:

	SvgCircle( double x, double y, double r);
	~SvgCircle( void);

	std::string getContent( void) const;

};


// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgEllipse : public SvgBasicShape, public SvgAddable  {

private:

public:

	SvgEllipse( double cx, double cy, double rx, double ry);
	~SvgEllipse( void);

	std::string getContent( void) const;

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgLine : public SvgBasicShape, public SvgAddable  {

private:

public:

	SvgLine( double x1, double y1, double x2, double y2);
	~SvgLine( void);

	std::string getContent( void) const;

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgText : public SvgAddable  {

private:
	
	std::string text; 
	std::vector <std::string> attrvec;

public:

	SvgText( double, double, const std::string &);
	SvgText( double, double, double);
	~SvgText( void);

	void addFontfamily( const std::string &);
	void addFontsize( double);
	void addTextanchor( const std::string &);
	void addDominantbaseline( const std::string &);
	void addRotate( double, double, double);

	std::string getContent( void) const;

};

class SvgGraph { 

private:

	SvgFile svgf;
	Cambus cam;
	double svgwidth;  // width of the whole SVG
	double svgheight; // height of the whole SVG

public:

	SvgGraph( void) = delete; 

	SvgGraph(
		double w0, double h0, double x1, double y1, double x2, double y2
	)
	 : svgf( w0, h0, x1, y1, x2, y2), svgwidth( w0), svgheight( h0)
	{}

	void setCambus( const Cambus &);

	void setBackground( const std::string &);

	void setGraphPaneColor( const std::string &);

	void startDrawingGraphPane( void);
	void endDrawingGraphPane( void);

	void drawXGridLines( 
		const std::vector <double> &, const std::string &
	);

	void drawYGridLines( 
		const std::vector <double> &, const std::string &
	);

	void drawBins(
		const std::vector <double> &,
		const std::vector <double> &,
		const std::vector <int> &,
		const std::string &,
		bool = false
	);

	void drawPoints(
		const std::vector <double> &,
		const std::vector <double> &,
		const std::string &
	);

	void drawCircle(
		double,
		double, 
		double,
		const std::string &
	);

	void drawXAxisTicks( 
		const std::vector <double> &, double, const std::string &
	);

	void drawYAxisTicks( 
		const std::vector <double> &, double, const std::string &
	);

	void setXAxisLabels(
		const std::vector <double> &, const std::string &, double, double, double
	);

	void setYAxisLabels(
		const std::vector <double> &, const std::string &, double, double, double
	);

	void setGraphTitle(
		const std::string &, const std::string &, double, double, double
	);

	void setXAxisTitle(
		const std::string &, const std::string &, double, double, double
	);

	void setYAxisTitle( 
		const std::string &, const std::string &, double, double, double
	);

	void drawGraphPaneFrame( const std::string);

	void addElement( const GraphAddable &);
	
	void addRectActu( const SvgRect &); 
	void addLineActu( const SvgLine &); 

	bool writeFile( const std::string &);

}; 

// interface for addable SvgGraph elements 
class GraphAddable {
public:
	virtual std::string getContent( const Cambus &) const = 0;
};

// Base class for GraphRect, etc.
class GraphBasicShape {

protected:

	std::string fill;
	std::string stroke;
	std::string strokewidth;
	std::string fillopacity;

	std::string getBasicShapeAttr( void) const;

public:

	GraphBasicShape( void);
	virtual ~GraphBasicShape( void);

	virtual void setFill( const std::string &s0);
	virtual void setStroke( const std::string &s0);
	virtual void setStrokewidth( double v0);
	virtual void setFillopacity( double v0);

};

// 論理座標系で指定する。(x,y)は「左下」の点。
class GraphRect : public GraphBasicShape, public GraphAddable {

private:

	double x;
	double y;
	double w;
	double h;

	std::vector < std::unique_ptr <GraphRectAnimator> > animatorpvec;

public:

	GraphRect( double x0, double y0, double w0, double h0);
	~GraphRect( void);

	double getX( void) const;
	double getY( void) const;
	double getW( void) const;
	double getH( void) const;

	std::string getContent( const Cambus &cam) const;

	void addAnimateBuildup( double);

};

// 論理座標系で指定する。
class GraphLine : public GraphBasicShape, public GraphAddable {

private:

	double x1;
	double y1;
	double x2;
	double y2;

// ↓あとで
//	std::vector < std::unique_ptr <GraphLineAnimator> > animatorpvec;

public:

	GraphLine( double xa, double ya, double xb, double yb);
	~GraphLine( void);

// ↓アニメーションを入れるときにつくる。
//	double getX1( void) const;
//	double getY1( void) const;
//	double getX2( void) const;
//	double getY2( void) const;

	std::string getContent( const Cambus &cam) const;

//	void addAnimateOpacity( double); // 透過度をゼロからMAXに？

};

// 論理座標系で指定する。
// ただしドット専用。
class GraphDot : public GraphBasicShape, public GraphAddable {

private:

	double cx;
	double cy;
	double size; // これだけは実際の座標系での大きさ（半径）

// ↓あとで
//	std::vector < std::unique_ptr <GraphCircleAnimator> > animatorpvec;

public:

	GraphDot( double cx0, double cy0, double size0);
	~GraphDot( void);

// ↓アニメーションを入れるときにつくる。
//	double getCx( void) const;
//	double getCy( void) const;
//	double getR( void) const;

	std::string getContent( const Cambus &cam) const;

//	void addAnimateDelay( double); // ある秒数から出現？

};

// interface for animators of SvgGraph elements 
class GraphRectAnimator {
public:
	virtual std::string getContent(
		const GraphRect &, const Cambus &
	) const = 0;
};

// 長方形が下から伸びてくるアニメーション
class BuildupGraphRectAnimator : public GraphRectAnimator {

private:

	double sec;

public:

	BuildupGraphRectAnimator( void) = delete;
	BuildupGraphRectAnimator( double);
	~BuildupGraphRectAnimator( void);
	
	std::string getContent( const GraphRect &, const Cambus &) const;

};

class SvgHistogramMaker {

private:

	std::vector <double> leftvec; 
	std::vector <double> rightvec; 
	std::vector <int> counts; 
	bool animated; 

	std::string graph_title;
	std::string xaxis_title; 
	std::string yaxis_title; 

	double svgwidth;
	double svgheight;
	double outermargin;
	double graph_title_fontsize;
	double graph_title_margin; // グラフタイトルの下のマージン
	double axis_title_fontsize;
	double axis_title_margin; // 軸タイトルと軸ラベルの間のマージン
	double axis_label_fontsize;
	double axis_ticklength;

public:

	SvgHistogramMaker( void) = delete;

	SvgHistogramMaker( 
		const std::vector <double> &lv0,
		const std::vector <double> &rv0,
		const std::vector <int> &c0,
		bool an0 = false
	);

	~SvgHistogramMaker( void);

	void setDefaults( void);
	
	void setGraphTitle( const std::string &s);	
	void setXAxisTitle( const std::string &s);	
	void setYAxisTitle( const std::string &s);
	
	SvgGraph createGraph( void);

};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

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

	std::vector <double> ret;
	
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
	std::vector <double> headcands = { 5.0, 2.5, 2.0, 1.0};

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

SvgFile :: 
SvgFile( double w0, double h0, double x1, double y1, double x2, double y2)
 : width( w0), height( h0),
   viewBoxX1( x1), viewBoxY1( y1), viewBoxX2( x2), viewBoxY2( y2),
   filecontent()
{}

SvgFile :: 
~SvgFile( void)
{}

// これは最終的に他のメソッドで代替される予定。
void 
SvgFile :: 
addFileContent( const std::string &s0){
	filecontent.push_back( s0);
}

void 
SvgFile :: 
addElement( const SvgAddable &elem)
{
	filecontent.push_back( elem.getContent());
}

std::vector <std::string> 
SvgFile :: 
getFileContent( void)
{

	std::vector <std::string> ret;
	ret.reserve( filecontent.size() + 4);

	// 1行目
	{
		// This line should be exactly in the first line in SVG file.
		const std::string line1   
		= { R"(<?xml version="1.0" encoding="UTF-8" ?>)"};
		ret.push_back( line1);
	}

	// 2行目
	{
		std::stringstream ss;
		ss << R"(<svg width=")" << width << R"(px")" << " " 
		<< R"(height=")" << height << R"(px")" << " " 
		<< R"(viewBox=")" << viewBoxX1 << " " << viewBoxY1 << " " << viewBoxX2 << " " << viewBoxY2 << R"(")" << " " 
		<< R"(xmlns="http://www.w3.org/2000/svg">)";
		ret.push_back( ss.str());
	}

	// 3行目以降＝SVGの中身
	ret.insert( ret.end(), filecontent.begin(), filecontent.end());

	// 最後の2行
	ret.push_back( "</svg>");
	std::string detector = "<!-- " 
						u8"\u6587\u5B57\u30B3\u30FC\u30C9\u8B58\u5225\u7528" // 「文字コード識別用」というUTF-8文字列
						" -->";
	ret.push_back( detector);
	
	return ret;

}

// ファイルに書き出すメソッド
// 正常終了でtrue、異常終了でfalseを返す。
// 
// appendのデフォルト値はtrue、overwriteのデフォルト値はfalse、askのデフォルト値はtrue。
// 同じファイル名のファイルがすでに存在する場合の挙動は以下のとおり。
// appendがtrueなら、追加書き込みモードで開く。overwriteとaskは無視する。
// appendがfalseで、overwriteがtrueなら、上書きする。
// appendがfalseで、overwriteがfalseで、askがtrueなら、上書きするかどうかをコンソールで尋ねる。
// appendがfalseで、overwriteがfalseで、askがfalseなら、ファイルを開かない。
bool 
SvgFile :: 
writeFile(
	const std::string &fn0,
	bool append,    // = false
	bool overwrite, // = false
	bool ask        // = true
)
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


/* ***** class SvgBasicShape ***** */

SvgBasicShape :: 
SvgBasicShape( void)
 : attrvec()
{}

//virtual 
SvgBasicShape :: 
~SvgBasicShape( void)
{}

//virtual 
void 
SvgBasicShape :: 
addFill( const std::string &s0)
{
	attrvec.push_back( R"(fill=")" + s0 + R"(")");
}

//virtual 
void 
SvgBasicShape :: 
addStroke( const std::string &s0)
{
	attrvec.push_back( R"(stroke=")" + s0 + R"(")");
}

//virtual 
void 
SvgBasicShape :: 
addStrokewidth( double v0)
{
	std::stringstream ss;
	ss << R"(stroke-width=")" << v0 << R"(")";
	attrvec.push_back( ss.str());
}

//virtual 
void 
SvgBasicShape :: 
addFillopacity( double v0)
{
	std::stringstream ss;
	ss << R"(fill-opacity=")" << v0 << R"(")";
	attrvec.push_back( ss.str());
}


/* ***** class SvgRect ***** */

SvgRect :: 
SvgRect( double x0, double y0, double w0, double h0)
: SvgBasicShape(), animatevec()
{
	std::stringstream ss;
	ss << R"(x=")"      << x0 << R"(")" << " "
		<< R"(y=")"      << y0 << R"(")" << " "
		<< R"(width=")"  << w0 << R"(")" << " " 
		<< R"(height=")" << h0 << R"(")";
	attrvec.push_back( ss.str());
}

SvgRect :: 
~SvgRect( void)
{}

void 
SvgRect :: 
addAnimate( const std::string &attr, double v1, double v2, double sec)
{
	std::stringstream ss;
	ss << "<animate "
		<< "attributeName=\"" << attr << "\"" << " "
		<< "begin=\"0s\"" << " " 
		<< "dur=\"" << sec << "\"" << " "
		<< "from=\"" << v1 << "\"" << " " 
		<< "to=\"" << v2 << "\"" << " " 
		<< "repeatCount=\"1\""
		<< "/>";
	animatevec.push_back( ss.str());
}

std::string 
SvgRect :: 
getContent( void) 
const
{

	std::string ret;

	if ( animatevec.size() < 1){
	
		ret = "<rect";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += " />";
	
	} else {
	
		ret = "<rect";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += " >";
		for ( const auto &animatetag : animatevec){
			ret += animatetag;
		}
		ret += "</rect>";
	
	}

	return ret;

}


/* ***** class SvgCircle ***** */

SvgCircle :: 
SvgCircle( double x, double y, double r)
: SvgBasicShape() 
{
	std::stringstream ss;
	ss << R"(cx=")" << x << R"(")" << " "
		<< R"(cy=")" << y << R"(")" << " "
		<< R"(r=")" << r << R"(")";
	attrvec.push_back( ss.str());
}

SvgCircle :: 
~SvgCircle( void)
{}

std::string 
SvgCircle :: 
getContent( void)
const
{
	std::string ret = "<circle";
	for ( const auto &attr : attrvec){
		ret += " ";
		ret += attr;
	}
	ret += " />";
	return ret;
}


/* ***** class SvgEllipse ***** */

SvgEllipse :: 
SvgEllipse( double cx, double cy, double rx, double ry)
: SvgBasicShape() 
{
	std::stringstream ss;
	ss << R"(cx=")" << cx << R"(")" << " "
		<< R"(cy=")" << cy << R"(")" << " "
		<< R"(rx=")" << rx << R"(")" << " "
		<< R"(ry=")" << ry << R"(")";
	attrvec.push_back( ss.str());
}

SvgEllipse :: 
~SvgEllipse( void)
{}

std::string 
SvgEllipse :: 
getContent( void) 
const
{
	std::string ret = "<ellipse";
	for ( const auto &attr : attrvec){
		ret += " ";
		ret += attr;
	}
	ret += " />";
	return ret;
}


/* ***** class SvgLine ***** */

SvgLine :: 
SvgLine( double x1, double y1, double x2, double y2)
: SvgBasicShape() 
{
	std::stringstream ss;
	ss << R"(x1=")" << x1 << R"(")" << " "
		<< R"(y1=")" << y1 << R"(")" << " "
		<< R"(x2=")" << x2 << R"(")" << " "
		<< R"(y2=")" << y2 << R"(")";
	attrvec.push_back( ss.str());
}

SvgLine :: 
~SvgLine( void)
{}

std::string 
SvgLine :: 
getContent( void) 
const
{
	std::string ret = "<line";
	for ( const auto &attr : attrvec){
		ret += " ";
		ret += attr;
	}
	ret += " />";
	return ret;
}


/* ***** class SvgText ***** */

SvgText :: 
SvgText( double x0, double y0, const std::string &s0)
 : text( s0), attrvec()
{
	std::stringstream ss;
	ss << R"(x=")" << x0 << R"(")" << " "
	   << R"(y=")" << y0 << R"(")";
	attrvec.push_back( ss.str());
}

SvgText :: 
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

SvgText :: 
~SvgText( void)
{}

void 
SvgText :: 
addFontfamily( const std::string &s0)
{
	attrvec.push_back( R"(font-family=")" + s0 + R"(")");
}	

void 
SvgText :: 
addFontsize( double v0)
{
	std::stringstream ss;
	ss << v0;
	attrvec.push_back( R"(font-size=")" + ss.str() + R"(")");
}	

void 
SvgText :: 
addTextanchor( const std::string &s0)
{
	attrvec.push_back( R"(text-anchor=")" + s0 + R"(")");
}	

void 
SvgText :: 
addDominantbaseline( const std::string &s0)
{
	attrvec.push_back( R"(dominant-baseline=")" + s0 + R"(")");
}	

void 
SvgText :: 
addRotate( double d0, double x0, double y0)
{
	std::stringstream ss;
	ss << R"(transform="rotate)" << "(" << d0 << " " << x0 << " " << y0 << ")" << R"(")";
	attrvec.push_back( ss.str());
}	

std::string 
SvgText :: 
getContent( void) const
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


/* ***** class SvgGraph ***** */

// GraphPaneの座標系を示すCambusを設定する。
void 
SvgGraph :: 
setCambus( const Cambus &c0)
{
	cam = c0;
}

// SVG全体の背景色を設定
void 
SvgGraph :: 
setBackground( const std::string &b0)
{
	SvgRect r1( 0, 0, svgwidth, svgheight);
	r1.addFill( b0);
	r1.addStroke( b0);
	addRectActu( r1);
}

// graphpane = cambus の背景色を塗る。
void 
SvgGraph :: 
setGraphPaneColor( const std::string &cambuscolor)
{
	
	{
		SvgRect r( cam.actuXMin, cam.actuYMin, cam.actuWidth, cam.actuHeight);
		r.addFill( cambuscolor);
		r.addStroke( cambuscolor);
		addRectActu( r);
	}

}

// GraphPaneでの描画を開始する。
// GraphPaneでの描画をはみ出さないようにする。
// 座標系は変化しない。
// 必ずendDrawingGraphPane()を終了時に実行すべし。
void 
SvgGraph :: 
startDrawingGraphPane( void)
{
	double actuX = cam.actuXMin;
	double actuY = cam.actuYMin;
	double paneWidth = cam.actuWidth;
	double paneHeight = cam.actuHeight;

	stringstream ss;
	ss << "<svg "
	   << "x=\"" << actuX << "\"" << " "
	   << "y=\"" << actuY << "\"" << " " 
	   << "width=\"" << paneWidth << "\"" << " " 
	   << "height=\"" << paneHeight << "\"" << " " 
	   << "viewBox=\""
	       << actuX << " " << actuY << " " 
		   << paneWidth << " " << paneHeight
	   << "\"" << ">";
	svgf.addFileContent( ss.str());

}

// GraphPaneでの描画を終了しる。
void 
SvgGraph :: 
endDrawingGraphPane( void)
{
	svgf.addFileContent( "</svg>");
}

// x軸の目盛を示すグリッド線を引く。
void 
SvgGraph :: 
drawXGridLines(
	const std::vector <double> &xgridpoints,
	const std::string &linecolor 
)
{

	double theoy_top = cam.theoYMax;
	double theoy_bottom = cam.theoYMin;

	for ( auto v : xgridpoints){

		GraphLine li( v, theoy_top, v, theoy_bottom);
		li.setStroke( linecolor);
		li.setStrokewidth( 1);
		addElement( li); 

	}

}

// y軸の目盛を示すグリッド線を引く。
void 
SvgGraph :: 
drawYGridLines(
	const std::vector <double> &ygridpoints,
	const std::string &linecolor 
)
{

	double theox_left = cam.theoXMin;
	double theox_right = cam.theoXMax;

	for ( auto v : ygridpoints){

		GraphLine li( theox_left, v, theox_right, v);
		li.setStroke( linecolor);
		li.setStrokewidth( 1);
		addElement( li); 

	}

}

// 度数を示すバーを描く。
// animted = trueのとき、下から1秒間で伸びてくるアニメーションが加わる。
// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawBins(
	const std::vector <double> &leftvec, 
	const std::vector <double> &rightvec, 
	const std::vector <int>    &counts, 
	const std::string          &color,
	bool animated // = false 
)
{

	for ( int i = 0; i < counts.size(); i++){

		GraphRect rect( leftvec[ i], 0, rightvec[ i] - leftvec[ i], counts[ i]);
		rect.setFill( color);
		rect.setStroke( color);

		if ( animated == true){
			rect.addAnimateBuildup( 1);
		}

		addElement( rect); 

	}

}

// 散布図となる点を描く。
// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawPoints(
	const std::vector <double> &xvec, 
	const std::vector <double> &yvec, 
	const std::string          &color
)
{

	int n = xvec.size();
	if ( xvec.size() < yvec.size()){
		n = yvec.size();
	}

	for ( int i = 0; i < n; i++){

		GraphDot dot( xvec[ i], yvec[ i], 1);
		dot.setFill( color);
		dot.setStroke( color);
		addElement( dot);

	}

}

// 円を描く。
// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawCircle(
	double cx, double cy, double r,
	const std::string &color
)
{

	// 縦方向と横方向の縮尺が違う場合に備えて、楕円を利用する。

	Point theoP1( cx, cy); 
	Point theoP2( cx+r, cy); 
	Point theoP3( cx, cy+r); 
	Point actuP1 = cam.getActualFromTheoretical( theoP1);
	Point actuP2 = cam.getActualFromTheoretical( theoP2);
	Point actuP3 = cam.getActualFromTheoretical( theoP3);

	SvgEllipse el( actuP1.x, actuP1.y, 
	               actuP2.x - actuP1.x, // 横方向の径
				   actuP1.y - actuP3.y  // 縦方向の径
	);
	el.addFill( "none");
	el.addStroke( color);
	svgf.addElement( el);

}

// x軸の目盛のヒゲを描く。
void 
SvgGraph :: 
drawXAxisTicks(
	const std::vector <double> &xgridpoints,
	double ticklength,
	const std::string &color
)
{

	for ( auto v : xgridpoints){

		double actuX = cam.getXActualFromTheoretical( v);

		double x1 = actuX;
		double y1 = cam.actuYMax;
		double x2 = actuX;
		double y2 = cam.actuYMax + ticklength; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( color); 
		li.addStrokewidth( 1); 
		svgf.addElement( li);		

	}

}

// y軸の目盛のヒゲを描く。
void 
SvgGraph :: 
drawYAxisTicks(
	const std::vector <double> &ygridpoints,
	double ticklength,
	const std::string &color
)
{

	for ( auto v : ygridpoints){

		double actuY = cam.getYActualFromTheoretical( v);

		double x1 = cam.actuXMin;
		double y1 = actuY;
		double x2 = cam.actuXMin - ticklength; 
		double y2 = actuY; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( color); 
		li.addStrokewidth( 1); 
		svgf.addElement( li);		

	}

}

// x軸の目盛のラベルを描く。
void 
SvgGraph :: 
setXAxisLabels(
	const std::vector <double> &xgridpoints, 
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double ticklength       
)
{

	for ( auto v : xgridpoints){

		double actuX = cam.getXActualFromTheoretical( v);

		// textタグで、IEやWordはdominant-baselineが効かないらしい。
		// （指定してもdominant-baseline="alphabetic"扱いになる。）
		
		// 描画領域の下端から以下の余白だけ離す。
		// alphabeticの基線は指定座標よりfontbaseだけ上なので、その分をずらしている。
		double actuY = cam.actuYMax + ticklength + fontsize * ( 1.0 - fontbase);

		// vの桁数はどうなるのか。。 

		SvgText te( actuX, actuY, v); // vはstringに変換される。
		te.addFontfamily( fontface);
		te.addFontsize( fontsize);
		te.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		te.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
		svgf.addElement( te);		

	}

}

// y軸の目盛のラベルを描く。
void 
SvgGraph ::
setYAxisLabels(
	const std::vector <double> &ygridpoints, 
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double ticklength       
)
{

	/*
	文字列を回転させる方法を探った→svgtest04.svgとsvgtest05.svg
	　svgtest04.svgで2つの方法を試したが、もっとシンプルにしたかった。
	　svgtest05.svgで、transform属性を使えばよいことがわかった。
	*/
	for ( auto v : ygridpoints){

		double actuY = cam.getYActualFromTheoretical( v);

		// alphabetic基線に合わせるためにfontbaseだけずらしている。
		double actuX = cam.actuXMin - ticklength - fontsize * fontbase;
		
		SvgText te( actuX, actuY, v); // vはstringに変換される。
		te.addFontfamily( fontface);
		te.addFontsize( fontsize);
		te.addTextanchor( "middle"); 
		te.addDominantbaseline( "alphabetic"); 
		te.addRotate( 270, actuX, actuY); // 回転の中心が各点で異なるので、一括指定できない。
		svgf.addElement( te);				

	}

}

// グラフタイトルを表示
void 
SvgGraph :: 
setGraphTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = std::round( svgwidth / 2);
	double actuY = outermargin + fontsize * ( 1 - fontbase);

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); // 左右方向に中央揃えをする。
	te.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
	svgf.addElement( te);	

}

// x軸タイトルを表示
void 
SvgGraph :: 
setXAxisTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = std::round( cam.getActualMidX());
	double actuY = svgheight - outermargin - fontsize * fontbase; 

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); 
	te.addDominantbaseline( "alphabetic"); 
	svgf.addElement( te);	

}

// y軸タイトルを表示
void 
SvgGraph ::
setYAxisTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = outermargin + fontsize * ( 1 - fontbase);
	double actuY = std::round( cam.getActualMidY());

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); 
	te.addDominantbaseline( "alphabetic"); 
	te.addRotate( 270, actuX, actuY); // 回転の中心が各点で異なるので、一括指定できない。
	svgf.addElement( te);	

}

// graphpaneの枠線を描画
// 注：この描画は最後にすべき。
// 　　fillは透過させる。
void 
SvgGraph :: 
drawGraphPaneFrame(
	const std::string color
)
{
	SvgRect r( cam.actuXMin, cam.actuYMin, cam.actuWidth, cam.actuHeight);
	r.addFillopacity( 0); // 完全透過
	r.addStroke( color);
	r.addStrokewidth( 1);
	svgf.addElement( r);
}

// 要素オブジェクトを追加。
// elem内でcamを使って座標変換することになる。
void 
SvgGraph :: 
addElement( const GraphAddable &elem)
{
	svgf.addFileContent( elem.getContent( cam));
}

// 座標変換せずに描画
void 
SvgGraph :: 
addRectActu( const SvgRect &r0)
{
	svgf.addElement( r0);
}

// 座標変換せずに描画
void 
SvgGraph :: 
addLineActu( const SvgLine &l0)
{
	svgf.addElement( l0);
}

// ファイルに書き出すメソッド
// 正常終了でtrue、異常終了でfalseを返す。
bool 
SvgGraph :: 
writeFile( const std::string &fn0)
{
	return svgf.writeFile( fn0);
}


/* ***** class GraphBasicShape ***** */

std::string 
GraphBasicShape :: 
getBasicShapeAttr( void) const
{

	std::stringstream ss;

	if ( fill.size() > 0){
		ss << "fill=" << "\"" << fill << "\"" << " ";
	}
	
	if ( stroke.size() > 0){
		ss << "stroke=" << "\"" << stroke << "\"" << " ";
	}

	if ( strokewidth.size() > 0){
		ss << "stroke-width=" << "\"" << strokewidth << "\"" << " ";
	}

	if ( fillopacity.size() > 0){
		ss << "fill-opacity=" << "\"" << fillopacity << "\"" << " ";
	}

	return ss.str();

}

GraphBasicShape :: 
GraphBasicShape( void)
: fill(), stroke(), strokewidth(), fillopacity()
{}

//virtual 
GraphBasicShape :: 
~GraphBasicShape( void)
{}

//virtual 
void 
GraphBasicShape :: 
setFill( const std::string &s0)
{
	fill = s0;
}

//virtual 
void 
GraphBasicShape :: 
setStroke( const std::string &s0)
{
	stroke = s0;
}

//virtual 
void 
GraphBasicShape :: 
setStrokewidth( double v0)
{
	std::stringstream ss;
	ss << v0;
	strokewidth = ss.str();
}

//virtual 
void 
GraphBasicShape :: 
setFillopacity( double v0)
{
	std::stringstream ss;
	ss << v0;
	fillopacity = ss.str();
}


/* ***** class GraphRect ***** */

GraphRect :: 
GraphRect( double x0, double y0, double w0, double h0)
: GraphBasicShape(), x( x0), y( y0), w( w0), h( h0), animatorpvec()
{}

GraphRect :: 
~GraphRect( void)
{}

double 
GraphRect :: 
getX( void)
const
{
	return x;
}

double 
GraphRect :: 
getY( void)
const
{
	return y;
}

double 
GraphRect :: 
getW( void)
const
{
	return w;
}

double 
GraphRect :: 
getH( void)
const
{
	return h;
}

std::string 
GraphRect :: 
getContent( const Cambus &cam) 
const
{

	double leftx   = cam.getXActualFromTheoretical( x);
	double bottomy = cam.getYActualFromTheoretical( y);
	double rightx  = cam.getXActualFromTheoretical( x + w);
	double topy    = cam.getYActualFromTheoretical( y + h);
	double actuw   = rightx - leftx; 
	double actuh   = bottomy - topy;

	std::stringstream ss;

	ss << "<rect" << " ";

	ss <<
		R"(x=")"      << leftx << R"(")" << " " << 
		R"(y=")"      << topy  << R"(")" << " " << 
		R"(width=")"  << actuw << R"(")" << " " << 
		R"(height=")" << actuh << R"(")" << " ";
	
	ss << getBasicShapeAttr() << ">";

	int animatorlen = animatorpvec.size();
	if ( animatorlen > 0){

		ss << std::endl;

		for ( int i = 0; i < animatorlen; i++){
			ss << animatorpvec[ i]->getContent( *this, cam);
			ss << std::endl;
		}

	}

	ss << "</rect>";

	return ss.str();

}

// 長方形が下から伸びてくるアニメーションを追加する。
void
GraphRect :: 
addAnimateBuildup( double sec)
{

	// BuildupGraphRectAnimatorのコンストラクタを呼び、
	// インスタンスをつくってunique_ptrにして、それをvectorに追加。
	animatorpvec.push_back( std::make_unique <BuildupGraphRectAnimator> ( sec));
	
}


/* ***** class GraphLine ***** */

GraphLine :: 
GraphLine( double xa, double ya, double xb, double yb)
: GraphBasicShape(), x1( xa), y1( ya), x2( xb), y2( yb) // , animatorpvec()
{}

GraphLine :: 
~GraphLine( void)
{}

std::string 
GraphLine :: 
getContent( const Cambus &cam) 
const
{

	double actux1 = cam.getXActualFromTheoretical( x1);
	double actuy1 = cam.getYActualFromTheoretical( y1);
	double actux2 = cam.getXActualFromTheoretical( x2);
	double actuy2 = cam.getYActualFromTheoretical( y2);

	std::stringstream ss;

	ss << "<line" << " ";

	ss <<
		R"(x1=")" << actux1 << R"(")" << " " << 
		R"(y1=")" << actuy1 << R"(")" << " " << 
		R"(x2=")" << actux2 << R"(")" << " " << 
		R"(y2=")" << actuy2 << R"(")" << " ";
	
	ss << getBasicShapeAttr() << ">";

// ↓まだ。
//	int animatorlen = animatorpvec.size();
//	if ( animatorlen > 0){
//
//		ss << std::endl;
//
//		for ( int i = 0; i < animatorlen; i++){
//			ss << animatorpvec[ i]->getContent( *this, cam);
//			ss << std::endl;
//		}
//
//	}

	ss << "</line>";

	return ss.str();

}

/* ***** class GraphDot ***** */

GraphDot :: 
GraphDot( double cx0, double cy0, double size0)
: GraphBasicShape(), cx( cx0), cy( cy0), size( size0) // , animatorpvec()
{}

GraphDot :: 
~GraphDot( void)
{}

std::string 
GraphDot :: 
getContent( const Cambus &cam) 
const
{

	double actux = cam.getXActualFromTheoretical( cx);
	double actuy = cam.getYActualFromTheoretical( cy);

	std::stringstream ss;

	ss << "<circle" << " ";

	ss <<
		R"(cx=")" << actux << R"(")" << " " << 
		R"(cy=")" << actuy << R"(")" << " " << 
		R"(r=")"  << size << R"(")" << " ";
	
	ss << getBasicShapeAttr() << ">";

// ↓まだ。
//	int animatorlen = animatorpvec.size();
//	if ( animatorlen > 0){
//
//		ss << std::endl;
//
//		for ( int i = 0; i < animatorlen; i++){
//			ss << animatorpvec[ i]->getContent( *this, cam);
//			ss << std::endl;
//		}
//
//	}

	ss << "</circle>";

	return ss.str();

}


/* ***** class BuildupGraphRectAnimator ***** */

BuildupGraphRectAnimator ::
BuildupGraphRectAnimator( double s0)
: sec( s0)
{}

BuildupGraphRectAnimator ::
~BuildupGraphRectAnimator( void)
{}

std::string 
BuildupGraphRectAnimator :: 
getContent( const GraphRect &rect, const Cambus &cam)
const
{

	double x = rect.getX();
	double y = rect.getY();
	double w = rect.getW();
	double h = rect.getH();

	// below are "actual" coordinate points
	double leftx = cam.getXActualFromTheoretical( x);
	double rightx = cam.getXActualFromTheoretical( x + w);
	double bottomy = cam.getYActualFromTheoretical( y);
	double topy = cam.getYActualFromTheoretical( y + h);

	double actuy_start = bottomy;
	double actuy_end = topy;

	double actuh_start = 0;
	double actuh_end = bottomy - topy;

	std::stringstream ss;
	ss << "<animate " <<
		"attributeName=\"y\"" << " " << 
		"begin=\"0s\"" << " " <<
		"dur=\"" << sec << "\"" << " " << 
		"from=\"" << actuy_start << "\"" << " " <<
		"to=\"" << actuy_end << "\"" << " " <<
		"repeatCount=\"1\"" <<
	"/>";

	ss << std::endl;
	
	ss << "<animate " << 
		"attributeName=\"height\"" << " " << 
		"begin=\"0s\"" << " " <<
		"dur=\"" << sec << "\"" << " " << 
		"from=\"" << actuh_start << "\"" << " " <<
		"to=\"" << actuh_end << "\"" << " " <<
		"repeatCount=\"1\"" <<
	"/>";

	return ss.str();

}


/* ***** class SvgHistogramMaker ***** */

SvgHistogramMaker :: 
SvgHistogramMaker( 
	const std::vector <double> &lv0,
	const std::vector <double> &rv0,
	const std::vector <int> &c0,
	bool an0 // = false
) : leftvec( lv0), rightvec( rv0), counts( c0), animated( an0)
{
	setDefaults();
}

SvgHistogramMaker :: 
~SvgHistogramMaker( void)
{}

void 
SvgHistogramMaker :: 
setDefaults( void)
{

	graph_title = "(No Title)";
	xaxis_title = "x (Not Labeled)";
	yaxis_title = "y (Not Labeled)";

	// default values
	svgwidth = 500;
	svgheight = 500;
	outermargin = 20;
	graph_title_fontsize = 20;
	graph_title_margin = 10; // グラフタイトルの下のマージン
	axis_title_fontsize = 20;
	axis_title_margin = 5; // 軸タイトルと軸ラベルの間のマージン
	axis_label_fontsize = 14;
	axis_ticklength = 5;

}

void 
SvgHistogramMaker :: 
setGraphTitle( const std::string &s)
{
	graph_title = s;
}

void 
SvgHistogramMaker :: 
setXAxisTitle( const std::string &s)
{
	xaxis_title = s;
}

void 
SvgHistogramMaker :: 
setYAxisTitle( const std::string &s)
{
	yaxis_title = s;
}

SvgGraph
SvgHistogramMaker :: 
createGraph( void)
{

	SvgGraph svgg( svgwidth, svgheight, 0, 0, svgwidth, svgheight);

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
	std::vector <double> xgridpoints = getGridPoints( xminval, xmaxval);

	// y軸
	double yminval = 0;
	double ymaxval = *( max_element( counts.begin(), counts.end()));
	std::vector <double> ygridpoints = getGridPoints( yminval, ymaxval);
	
	
	// 描画範囲は、Gridpointsのさらに5%外側にする。
	double theoWidthTemp = xgridpoints.back() - xgridpoints.front();
	double theoXMin = xgridpoints.front() - 0.05 * theoWidthTemp;
	double theoXMax = xgridpoints.back() + 0.05 * theoWidthTemp;
	
	double theoHeightTemp = ygridpoints.back() - ygridpoints.front();
	double theoYMin = ygridpoints.front() - 0.05 * theoHeightTemp;
	double theoYMax = ygridpoints.back() + 0.05 * theoHeightTemp;
	
	// SvgGraphインスタンス内のキャンバスの設定
	// 注：このCambusオブジェクトが与えられたあとで、このオブジェクトを用いて
	// 　　「即時に」座標変換がなされる。
	// 　　あとでCambusオブジェクトを入れ替えてもそれまでに追加された描画部品には影響しない。

	Cambus cam;
	cam.setActual( graphpane_actuX1, graphpane_actuY1, graphpane_actuX2, graphpane_actuY2);
	cam.setTheoretical( theoXMin, theoYMin, theoXMax, theoYMax);
	svgg.setCambus( cam);


	// 背景の描画開始

	svgg.setBackground( "whitesmoke");

	svgg.setGraphPaneColor( "gainsboro");

	// 背景の描画終了


	// 描画領域への描画開始

	svgg.startDrawingGraphPane();

	svgg.drawXGridLines( xgridpoints, "silver");
	svgg.drawYGridLines( ygridpoints, "silver");

	if ( animated == true){
		svgg.drawBins( leftvec, rightvec, counts, "gray", true);
	} else {
		svgg.drawBins( leftvec, rightvec, counts, "gray");
	}

	svgg.endDrawingGraphPane();

	// 描画領域への描画終了


	// 周辺情報記載の開始

	svgg.drawXAxisTicks( xgridpoints, axis_ticklength, "black");
	svgg.setXAxisLabels( xgridpoints, "Arial,san-serif", 0.2, axis_label_fontsize, axis_ticklength);
	// Arial san-serif は、alphabetic基線がいつも0.2ズレているのか？

	// 目盛ラベルの数値の桁数はどうなるのか。。

	svgg.drawYAxisTicks( ygridpoints, axis_ticklength, "black");
	svgg.setYAxisLabels( ygridpoints, "Arial,san-serif", 0.2, axis_label_fontsize, axis_ticklength);

	svgg.setGraphTitle( graph_title, "Arial,san-serif", 0.2, graph_title_fontsize, outermargin);
	// タイトル文字列内で、"<"とかを自動でエスケープしたい。

	svgg.setXAxisTitle( xaxis_title, "Arial,san-serif", 0.2,  axis_title_fontsize, outermargin);
	svgg.setYAxisTitle( yaxis_title, "Arial,san-serif", 0.2,  axis_title_fontsize, outermargin);
	
	// 周辺情報記載の終了

	// TODO: 軸の単位の記載→優先順位は低い。

	// SVGアニメをパワポに貼っても動かないらしい。

	// textタグで、IEやWordはdominant-baselineが効かないらしい。
	// （指定してもdominant-baseline="alphabetic"扱いになる。）

	// 以上、<g>での一括指定ができるところが多いが、とりあえず無視する。



	// 枠線
	svgg.drawGraphPaneFrame( "black");


	// TODO: フォントサイズを自動調節する？

	// SVGのviewBoxについて：アスペクト比が違っているとわかりにくい。
	// （強制的に余白がつくられたりするか、強制的に拡大縮小して円が歪んだりする）ので、
	// svgタグのサイズとviewBoxのサイズを合わせたい。
	
	// ここを書いていく。

	
	return svgg;




}


