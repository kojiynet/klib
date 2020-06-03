
/*
	
	ksvg00.cpp
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto

	SVGファイルを作成するためのモジュール
	以下の場合はさらにsvggraphを使う。
	・特定の形式のグラフを出力する場合。
	・座標変換をかける場合。

	-std:c++17でコンパイルが通るかやってみる。
	gccでも。
	
	SvgAddableをまとったクラスは、論理座標が要らない場合用。

	Memo: 
	SVGアニメをパワポに貼っても動かないらしい。
	textタグで、IEやWordはdominant-baselineが効かないらしい。
	　（指定してもdominant-baseline="alphabetic"扱いになる。）
	SVGのviewBoxについて：アスペクト比が違っているとわかりにくい。
	　（強制的に余白がつくられたりするか、強制的に拡大縮小して円が歪んだりする）ので、
	　svgタグのサイズとviewBoxのサイズを合わせたい。）

*/	


/* ********** Preprocessor Directives ********** */

#ifndef ksvg_cpp_include_guard
#define ksvg_cpp_include_guard

#include <vector>
#include <string>
#include <sstream>

#include <k09/koutputfile00.cpp>


/* ********** Namespace Declarations/Directives ********** */


/* ********** Class Declarations ********** */

class SvgFile;

class SvgAddable;
class SvgBasicShape;
class SvgRect;
class SvgCircle;
class SvgEllipse; 
class SvgLine;
class SvgText;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */


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

public:

	SvgFile() = delete;
	SvgFile( double w0, double h0, double x1, double y1, double x2, double y2);
	SvgFile( double w0, double h0);
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

// interface for svg element classes which are addable to 
// SvgFile class instance 
class SvgAddable {
public:
	virtual std::string getContent( void) const = 0;
};

// Base class for Rect, Circle, Ellipse, Line
class SvgBasicShape {

protected:

	std::string fill;
	std::string stroke;
	std::string strokewidth;
	std::string fillopacity;

	virtual std::string getBasicShapeAttr( void) const;

public:

	SvgBasicShape( void);
	virtual ~SvgBasicShape( void);

	virtual void addFill( const std::string &s0);
	virtual void addStroke( const std::string &s0);
	virtual void addStrokewidth( double v0);
	virtual void addFillopacity( double v0);

};


// アニメーション機能は削除した。
class SvgRect : public SvgBasicShape, public SvgAddable {

private:

	double x;
	double y;
	double w;
	double h;

public:

	SvgRect( double x0, double y0, double w0, double h0);
	~SvgRect( void);

	std::string getContent( void) const;

};


class SvgCircle : public SvgBasicShape, public SvgAddable {

private:

	double cx;
	double cy;
	double r;

public:

	SvgCircle( double, double, double);
	~SvgCircle( void);

	std::string getContent( void) const;

};


class SvgEllipse : public SvgBasicShape, public SvgAddable  {

private:

	double cx;
	double cy;
	double rx; // x軸方向の半径
	double ry; // y軸方向の半径

public:

	SvgEllipse( double cx0, double cy0, double rx0, double ry0);
	~SvgEllipse( void);

	std::string getContent( void) const;

};


class SvgLine : public SvgBasicShape, public SvgAddable  {

private:

	double x1;
	double y1;
	double x2;
	double y2;

public:

	SvgLine( double x1_, double y1_, double x2_, double y2_);
	~SvgLine( void);

	std::string getContent( void) const;

};


class SvgText : public SvgAddable  {

private:
	
	std::string text; 

