
/*
	koutputfile.cpp
	Ver. k09.01
	
	ファイル書き込みクラス
	
	TODO: datasetを書き出す関数を書いて、旧バージョンを消す。
	
*/


/* ********** プリプロセッサ指令 ********** */

#ifndef koutputfile_cpp_include_guard
#define koutputfile_cpp_include_guard

#include <fstream>
#include <k09/kutil01.cpp>
#include <k09/kfileboost00.cpp>

//using namespace std;


/* ********** クラス宣言 ********** */

class koutputfile;


/* ********** 関数宣言 ********** */


/* ********** クラス定義 ********** */

class koutputfile {
	
	private:
		
		std::ofstream ofs;
		std::string fn;
		
	public:
		
		koutputfile( void);
		koutputfile( const std::string &);
		~koutputfile( void);
		
		void setFileName( const std::string &);
		bool open( bool = true, bool = false, bool = true);
		void close( void);
		bool writeLine( const std::string &);
		bool writeLines( const std::vector <std::string> &);

		std::ofstream & stream( void);
		
/*
		void writeDataset( kdataset &kd, const char *sep);
		void writeDataset( kdataset &kd, const char *sep, int k, int el);
		void clearStream( void);
		void writeStream( void);
*/
		
};


/* ********** グローバル変数 ********** */


/* ********** 関数定義 ********** */


/* ********** クラスメンバ関数 ********** */

koutputfile :: koutputfile( void) : ofs(), fn()
{}

// sをファイル名とする。
koutputfile :: koutputfile( const std::string &s) : ofs()
{
	
	setFileName( s);
	
}

koutputfile :: ~koutputfile( void)
{
	
	close();
	
}

void koutputfile :: setFileName( const std::string &s)
{
	
	fn.assign( s);
	
}

// 書き込み用ファイルを開く。
// appendのデフォルト値はtrue、overwriteのデフォルト値はfalse、askのデフォルト値はtrue。
// 同じファイル名のファイルがすでに存在する場合の挙動は以下のとおり。
// appendがtrueなら、追加書き込みモードで開く。overwriteとaskは無視する。
// appendがfalseで、overwriteがtrueなら、上書きする。
// appendがfalseで、overwriteがfalseで、askがtrueなら、上書きするかどうかをコンソールで尋ねる。
// appendがfalseで、overwriteがfalseで、askがfalseなら、ファイルを開かない。
// 正常終了でtrue、異常終了でfalseを返す。
bool koutputfile :: open( bool append, bool overwrite, bool ask)
{
	
	std::ios_base :: openmode om;
	
	// このオブジェクトで前に別のファイルをopenしたことがある場合に備えて、clear()する。
	ofs.clear();
	
	if ( fn.size() < 1){
		alert( "koutputfile :: open()", "File name is not given.");
		return false;
	}
	
	om = std::ios_base :: out;
	
	if ( fileExists( fn.c_str()) == true){
		if ( append == true){
			om = om | ( std::ios_base :: app);
		} else {
			if ( overwrite == true){
				// 上書きする。
			} else {
				if ( ask == true){
					bool b;
					b = alertYesNo( "koutputfile :: open()", "The file \"" + fn + "\" already exists. Do you want to overwrite?");
					if ( b == true){
						// 上書きする。
					} else {
						alert( "koutputfile :: open()", "The file will not be overwritten.");
						return false;
					}
				} else {
					alert( "koutputfile :: open()", "The file already exists.");
					return false;
				}
			}
		}
	}
	
	ofs.open( fn.c_str(), om);
	
	if ( ofs.good() == false){
		alert( "koutputfile :: open()", "The file can not be opened.");
		ofs.close();
		return false;
	}
	
	return true;
	
}

void koutputfile :: close( void)
{
	
	ofs.close();
	
}

// sを改行付きで書き出す。
// 正常に書き込まれた場合にtrueを返す。
bool koutputfile :: writeLine( const std::string &s)
{
	
	if ( ofs.is_open() == false || ofs.good() == false){
		alert( "koutputfile :: writeLine()", "The file status is not good, so writing was skipped.");
		return false;
	}
	
	ofs << s << std::endl;
	
	if ( ofs.is_open() == false || ofs.good() == false){
		alert( "koutputfile :: writeLine()", "The program tried to write, but the file status got not good.");
		return false;
	}
	
	return true;
	
}

// svに含まれるstringを順番に改行付きで書き出す。
// 正常に書き込まれた場合にtrueを返す。
bool koutputfile :: writeLines( const std::vector <std::string> &sv)
{
	
	int n;
	
	if ( ofs.is_open() == false || ofs.good() == false){
		alert( "koutputfile :: writeLines()", "The file status is not good, so writing was skipped.");
		return false;
	}
	
	n = sv.size();
	for ( int i = 0; i < n; i++){
		ofs << sv[ i] << std::endl;
	}
	
	if ( ofs.is_open() == false || ofs.good() == false){
		alert( "koutputfile :: writeLine()", "The program tried to write, but the file status got not good.");
		return false;
	}
	
	return true;
	
}

// 内部のofstreamのオブジェクトを返す。
std::ofstream &
koutputfile ::
stream( void)
{

	return ofs;

}


/*

以下、消す候補


// データセットkdを区切り文字sepで単純にファイルに書き出す。
void koutputfile :: writeDataset( kdataset &kd, const char *sep)
{
	
	ostringstream oss;
	
	if ( f == NULL){
		alert( "koutputfile :: writeDataset()");
		return;
	}
	
	oss.str( "");
	oss.clear();
	
	kd.print( oss, string( sep));
	
	fputs( oss.str().c_str(), f);
	
}

// データセットkdを区切り文字sepで、精度を指定してファイルに書き出す。
// 精度を示すk、elについてはktools.cppのdoubleToString()参照。
void koutputfile :: writeDataset( kdataset &kd, const char *sep, int k, int el)
{
	
	ostringstream oss;
	
	if ( f == NULL){
		alert( "koutputfile :: writeDataset()");
		return;
	}
	
	oss.str( "");
	oss.clear();
	
	kd.print( oss, string( sep), k, el);
	
	fputs( oss.str().c_str(), f);
	
}

// streamをクリアする。
void koutputfile :: clearStream( void)
{
	
	stream.str( "");
	stream.clear();
	
}

// streamをファイルに書き込む。同時にstreamをクリアする。
void koutputfile :: writeStream( void)
{
	
	write( stream.str());
	clearStream();
	
}

*/

#endif /* koutputfile_cpp_include_guard */
