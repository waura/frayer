#ifndef _OUTPUTERROR_H_
#define _OUTPUTERROR_H_

#include "LibIEDllHeader.h"

#include <string>
#include <list>

//#ifdef _DEBUG
//	#define PushLog(log_level, str) PushLog(__FILE__, __LINE__, log_level, str)
//#endif

typedef std::list<std::string> LogList;

enum LOG_LEVEL{
	_INFO,
	_WARN,
	_ERROR,
	_FATAL,
};

#define IE_ERROR_DOESNT_SUPPORT_SSE2			TEXT("SSE2対応のCPUではありません")
#define IE_ERROR_CONFIG_FILE_DONT_EXIST			TEXT("設定ファイルが見つかりません")
#define IE_ERROR_BRUSH_CONFIG_FILE_DONT_EXIST	TEXT("ブラシ設定ファイルが見つかりません")
#define IE_ERROR_LOAD_FILED_BRUSH_CONFIG_FILE	TEXT("ブラシ設定ファイルの読み込みに失敗しました")
#define IE_ERROR_BRUSH_CONFIG_FILE_CANT_WRITE	TEXT("ブラシ設定ファイルの書き込みに失敗しました")
#define IE_ERROR_TOOL_CONFIG_FILE_CANT_WRITE	TEXT("ツール設定ファイルの書き込みに失敗しました")
#define IE_ERROR_EFFECT_CONFIG_FILE_CANT_WRITE	TEXT("エフェクト設定ファイルの書き込みに失敗しました")
#define IE_ERROR_PALETTE_CONFIG_FILE_CANT_WRITE	TEXT("パレット設定ファイルの書き込みに失敗しました")
#define IE_ERROR_LAYER_INVISIBLE				TEXT("レイヤーが表示されていません")
#define IE_ERROR_SELECT_LAYER_DONT_EXIST		TEXT("選択レイヤーがありません")
#define IE_ERROR_CANT_EDIT_LAYER				TEXT("編集可能なレイヤーではありません")
#define IE_ERROR_SELECT_BRUSH_DONT_EXIST		TEXT("選択ブラシがありません")
#define IE_ERROR_CANT_LOAD_BRUSH_TEXTURE		TEXT("ブラシテクスチャが読み込めませんでした")
#define IE_ERROR_INVALID_BRUSH_TEXTURE			TEXT("不正なブラシテクスチャです。ブラシテクスチャは256x256、512x512、1024x1024のいずれかのサイズを使用してください")


class  _EXPORTCPP OutputError{
public:
	OutputError(){};
	~OutputError(){};

	static void Init(const char* exePath, const char* logDirPath, LOG_LEVEL log_level);
	static void End();

	static void PushLog(LOG_LEVEL log_level, const char* str);
	static void PushLog(LOG_LEVEL log_level, const char* str, const char* v1);
	static void PushLog(LOG_LEVEL log_level, const char* str, int val);
	//static void PushLog(const char* file, int line, LOG_LEVEL log_level, const char* str);
	//static void FlushLog();
	static void Alert(const char* str);
	static void Alert(const char* str, const char* v1);
private:
	static void SetLogLevel(LOG_LEVEL log_level);

	static bool m_IsInited;
	static int m_LogBufSize;
	static LOG_LEVEL m_LogLevel;
	static std::string m_LogDirPath;
	static LogList m_LogList;
};

#endif