#ifndef _ADDNEWMASKHANDLE_H_
#define _ADDNEWMASKHANDLE_H_

#include "ImgFileHandle.h"


class _EXPORTCPP AddNewMaskHandle : public ImgFileHandle
{
public:
	AddNewMaskHandle();
	~AddNewMaskHandle();

	void SetAddToIndex(int index){
		m_new_mask_index = index;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	int m_new_mask_index;
	RECT m_mask_rect;

	bool m_isAddedMask;
	ImgMask_weakPtr m_addMask;
	ImgMask_Ptr m_addMask_lock;
};

#endif //_ADDNEWMASKHANDLE_H_