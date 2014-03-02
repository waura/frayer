#ifndef _IMGFILEHANDLEMG_H_
#define _IMGFILEHANDLEMG_H_

#include "LibIEDllHeader.h"

class ImgFile;
class ImgFileHandle;

enum {
	IFH_NULL,
	IFH_STACK,
	IFH_ADD_COPY_LAYER,
	IFH_ADD_NEW_LAYER,
	IFH_ADD_NEW_LAYER_GROUP,
	IFH_ADD_NEW_MASK,
	IFH_CHANGE_CANVAS_SIZE,
	IFH_CHANGE_LAYER_LINE,
	IFH_CHANGE_LAYER_OPACITY,
	IFH_CHANGE_LAYER_SYNTH_FOURCC,
	IFH_CHANGE_RESOLUTION,
	IFH_EDIT_LAYER,
	IFH_EDIT_MASK,
	IFH_MERGE_LAYER,
	IFH_MOVE_LAYER,
	IFH_REMOVE_LAYER,
	IFH_SET_LAYER_NAME,
};

class _EXPORTCPP ImgFileHandleMG
{
public:
	/////////////////////////////////////
	/*!
		create fild handle instance
		@param[in] pFile 
		@param[in] id set handle type id
	*/
	static ImgFileHandle* CreateImgFileHandle(ImgFile_Ptr pFile, int id);

	/////////////////////////////////////
	/*!
		destory handle object
		@param[in] ppHandle destory object
	*/
	static void ReleaseImgFileHandle(ImgFileHandle* ppHandle);
};


#endif //_IMGFILEHANDLEMG_H_