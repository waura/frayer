#ifndef _CHANGE_CANVAS_SIZE_HANDLE_H_
#define _CHANGE_CANVAS_SIZE_HANDLE_H_

#include "ImgFileHandle.h"

#include "ImgFile.h"

class _EXPORTCPP ChangeCanvasSizeHandle : public ImgFileHandle
{
public:
	ChangeCanvasSizeHandle(){
		m_toSize.width = 0;
		m_toSize.height = 0;
	}
	~ChangeCanvasSizeHandle(){
	}

	inline void SetNewCanvasSize(CvSize size){
		m_toSize = size;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	CvSize m_toSize;
	CvSize m_fromSize;
	ImgFile_weakPtr m_pEditFile;
};

#endif //_CHANGE_CANVAS_SIZE_HANDLE_H_