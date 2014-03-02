#include "stdafx.h"

#include "StackHandle.h"
#include "ImgFileHandleMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


StackHandle::~StackHandle()
{
	ImgFileHandle_Vec::reverse_iterator ritr = m_ImgFileHandle_Vec.rbegin();
	for (;ritr != m_ImgFileHandle_Vec.rend(); ritr++) {
		ImgFileHandleMG::ReleaseImgFileHandle(*ritr);
	}
	m_ImgFileHandle_Vec.clear();
}

void StackHandle::Push(ImgFileHandle* handle)
{
	m_ImgFileHandle_Vec.push_back(handle);
}

bool StackHandle::Do(ImgFile_weakPtr wpFile)
{
	bool bl = true;;
	ImgFileHandle_Vec::iterator itr = m_ImgFileHandle_Vec.begin();
	for (;itr != m_ImgFileHandle_Vec.end(); itr++) {
		if ((*itr)->Do(wpFile) == false) {
			bl = false;
		}
	}
	return bl;
}

void StackHandle::Redo(ImgFile_weakPtr wpFile)
{
	ImgFileHandle_Vec::iterator itr = m_ImgFileHandle_Vec.begin();
	for (;itr != m_ImgFileHandle_Vec.end(); itr++) {
		(*itr)->Redo(wpFile);
	}
}

void StackHandle::Undo(ImgFile_weakPtr wpFile)
{
	ImgFileHandle_Vec::reverse_iterator ritr = m_ImgFileHandle_Vec.rbegin();
	for (; ritr != m_ImgFileHandle_Vec.rend(); ritr++) {
		(*ritr)->Undo(wpFile);
	}
}