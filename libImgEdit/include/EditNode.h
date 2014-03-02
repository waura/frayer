#ifndef _EDIT_NODE_H_
#define _EDIT_NODE_H_

#include "LibIEDllHeader.h"


#include <list>

enum NODE_FLAG{
	EDIT_DRAW = 1,
	MOVE_IMG,
};

class _EXPORTCPP IEditNode
{
public:
	IEditNode(){};
	virtual ~IEditNode(){};

	NODE_FLAG node_flag;	//
};

/*!
	@brief 画像編集時、記憶
	edit_img,undo_img,write_mapは編集レイヤーと同じサイズ同じ位置に配置される。
*/
class _EXPORTCPP EditNode : public IEditNode
{
public:
	EditNode() {
		node_flag = EDIT_DRAW;
	}

	IplImageExt edit_img;		//レイヤーに加算する画像
	IplImageExt undo_img;		//元に戻すのに使用する画像
	IplImageExt write_map;		//undoを書き込んだかどうか
	RECT blt_rect;				//ノードの使用する範囲 スクリーン座標
	RECT node_rect;				//ノードの位置 スクリーン座標
	uint32_t fourcc;			//ノードの重ね方
	bool isBakedWriteMap;		//write_mapが確定したかどうか
};

class _EXPORTCPP MoveNode : public IEditNode
{
public:
	MoveNode() {
		node_flag = MOVE_IMG;
	}

	int move_x; //X軸方向移動量
	int move_y; //Y軸方向移動量
};

//
enum EDIT_FLAG{
	ADD_EDIT = 1,
	SUB_EDIT,
};

class _EXPORTCPP EditQueueNode
{
public:
	IEditNode* pNode;
	EDIT_FLAG flag;
};

typedef std::list<EditQueueNode> EditQueue;

#endif //_EDIT_NODE_H_