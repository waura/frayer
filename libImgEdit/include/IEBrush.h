#ifndef _IEBRUSH_H_
#define _IEBRUSH_H_

#include "LibIEDllHeader.h"
#include "InputData.h"
#include "IEBrushEventListener.h"

#include <list>
#include <map>

typedef std::list<IEBrushEventListener* > IEBrushEventListener_List;
typedef std::map<int, IplImage*> IEBrushImgBuffer_Map;

#define _MAX_BRUSH_IMG_BUF_SIZE_ 1000000

typedef struct _BRUSH_CTRL {
	double rad;
	double alpha;
	double interval;
	double center_x;
	double center_y;
} BRUSH_CTRL, *LPBRUSH_CTRL;

enum IE_BRUSH_TYPE {
	IE_IMG_BRUSH=0,
	IE_EM_BRUSH,
};

enum IE_BRUSH_CTRL_ID {
	OFF = 0,
	PRESSURE,
	AZIMUTH,
	ALTITUDE,
	RANDOM,
	_MAX_INDEX_,
};

//
//
class _EXPORTCPP IEBrush
{
public:
	IEBrush();
	virtual ~IEBrush();

	////////////////////////////////////
	/*!
		XML形式でブラシグループのデータを書き込み
	*/
	virtual void WriteBrushToXml(FILE* fp, int indt);

	/////////////////////////////
	/*!
		イベントリスナーを登録する。
		@param[in] pListener 登録するリスナー
	*/
	void AddEventListener(IEBrushEventListener*  pListener);

	/////////////////////////////
	/*!
		イベントリスナーを削除する。
		@param[in] pListener 削除するリスナー
	*/
	void DeleteEventListener(IEBrushEventListener*  pListener);

	void ClearBrushImgBufMap();

	void ManageBufMap();

	/////////////////////////////
	/*!
		Clear Prev Draw Brush Point for Interval calc
	*/
	inline void ClearPrevDrawPt()
	{
		m_prev_draw_pt.x = -1000.0;
		m_prev_draw_pt.y = -1000.0;
	}

	/////////////////////////////
	/*!
		ブラシデータを作成する。
	*/
	virtual void CreateBrush(const uchar* buffer, int width, int height, int depth) = 0;

	///////////////////////////////
	///*!
	//	ブラシデータをdstに転送する
	//	@param[in,out] dst 1ch image
	//	@param[in] center_lpd center input data
	//	@param[out] blted_rc
	//	@return
	//*/
	//template <class _CBltOp>
	//virtual void BltBrushData(const _CBltOp& bltOp, IplImageExt* dst, const LPIE_INPUT_DATA center_lpd, LPRECT blted_rc) = 0;

	/////////////////////////////
	/*!
		サイズ(width, height)のサンプル画像を生成して返す。
		@return 作成したIplImageを返す
	*/
	virtual IplImage* CreateBrushSampleImg(int width, int height) = 0;

	virtual void ReleaseBrushSampleImg(IplImage** img);

	/////////////////////////////
	/*!
		ブラシプレビューをdstに描画する
		@param[in] dst
		@return
	*/
	virtual void DrawBrushPreview(IplImage* dst) = 0;

	//////////////////////////////
	/*!
		ブラシ元画像の直径
	*/
	void SetOrgRadius(int r);
	inline int GetOrgRadius() const{
		return m_orgRadius;
	}

	//////////////////////////////
	/*!
		ブラシの有効範囲直径をセットする。
	*/
	virtual void SetRadius(int r);

	//////////////////////////////
	/*!	
		ブラシの有効範囲直径を返す。
	*/
	inline int GetRadius() const {
		return m_radius;
	}

	//////////////////////////////
	/*!	
		直径のコントロールを指定する
	*/
	void SetRadiusCtrlID(IE_BRUSH_CTRL_ID id);

	inline IE_BRUSH_CTRL_ID GetRadiusCtrlID() {
		return m_radius_ctrl_id;
	}

	void SetAlphaCtrlID(IE_BRUSH_CTRL_ID id);

	inline IE_BRUSH_CTRL_ID GetAlphaCtrlID() {
		return m_alpha_ctrl_id;
	}

	//////////////////////////////
	/*!
		minimum brush raqdius(0~100)
	*/
	inline int GetMinRadius() const{
		return m_min_radius_per*100;
	}
	void SetMinRadius(int percent);

	////////////////////////////////
	/*!
		minimum brush alpha(0~100)
	*/
	inline int GetMinAlpha() const {
		return m_min_alpha_per * 100;
	}
	void SetMinAlpha(int percent);

