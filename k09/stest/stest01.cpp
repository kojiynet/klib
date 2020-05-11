
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
#include <k09/ksvg00.cpp>
#include <k09/krand00.cpp>
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

SvgGraph calculatePi( double);

SvgGraph createScatterAndCircle(
	const std::vector <double> &xvec,
	const std::vector <double> &yvec
);


/* ********** Class Definitions ********** */


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


	// ヒストグラムをつくる。

	vector <int> codes;
	vector <int> counts;
	vector <double> leftvec;
	vector <double> rightvec;
	ft.getVectors( codes, counts);
	ft.getRangeVectors( leftvec, rightvec);

	{
		SvgHistogramMaker histm( leftvec, rightvec, counts);
		histm.setGraphTitle( "Frequency - restricted to v less than 2500");
		histm.setXAxisTitle( "Household Income");
		histm.setYAxisTitle( "#Cases");
		SvgGraph svgg = histm.createGraph();
		svgg.writeFile( "stest01out01.svg");
	}
	{
		SvgHistogramMaker histm( leftvec, rightvec, counts, true); // アニメバージョン
		histm.setGraphTitle( "Frequency - restricted to v less than 2500");
		histm.setXAxisTitle( "Household Income");
		histm.setYAxisTitle( "#Cases");
		SvgGraph svgg = histm.createGraph();
		svgg.writeFile( "stest01out02.svg");
	}

	{
		SvgGraph svgg = calculatePi( 4000);
		svgg.writeFile( "stest01out03.svg");
	}

	// pclub06の内容を回収しつつ。

	// SvgGraphの中で、GraphPane内の座標を自動変換できるように。。
	// （それを目的としていたのに忘れていた）
	// 散布図用の、赤い楕円。

	


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

SvgGraph calculatePi( double n)
{

	std::vector <double> xvec;
	std::vector <double> yvec;
	xvec.reserve( n);
	yvec.reserve( n);

	for ( int i = 0; i < n; i++) {
    	xvec.push_back( randomUniform( -1.0, 1.0));
    	yvec.push_back( randomUniform( -1.0, 1.0));
	}	

	int ninside = 0;
	for ( int i = 0; i < n; i++){
		if ( xvec[ i] * xvec[ i] + yvec[ i] * yvec[ i] < 1.0){
			ninside++;
		}
	}
	std::cout << "N of Draws: " << n << std::endl;
	std::cout << "Estimated PI: " << ( ( double)ninside / n ) * 4.0 << std::endl << std::endl;	

	SvgGraph svgg = createScatterAndCircle( xvec, yvec);
	
	return svgg;
	
}

SvgGraph createScatterAndCircle(
	const std::vector <double> &xvec,
	const std::vector <double> &yvec
)
{

	std::string graph_title = "Random Numbers and Circle";
	std::string xaxis_title = "x";
	std::string yaxis_title = "y";

	// SVG領域の大きさと、座標系のある領域の大きさを指定することで、それらしく計算してほしい。
	double svgwidth = 500;
	double svgheight = 500;
	double outermargin = 20;
	double graph_title_fontsize = 20;
	double graph_title_margin = 10; // グラフタ\イトルの下のマージン
	double axis_title_fontsize = 20;
	double axis_title_margin = 5; // 軸タイトルと軸ラベルの間のマージン
	double axis_label_fontsize = 14;
	double axis_ticklength = 5;

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
	double xminval = *( min_element( xvec.begin(), xvec.end()));
	double xmaxval = *( max_element( xvec.begin(), xvec.end()));
	std::vector <double> xgridpoints = getGridPoints( xminval, xmaxval);

	// y軸
	double yminval = *( min_element( yvec.begin(), yvec.end()));
	double ymaxval = *( max_element( yvec.begin(), yvec.end()));
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

	svgg.drawXGridLines( xgridpoints, "silver");
	svgg.drawYGridLines( ygridpoints, "silver");

	// 背景の描画終了



	// メインの情報の描画終了

	svgg.drawPoints( xvec, yvec, "gray");

	svgg.drawCircle( 0, 0, 1, "red");

	// メインの情報の描画終了


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


	// 枠線
	svgg.drawGraphPaneFrame( "black");
	

	return svgg;

}




/* ********** Definitions of Member Functions ********** */


