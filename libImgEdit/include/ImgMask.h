#ifndef _IMGMASK_H_
#define _IMGMASK_H_

#include "LibIEDllHeader.h"

#include "IMaskSynthesizer.h"
#include "EditNode.h"
#include "ImgUpdateData.h"

class ImgFile;

namespace IE_MASK_RESOURCE_ID{
	enum {
		UPDATE_DATA_QUEUE=0x01,
		EDITNODE_QUEUE=0x02,
	};
};

namespace IE_MASK_STATE_ID{
	enum {
		VALID=0,	//mask affect edit
		INVALID=1,	//mask don't affect edit
	};
};

class _EXPORTCPP ImgMask
{
public:
	ImgMask(ImgFile_weakPtr parent_file);
	~ImgMask();

	///////////////////////////////
	/*!
		mask initialize
		@param[in] lprc layer rect
	*/
	void Init(const LPRECT lprc);
	void End();

	////////////////////////////////////
	/*!
		@param[in] enable_lprc
		@param[in] updated_lprc
	*/
	bool Update(const LPRECT enable_lprc, LPRECT updated_lprc);

	LPUPDATE_DATA CreateUpdateData();
	void PushUpdateData(LPUPDATE_DATA data);
	void ClearUpdateData();
	void LockUpdateData();
	void UnlockUpdateData();

	EditNode* CreateEditNode(const LPRECT lprc);
	void DestroyEditNode(EditNode* pEditNode);
	void PushAddEditNodeQueue(EditNode* pNode);
	void PushSubEditNodeQueue(EditNode* pNode);

	inline void SetMaskState(unsigned int state_id){
		m_MaskStateID = state_id;
	}
	inline unsigned int GetMaskState() const{
		return m_MaskStateID;
	}

	//////////////////////////////////
	/*!
		set mask name
		@param[in] mask_name
	*/
	void SetName(const char* mask_name);
	int GetNameSize() const;
	void GetName(char* mask_name) const;

	//////////////////////////////////
	/*!
	*/
	inline void GetMaskRect(LPRECT lprc) const {
		(*lprc) = m_MaskRect;
	}

	//////////////////////////////////
	/*!
		return display state
		@return display state
	*/
	inline bool isVisible(){
		return m_IsVisible;
	}

	/////////////////////////////////
	/*!
		set display state
		@param[in] bl display state
	*/
	inline void SetVisible(bool bl){
		m_IsVisible = bl;
	}

	inline ImgFile_weakPtr GetParentFile() const {
		return m_pParentFile;
	}

	/////////////////////////////////
	/*!
		set mask synth method fourcc
		@param[in] fourcc
	*/
	inline void SetMaskSynthFourCC(uint32_t fourcc){
		m_MaskSynthFourCC = fourcc;
	}
	inline uint32_t GetMaskSynthFourCC() const {
		return m_MaskSynthFourCC;
	}

	inline void GetMaskData(int x, int y, uint8_t* data) const{
		if( isInRect(x, y, (LPRECT)&m_MaskRect) ){
			m_Img->GetMaskData(x - m_MaskRect.left, y - m_MaskRect.top, data);
		}
	}

	inline const IplImageExt* GetMaskImage() const {
		return m_Img;
	}

	///////////////////////////////////
	inline void ResourceLock(unsigned int id){
#ifdef _WIN32
		switch (id) {
			case IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE:
				::EnterCriticalSection( &m_csUpdateDataQueue );
				break;
			case IE_MASK_RESOURCE_ID::EDITNODE_QUEUE:
				::EnterCriticalSection( &m_csEditNodeQueue );
				break;
			default:
				assert( 0 );
				break;
		}
#else
		while ((m_LockResourceID & id) == id);
		m_LockResourceID |= id;
#endif
	}
	inline void ResourceUnlock(unsigned int id){
#ifdef _WIN32
		switch (id) {
			case IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE:
				::LeaveCriticalSection( &m_csUpdateDataQueue );
				break;
			case IE_MASK_RESOURCE_ID::EDITNODE_QUEUE:
				::LeaveCriticalSection( &m_csEditNodeQueue );
				break;
			default:
				assert( 0 );
				break;
		}
#else
		m_LockResourceID &= (~id);
#endif
	}


private:

	void UpdateMaskState();

	void UpdateEditNode(const LPRECT lprc);

	/////////////////////////////////
	/*!
		pNodeを加算する。
		@param[in] pNode
		@param[in] lprc 更新範囲
	*/
	void AddEditNode(EditNode* pNode, const LPRECT lprc);

	//////////////////////////////////
	/*!
		pNodeを減算する
		@param[in] pNode
		@param[in] lprc 更新範囲
	*/
	void SubEditNode(EditNode* pNode, const LPRECT lprc);

	
	unsigned int m_MaskStateID;

	std::string m_name;
	uint32_t m_MaskSynthFourCC;
	ImgFile_weakPtr m_pParentFile;

	bool m_IsInited;
	bool m_IsVisible;
	RECT m_MaskRect;
	IplImageExt* m_Img; //選択範囲 1ch

	EditQueue* m_pEditQueueForeGrnd;
	EditQueue* m_pEditQueueBackGrnd;

	LPUpdateData_List* m_pUpdateDataForeGrnd_List;
	LPUpdateData_List* m_pUpdateDataBackGrnd_List;

#ifdef _WIN32
	CRITICAL_SECTION m_csUpdateDataQueue;
	CRITICAL_SECTION m_csEditNodeQueue;
#else
	unsigned int m_LockResourceID;
#endif
};


//blt undo image operator
class BltMaskUndoOp
{
public:
	/*!
		@param[out] dst undo image, 1ch
		@param[in] src source of write image, 1ch
		@param[in] mask writed map, 1ch
		@param[in] width write width
	*/
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		if(src == NULL)
			return;

		if(mask == NULL){
			memcpy(dst, src, width * sizeof(uint8_t));
			return;
		}

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;
		uint8_t* pmask = (uint8_t*) mask;

		int i;
		for(i=0; i<width; i++){
			if((*pmask) == 0)
				(*pdst) = (*psrc);
			pdst++;
			psrc++;
			pmask++;
		}
	}
};

#endif //_IMGMASK_H_