#ifndef _IMGSELECTER_H_
#define _IMGSELECTER_H_

#include "IIETool.h"
#include <libImgEdit.h>


class IImgSelecter :
	public IIETool,
	public ImgEditEventListener
{
public:	
	IImgSelecter(){}
	virtual ~IImgSelecter() {
		if (m_pImgEdit) {
			m_pImgEdit->DeleteEventListener(this);
		}
	}

	virtual void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
	{
		if (pOldFile) {
			pOldFile->SetMaskDrawType( MASK_DOT_LINE_DRAW );
		}
		if (pNewFile) {
			OnSelect();
		}
	}

	//マウス座標系は画像座標
	virtual IE_INPUT_DATA_FLAG GetInputDataFlag(){
		return IE_INPUT_DATA_FLAG::IMAGE_POS;
	}

	virtual void OnDraw(HDC hdc){};

	virtual void OnSelect();
	virtual void OnNeutral();

	virtual void SetImgEdit(ImgEdit* pImgEdit)
	{
		if (m_pImgEdit) {
			m_pImgEdit->DeleteEventListener(this);
		}
		if (pImgEdit) {
			pImgEdit->AddEventListener(this);
		}
		IIETool::SetImgEdit(pImgEdit);
	}

	virtual void OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt);
};

#endif