#ifndef _STACK_HANDLE_H_
#define _STACK_HANDLE_H_

#include "ImgFileHandle.h"

#include <vector>

typedef std::vector<ImgFileHandle*> ImgFileHandle_Vec;

class _EXPORTCPP StackHandle : public ImgFileHandle
{
public:
	StackHandle(){
	}
	~StackHandle();

	inline void SetHandleName(const char* src)
	{
		strncpy(handle_name, src, MAX_IMG_FILE_HANDLE_NAME);
	}

	void Push(ImgFileHandle* handle);

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	ImgFileHandle_Vec m_ImgFileHandle_Vec;
};

#endif //_STACK_HANDLE_H_