#ifndef _POLYGONSELECTER_BASE_H_
#define _POLYGONSELECTER_BASE_H_


#include "IImgSelecter.h"

typedef std::vector<CvLine2D64f> CvLine_Vec;

class PolygonSelecter_Base : public IImgSelecter
{
public:
	PolygonSelecter_Base(){};
	virtual ~PolygonSelecter_Base(){};

	//マウス座標はウィンドウクライアント座標家系
	virtual IE_INPUT_DATA_FLAG GetInputDataFlag(){
		return IE_INPUT_DATA_FLAG::WINDOW_POS;
	}

	virtual void OnDraw(HDC hdc);

protected:

	//////////////////////////
	/*!
		m_CvLine_Vecに格納されている線分の最大範囲を求める
		@param[out] lprect
	*/
	void GetLinesRect(LPRECT lprect);


	//////////////////////////
	/*!
		保持している線分から選択領域の作成
		@param[in] handle_name 更新するハンドルの名前
		@param[in] lprect 更新する範囲
	*/
	void CreateMask(const char* handle_name, const LPRECT lprect);

	CvLine_Vec m_CvLine_Vec;	
	bool m_isFirst;
	bool m_isLineVecLock;
	CvPoint2D64f m_beforePt;
	CvPoint2D64f m_nowPt;

private:
	ImgMask* m_pEditMask;
};

#endif