	/////////////////////////////////
	/*!
		ブラシ間隔を返す。
	*/
	inline double GetStepSize(const LPIE_INPUT_DATA lpd) {
		BRUSH_CTRL brush_ctrl;
		GetBrushCtrl(&brush_ctrl, lpd);
		double d = brush_ctrl.interval;
		return (d > 1.0) ? d : 1.0;
	}

	inline int GetInterval() {
		return m_interval * 100;
	}
	void SetInterval(int interval);

	////////////////////////////////
	inline bool IsValidTexture() {
		return (m_texture != NULL);
	}
	inline void GetTexturePath(char* dst, size_t max_size) {
		strcpy_s(dst, max_size, m_texture_path.c_str());
	}
	bool AddTexture(const char* path);
	void DeleteTexture();

	////////////////////////////////
	/*!
		ブラシの種類のIDを返す
	*/
	inline int GetBrushType() {
		return m_brush_type_id;
	}

	//////////////////////////////
	/*!
		brush name
	*/
	inline const char* GetName() const {
		return m_name;
	}
	inline void GetName(char* dst) const {
		strcpy_s(dst, MAX_IE_BRUSH_NAME, m_name);
	}
	inline void SetName(const char* src){
		strcpy_s(m_name, MAX_IE_BRUSH_NAME, src);
	}

	//
	enum {
		MIN_RADIUS = 1,
		MAX_RADIUS = 1250,
		MIN_INTERVAL = 1,
		MAX_INTERVAL = 800,
	};

protected:
	inline void AddBrushToTable(int d, IplImage* img)
	{
		ManageBufMap();
		m_brushImgBuf_Map.insert(IEBrushImgBuffer_Map::value_type(d, img));
		m_brush_buf_map_size += img->width*img->height;
	}

	inline IplImage* GetBrushFromTable(int d)
	{
		IEBrushImgBuffer_Map::iterator itr_m = m_brushImgBuf_Map.find( d );
		if(itr_m != m_brushImgBuf_Map.end()){
			return (*itr_m).second;
		}
		return NULL;
	}

	inline void GetBrushCtrl(LPBRUSH_CTRL brush_ctrl, const LPIE_INPUT_DATA lpd)
	{
		//radius
		double ret_brush_rad =  m_radius * GetCtrlValue(m_radius_ctrl_id, lpd);
		double min_ret_brush_rad = m_min_radius_per * m_radius;
		if (ret_brush_rad < min_ret_brush_rad) {
			ret_brush_rad = min_ret_brush_rad;
		}
		brush_ctrl->rad = ret_brush_rad;

		//alpha
		double alpha_per = GetCtrlValue(m_alpha_ctrl_id, lpd);
		if (alpha_per < m_min_alpha_per) {
			brush_ctrl->alpha = m_min_alpha_per;
		} else {
			brush_ctrl->alpha = alpha_per;
		}

		brush_ctrl->interval = ret_brush_rad * m_interval;

		brush_ctrl->center_x = lpd->x;
		brush_ctrl->center_y = lpd->y;
	}

	inline double GetCtrlValue(int id, const LPIE_INPUT_DATA lpd)
	{
		switch (id) {
		case IE_BRUSH_CTRL_ID::OFF:
			return 1.0;
			break;
		case IE_BRUSH_CTRL_ID::PRESSURE:
			return lpd->p;
			break;
		case IE_BRUSH_CTRL_ID::AZIMUTH:
			return lpd->azimuth_rad / (2.0 * CV_PI);
			break;
		case IE_BRUSH_CTRL_ID::ALTITUDE:
			return (lpd->altitude_rad + CV_PI) / (2.0 * CV_PI);
			break;
		case IE_BRUSH_CTRL_ID::RANDOM:
			return lpd->random;
			break;
		default:
			assert(0);
			break;
		}
	}
	
	int m_orgRadius;			//ブラシ元データ直径
	int m_radius;				//ブラシ半径
	double m_min_radius_per;	//ブラシ最小半径(0.0~1.0)
	double m_min_alpha_per;		//ブラシ最小濃度(0.0~1.0)
	double m_interval;			//ブラシ間隔(0.0~1.0)

	std::string m_texture_path;
	IplImage* m_texture;		//

	int m_brush_type_id;

	CvPoint2D64f m_prev_draw_pt;

	IEBrushEventListener_List m_IEBrushEventListener_List;

private:

	IE_BRUSH_CTRL_ID m_radius_ctrl_id;
	IE_BRUSH_CTRL_ID m_alpha_ctrl_id;

	int m_brush_buf_map_size;
	char m_name[MAX_IE_BRUSH_NAME];
	IEBrushImgBuffer_Map m_brushImgBuf_Map;
};

//_EXPORT void ReleaseIEBrush(IEBrush_Ptr* ppBrush);

#endif