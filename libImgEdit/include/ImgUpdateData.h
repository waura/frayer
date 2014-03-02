#ifndef _IMGUPDATEDATA_H_
#define _IMGUPDATEDATA_H_

#include <windows.h>
#include <list>

//
enum UPDATE_FLAG{
	NO_UPDATE = 0,
	UPDATE_LAYER_IMAGE = 1,			//レイヤー画像の更新
	UPDATE_MASK_IMAGE = 3,			//
	UPDATE_DISPLAY_IMAGE = 4,		//表示画面の更新
};

//イメージ再生成データ
typedef struct _UPDATE_DATA
{
	RECT rect;
	UPDATE_FLAG update_flag;
	bool isAll;
} UPDATE_DATA;

typedef UPDATE_DATA* LPUPDATE_DATA;
typedef std::list<LPUPDATE_DATA>  LPUpdateData_List;


#endif //_IMGUPDATEDATA_H_