	double x;
	double y; 
	std::string fontfamily;
	std::string fontsize;
	std::string textanchor;
	std::string dominantbaseline;
	std::string rotateDeg;
	std::string rotateX;
	std::string rotateY;

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


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */


/* ********** Definitions of Member Functions ********** */

/* ***** class SvgFile ***** */

SvgFile :: 
SvgFile( double w0, double h0, double x1, double y1, double x2, double y2)
 : width( w0), height( h0),
   viewBoxX1( x1), viewBoxY1( y1), viewBoxX2( x2), viewBoxY2( y2),
   filecontent()
{}

SvgFile :: 
SvgFile( double w0, double h0)
 : width( w0), height( h0),
   viewBoxX1( 0), viewBoxY1( 0), viewBoxX2( w0), viewBoxY2( h0),
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
 : fill(), stroke(), strokewidth(), fillopacity()
{}

// virtual 
SvgBasicShape :: 
~SvgBasicShape( void)
{}

// virtual
std::string 
SvgBasicShape :: 
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

// virtual 
void 
SvgBasicShape :: 
addFill( const std::string &s0)
{
	fill = s0;
}

// virtual 
void 
SvgBasicShape :: 
addStroke( const std::string &s0)
{
	stroke = s0;
}

// virtual 
void 
SvgBasicShape :: 
addStrokewidth( double v0)
{
	std::stringstream ss;
	ss << v0;
	strokewidth = ss.str();
}

// virtual 
void 
SvgBasicShape :: 
addFillopacity( double v0)
{
	std::stringstream ss;
	ss << v0;
	fillopacity = ss.str();
}


/* ***** class SvgRect ***** */

SvgRect :: 
SvgRect( double x0, double y0, double w0, double h0)
: SvgBasicShape(), x( x0), y( y0), w( w0), h( h0)
{}

SvgRect :: 
~SvgRect( void)
{}

std::string 
SvgRect :: 
getContent( void) 
const
{

	std::stringstream ss;

	ss << "<rect ";

	ss << 
		R"(x=")"      << x << R"(")" << " " << 
		R"(y=")"      << y << R"(")" << " " << 
		R"(width=")"  << w << R"(")" << " " << 
		R"(height=")" << h << R"(")" << " ";
	
	ss << getBasicShapeAttr();

	ss << ">";

	ss << "</rect>";

	return ss.str();

}


/* ***** class SvgCircle ***** */

SvgCircle :: 
SvgCircle( double cx0, double cy0, double r0)
: SvgBasicShape(), cx( cx0), cy( cy0), r( r0)
{}

SvgCircle :: 
~SvgCircle( void)
{}

std::string 
SvgCircle :: 
getContent( void)
const
{

	std::stringstream ss;

	ss << "<circle ";

	ss <<
		R"(cx=")" << cx << R"(")" << " " << 
		R"(cy=")" << cy << R"(")" << " " << 
		R"(r=")"  << r  << R"(")" << " ";
	
	ss << getBasicShapeAttr();

	ss << ">";

	ss << "</circle>";

	return ss.str();

}


/* ***** class SvgEllipse ***** */

SvgEllipse :: 
SvgEllipse( double cx0, double cy0, double rx0, double ry0)
: SvgBasicShape(), cx( cx0), cy( cy0), rx( rx0), ry( ry0) 
{}

SvgEllipse :: 
~SvgEllipse( void)
{}

std::string 
SvgEllipse :: 
getContent( void) 
const
{

	std::stringstream ss;

	ss << "<ellipse ";

	ss <<
		R"(cx=")" << cx << R"(")" << " " << 
		R"(cy=")" << cy << R"(")" << " " << 
		R"(rx=")" << rx << R"(")" << " " << 
		R"(ry=")" << ry << R"(")" << " ";	
	
	ss << getBasicShapeAttr();

	ss << ">";

	ss << "</ellipse>";

	return ss.str();	

}


/* ***** class SvgLine ***** */

SvgLine :: 
SvgLine( double x1_, double y1_, double x2_, double y2_)
: SvgBasicShape(), x1( x1_), y1( y1_), x2( x2_), y2( y2_)  
{}

SvgLine :: 
~SvgLine( void)
{}

std::string 
SvgLine :: 
getContent( void) 
const
{

	std::stringstream ss;

	ss << "<line ";

	ss << 
		R"(x1=")" << x1 << R"(")" << " " << 
		R"(y1=")" << y1 << R"(")" << " " << 
		R"(x2=")" << x2 << R"(")" << " " << 
		R"(y2=")" << y2 << R"(")" << " ";

	ss << getBasicShapeAttr();

	ss << ">";

	ss << "</line>";

	return ss.str();

}


/* ***** class SvgText ***** */

SvgText :: 
SvgText( double x0, double y0, const std::string &s0)
: text( s0), x( x0), y( y0), 
  fontfamily(), fontsize(), textanchor(), dominantbaseline(),
  rotateDeg(), rotateX(), rotateY() 
{}

SvgText :: 
SvgText( double x0, double y0, double v0)
: text(), x( x0), y( y0), 
  fontfamily(), fontsize(), textanchor(), dominantbaseline(),
  rotateDeg(), rotateX(), rotateY() 
{
	std::stringstream ss;
	ss << v0;
	text.assign( ss.str());
}

SvgText :: 
~SvgText( void)
{}

void 
SvgText :: 
addFontfamily( const std::string &s0)
{
	fontfamily = s0;
}	

void 
SvgText :: 
addFontsize( double v0)
{
	std::stringstream ss;
	ss << v0;
	fontsize = ss.str();
}	

void 
SvgText :: 
addTextanchor( const std::string &s0)
{
	textanchor = s0;
}	

void 
SvgText :: 
addDominantbaseline( const std::string &s0)
{
	dominantbaseline = s0;
}	

void 
SvgText :: 
addRotate( double d0, double x0, double y0)
{
	{
		std::stringstream ss;
		ss << d0;
		rotateDeg = ss.str();
	}
	{
		std::stringstream ss;
		ss << x0;
		rotateX = ss.str();
	}
	{
		std::stringstream ss;
		ss << y0;
		rotateY = ss.str();
	}

}	

std::string 
SvgText :: 
getContent( void) const
{

	std::stringstream ss;

	ss << "<text ";

	ss << R"(x=")" << x << R"(")" << " "
	   << R"(y=")" << y << R"(")" << " ";
	
	if ( fontfamily.size() > 0){
		ss << R"(font-family=")" << fontfamily << R"(")" << " ";
	}

	if ( fontsize.size() > 0){
		ss << R"(font-size=")" << fontsize << R"(")" << " ";
	}

	if ( textanchor.size() > 0){
		ss << R"(text-anchor=")" << textanchor << R"(")" << " ";
	}

	if ( dominantbaseline.size() > 0){
		ss << R"(dominant-baseline=")" << dominantbaseline << R"(")" << " ";
	}

	if ( rotateDeg.size() > 0){
		ss << R"(transform="rotate)" << 
				"(" << rotateDeg << " " << rotateX << " " << rotateY << ")"
		   << R"(")";
	}

	ss << ">";

	ss << text;

	ss << "</text>";

	return ss.str();

}


#endif // ksvg_cpp_include_guard
