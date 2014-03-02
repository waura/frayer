#ifndef _OPENCV2DIRECTX9_H_
#define _OPENCV2DIRECTX9_H_

#include <d3dx9.h>
#include <cxcore.h>

///////////////////////////////////////////////////////
/*!
	IplImageからDirectXテクスチャを作成する。
	@param[in] pDevice 使用するデバイス
	@param[in] Width 作成するテクスチャの幅
	@param[in] Height 作成するテクスチャの高さ
	@param[in] Levels ミップマップレベル
	@param[out] ppTexture 作成されたテクスチャ リソースを表す
*/
HRESULT CreateTextureIPLImage(LPDIRECT3DDEVICE9 pDevice, IplImage* iplImage, UINT Width, UINT Height, UINT Levels, LPDIRECT3DTEXTURE9* ppTexture);

#endif //_OPENCV2DIRECTX9_H_