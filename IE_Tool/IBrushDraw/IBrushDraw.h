#ifndef _IBRUSHDRAW_H_
#define _IBRUSHDRAW_H_

#include "IIETool.h"
#include <libImgEdit.h>


class IBrushDraw : public IIETool
{
public:
	IBrushDraw(){};
	virtual ~IBrushDraw(){};

	//入力情報,補間,補正有り。
	virtual bool isRevise(){
		return true;
	}

	//マウス座標は画像座標系
	virtual IE_INPUT_DATA_FLAG GetInputDataFlag(){
		return IE_INPUT_DATA_FLAG::IMAGE_POS;
	}

	//補間点の間隔
	virtual double GetReviseStepSize(const LPIE_INPUT_DATA lpd){
		if(m_pImgEdit){
			IEBrush_Ptr pBrush = m_pImgEdit->GetSelectBrush();
			if(pBrush){
				return pBrush->GetStepSize(lpd);
			}
		}
		return 1.0;
	}

	virtual void OnDraw(HDC hdc){};

	virtual void OnSelect(){
		m_pImgEdit->SetIECursor(IE_CURSOR_ID::BRUSH_CURSOR);

		char buf[MAX_IE_PARAM_REGISTER_NAME];
		if (m_pImgEdit->ReadFromParamRegister(this, "brush_group_index", buf)) {
			int bg_index = atoi(buf);
			m_pImgEdit->SetSelectBrushGroup(bg_index);
		}
		if (m_pImgEdit->ReadFromParamRegister(this, "brush_index", buf)) {
			int b_index = atoi(buf);
			m_pImgEdit->SetSelectBrush(b_index);
		}
	}
	virtual void OnNeutral(){
		m_pImgEdit->SetIECursor(IE_CURSOR_ID::NORMAL_CURSOR);

		char buf[MAX_IE_PARAM_REGISTER_NAME];
		int bg_index = m_pImgEdit->GetSelectBrushGroupIndex();
		wsprintf(buf, "%d", bg_index);
		m_pImgEdit->WriteToParamRegister(this, "brush_group_index", buf);

		int b_index = m_pImgEdit->GetSelectBrushIndex();
		wsprintf(buf, "%d", b_index);
		m_pImgEdit->WriteToParamRegister(this, "brush_index", buf);
	}

	virtual void OnMouseRButtonDown(UINT nFlags, const LPPOINT lpt){};
	virtual void OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt){
		ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
		if(f){
			IIEBrushPopupWnd* pWnd = m_pImgEdit->GetBrushPopupWnd();
			if(pWnd){
				pWnd->SetEditBrush( m_pImgEdit->GetSelectBrush() );
				pWnd->ShowPopup();
			}
		}
	}
};

#endif //_IBRUSHDRAW_H_