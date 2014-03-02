#ifndef _IEBRUSHIO_H_
#define _IEBRUSHIO_H_

#include "IEImgBrush.h"
#include "IEEMBrush.h"
#include "IEFileIOCommon.h"

typedef struct _AbrInfo AbrInfo;

struct _AbrInfo
{
	short version;
	short subversion;
	short count;
};

typedef std::vector<IEBrush_Ptr> IEBrush_Vec;

class _EXPORTCPP IEBrushIO
{
public:
	////////////////////////
	/*!
		IEBrushを独自形式で保存
		@param[in] pBrush 保存するブラシ
		@param[in] path 保存先のパス
	*/
	static void SaveIEBrush(const IEImgBrush_Ptr pBrush, const char* path);

	////////////////////////
	/*!
		独自形式ブラシファイルを読み込み
		@param[in] path 読込先のパス
		@return 読み込み、生成したブラシオブジェクトへのパス
	*/
	static IEBrush_Ptr LoadIEBrush(const char* path);

	////////////////////////
	/*!
		photoshop形式のブラシファイルの読み込み
		@return 読み込んだブラシ数, 読み込み失敗時は0
	*/
	static int StartReadAbrBrush(const char* path);

	//////////////////////////
	/*!
		IEBrushIOが保持しているブラシを返す。
		@return ブラシオブジェクトのポインタ
	*/
	static IEBrush_Ptr GetBrush();

	//////////////////////////
	/*!
		読み込み処理の終了。
	*/
	static void EndReadAbrBrush();
private:

	/////////////////////////
	/*!
		ブラシファイルのヘッダー部分を読み込み
	*/
	static bool ReadAbrInfo(FILE* abr, AbrInfo* abr_hdr);

	//////////////////////////
	/*!
		ver6の場合のブラシサンプル数の検出
	*/
	static int FindSampleCountV6(FILE* abr, AbrInfo * abr_hdr);

	//////////////////////////
	/*!
		8BIMタグ名がnameと等しいかどうか
	*/
	static bool ReachAbr8BIMSection(FILE *abr, const char* name);

	/////////////////////////
	/*!
		ブラシのバージョンが読み込み可能なものか？
	*/
	static bool isSupportedAbrHdr(const AbrInfo *abr_hdr);

	/////////////////////////
	/*!
		ブラシの読み込み
	*/
	static bool AbrBrushLoad(FILE *abr, const AbrInfo *abr_hdr, const char* filename, IEBrush_Ptr pBrush, int id);

	//////////////////////////
	/*!
		ver1 ver2 のブラシの読み込み
	*/
	static bool AbrBrushLoad_v12(FILE *abr, const AbrInfo *abr_hdr, const char *filename, IEBrush_Ptr pBrush, int id);

	////////////////////////////
	/*!
		ver6　のブラシの読み込み
	*/
	static bool AbrBrushLoad_v6(FILE *abr, const AbrInfo *abr_hdr, const char *filename, IEBrush_Ptr pBrush, int id);

	static void AbrBrushNormalLoad(FILE *abr, int width, int height, short depth, IEBrush_Ptr pBrush);
	static void AbrBrushDecodeLoad(FILE *abr, int widht, int height, short depth, IEBrush_Ptr pBrush);

	static char* abr_v1_brush_name(const char* filename, int id);
	static char* abr_read_ucs2_text(FILE *abr);


	static bool m_isReading;
	static IEBrush_Vec m_ieBrush_Vec;
};

#endif