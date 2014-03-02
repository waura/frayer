#ifndef _IIETOOL_H_
#define _IIETOOL_H_

#include "LibIEDllHeader.h"

#include "IEModules.h"
#include "IIEGuiFrame.h"
#include "IToolEditWndHandle.h"

class ImgEdit;

class _EXPORTCPP IIETool : public IIEGuiFrame
{
public:
	IIETool();
	virtual ~IIETool();

	/////////////////////////////////
	/*!
		ツールウィンドウに表示するボタン画像を返す
		@return HBITMAP画像
	*/
	virtual HBITMAP GetButtonImg(){
		return m_buttonImg;
	}

	//////////////////////////////////
	/*!
		選択されたときに呼び出し
	*/
	virtual void OnSelect(){};

	/////////////////////////////////
	/*!
		選択から外れるときに呼び出し
	*/
	virtual void OnNeutral(){};

	/////////////////////////////////
	/*!
		この関数の中でツールエディットウィンドウにコントロールを追加する
		@param[in] pHandle
	*/
	virtual void OnSetToolEditWnd(IToolEditWndHandle* pHandle){};

	/////////////////////////////////
	/*!
	*/
	virtual void SetImgEdit(ImgEdit* pImgEdit){ m_pImgEdit = pImgEdit; };

	//////////////////////////////
	/*!
		set tool dll handle
	*/
	inline void SetDllHandle(HANDLE hModule){
		m_hModule = hModule;
	}

protected:

	HBITMAP m_buttonImg;
	ImgEdit* m_pImgEdit;

private:
	HANDLE m_hModule;
};


#ifdef __cplusplus
#define IETOOL_EXPORT extern "C" __declspec (dllexport)
#else
#define IETOOL_EXPORT __declspec (dllexport)
#endif

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID();

//IETOOL_EXPORT void GetIEToolName(char* dst);
//IETOOL_EXPORT void GetIEToolDisplayNameJa(char* dst);

IETOOL_EXPORT IIETool* CreateIETool();
IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool);

#endif //_IIETOOL_H_