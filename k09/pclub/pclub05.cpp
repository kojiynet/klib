
/*
	
	pclub05.cpp
	
	pclub01.cppから派生。
	
	Written by Koji Yamamoto
	Copyright (C) 2019-2020 Koji Yamamoto
	
	番外編。SVGを複数つくり、pngにして、mp4とmovにする。
	2020年4月11日時点のpclub04.cppの内容を流用。
	
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

	// ファイルに書くメソッドも。。

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


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int, char *[])
{
	
	vector <string> batchlines;

	int i = 0;
	double deg = 0.0;
	for ( ; deg <= 360.0; i++, deg += 1.0){
		
		SvgFile svgf( 500, 500, 0, 0, 500, 500);

		SvgRect r0( 0, 0, 500, 500);
		r0.addStrokewidth( 0);
		r0.addFill( "white"); // 動画にするときに明示的に白にしておく必要がある。
		svgf.addRect( r0);

		SvgText t0( 250, 250, "moji"); // vはstringに変換される。
		t0.addFontfamily( "Arial,san-serif");
		t0.addFontsize( 200);
		t0.addTextanchor( "middle"); 
		t0.addDominantbaseline( "alphabetic"); 
		t0.addRotate( deg, 250, 250); // 回転させる。
		svgf.addText( t0);		

		svgf.addFileContent( R"(</svg>)");
		
		string detector = "<!-- " 
						u8"\u6587\u5B57\u30B3\u30FC\u30C9\u8B58\u5225\u7528" // 「文字コード識別用」というUTF-8文字列
						" -->";
		svgf.addFileContent( detector);

		vector <string> svglines = svgf.getFileContent();

		string svgfn;
		{
			stringstream ss;
			ss << "pclub05out" << setw( 3) << setfill( '0') << i << ".svg";
			svgfn = ss.str();
		}

		koutputfile outsvg( svgfn);
		outsvg.open( false, false, true);
		outsvg.writeLines( svglines);
		outsvg.close();

		// SVG -> PNG using Inkscape

		// command:
		// $ Inkscape.com --export-width=500 --file=XXX.svg --export-png=XXX.png

		string pngfn;
		{
			stringstream ss;
			ss << "pclub05out" << setw( 3) << setfill( '0') << i << ".png";
			pngfn = ss.str();
		}

		string syscommand;
		{
			stringstream ss;
			ss << "Inkscape.com --export-width=500 --file=" << svgfn << " --export-png=" << pngfn;
			syscommand = ss.str();
		}
		batchlines.push_back( syscommand);
//		system( syscommand.c_str());

	}

	koutputfile outsvg( "pclub05bat1.bat");
	outsvg.open( false, false, true);
	outsvg.writeLines( batchlines);
	outsvg.close();

	return 0;

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

