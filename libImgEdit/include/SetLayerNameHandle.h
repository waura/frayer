#ifndef _SETLAYERNAMEHANDLE_H_
#define _SETLAYERNAMEHANDLE_H_

#include "ImgFileHandle.h"

#include "ImgLayer.h"
#include "ImgFile.h"

class SetLayerNameHandle : public ImgFileHandle
{
public:
	SetLayerNameHandle(){
		strcpy(m_befor_layer_name, "");
		strcpy(m_set_layer_name, "");
	}
	~SetLayerNameHandle(){};

	inline void SetEditLayer(IImgLayer_Ptr pLayer){
		m_pEditLayer = pLayer;
	}

	inline void SetEditName(const char* name){
		strcpy(m_set_layer_name, name);
	}

	bool Do(ImgFile_weakPtr wpFile){
		IImgLayer_Ptr pLayer = m_pEditLayer.lock();
		assert(pLayer);

		pLayer->GetName(m_befor_layer_name);
		pLayer->SetName(m_set_layer_name);
		return true;
	}
	void Redo(ImgFile_weakPtr wpFile){
		Do(wpFile);
	}
	void Undo(ImgFile_weakPtr wpFile){
		IImgLayer_Ptr pLayer = m_pEditLayer.lock();
		assert(pLayer);

		pLayer->SetName(m_befor_layer_name);
	}

private:
	IImgLayer_weakPtr m_pEditLayer;
	char m_befor_layer_name[MAX_IMG_LAYER_NAME];
	char m_set_layer_name[MAX_IMG_LAYER_NAME];
};


#endif //_SETLAYERNAMEHANDLE_H_