#include <cxxtest/TestSuite.h>

#include "libImgEdit.h"
#include <Windows.h>

class ImgLayerTestSuite : public CxxTest::TestSuite
{
public:
	ImgLayerTestSuite()
	{
		//初期カレントディレクトリを取得し、ImgEditに渡す
		char cdPath[MAX_PATH];
		if(0 == ::GetCurrentDirectory(MAX_PATH, cdPath)){
			//error
			TS_ASSERT(true);
		}
		m_ImgEdit.SetStartDirectory(cdPath);

		//
		char exePath[MAX_PATH];
		wsprintf(exePath, "%s/Test_libImgEdit.exe", cdPath);
		m_ImgEdit.Init(".\\config.ini", exePath, ".\\log");
	}

	~ImgLayerTestSuite()
	{
		//
		m_ImgEdit.SetBrushPopupWnd(NULL);
		//
		m_ImgEdit.End();
	}

	void test_AddEditNode_Use_Mask()
	{
		UCvPixel bgColor = ucvPixel(255,255,255,255);
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &bgColor);

		RECT updated_rc;
		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(f->GetSelectLayer().lock());
		{//move layer
			MoveLayerHandle* move_layer_handle = (MoveLayerHandle*) f->CreateImgFileHandle(IFH_MOVE_LAYER);
			f->DoImgFileHandle( move_layer_handle );
			move_layer_handle->SetMoveXY(100, 200);
			move_layer_handle->Update();

			pLayer->ExtendLayer();
		}

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		f->Undo(1);

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		RECT mask_rc;
		mask_rc.left = 100;
		mask_rc.top = 200;
		mask_rc.right = mask_rc.left + 300;
		mask_rc.bottom = mask_rc.top + 300;

		{//edit mask
			EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)f->CreateImgFileHandle(IFH_EDIT_MASK);
			f->DoImgFileHandle( pEditMaskHandle );

			EditNode* pEditNode = pEditMaskHandle->CreateEditNode();
			pEditNode->edit_img.RectFillMask(
				mask_rc.left - pEditNode->node_rect.left,
				mask_rc.top - pEditNode->node_rect.top,
				mask_rc.right - mask_rc.left,
				mask_rc.bottom - mask_rc.top,
				255);

			pEditMaskHandle->Update(&(pEditNode->blt_rect));
			pEditMaskHandle->EndEdit();
		}

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		RECT edit_rc;
		edit_rc.left = 150;
		edit_rc.top = 100;
		edit_rc.right = edit_rc.left + 200;
		edit_rc.bottom = edit_rc.top + 400;

		UCvPixel fillColor = ucvPixel(100, 120, 140, 240);

		{//edit image
			EditLayerHandle* pHandle = (EditLayerHandle*)f->CreateImgFileHandle(IFH_EDIT_LAYER);
			f->DoImgFileHandle(pHandle);

			EditNode* pNode = pHandle->CreateEditNode();
			pNode->edit_img.RectFillColor(
				edit_rc.left - pNode->node_rect.left,
				edit_rc.top - pNode->node_rect.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top,
				&fillColor);
			pHandle->Update(&edit_rc);
			pHandle->EndEdit();
		}

		{//check edited image
			RECT and_rc;
			AndRect(&mask_rc, &edit_rc, &and_rc);

			IplImage* edited_img = cvCreateImage(
				cvSize(and_rc.right - and_rc.left, and_rc.bottom - and_rc.top),
				IPL_DEPTH_8U,
				4);

			IImgLayer_Ptr pLayer = f->GetLayerAtIndex(0);
			pLayer->CopyLayerImage(
				edited_img,
				0,
				0,
				and_rc.left,
				and_rc.top,
				and_rc.right - and_rc.left,
				and_rc.bottom - and_rc.top);

			IplImage* correct_img = cvCreateImage(
				cvSize(and_rc.right - and_rc.left, and_rc.bottom - and_rc.top),
				IPL_DEPTH_8U,
				4);
			cvSet(correct_img, cvScalar(fillColor.b, fillColor.g, fillColor.r, fillColor.a));

			double diff = GetMaxAbsDiff(edited_img, correct_img);
			TS_ASSERT(diff < 20.0);

			UCvPixel pix;
			pLayer->GetPixel(and_rc.left - 1, and_rc.top, &pix);
			TS_ASSERT(pix.value == bgColor.value);
			pLayer->GetPixel(and_rc.left, and_rc.top - 1, &pix);
			TS_ASSERT(pix.value == bgColor.value);
			pLayer->GetPixel(and_rc.right - 1, and_rc.bottom, &pix);
			TS_ASSERT(pix.value == bgColor.value);
			pLayer->GetPixel(and_rc.right, and_rc.bottom - 1, &pix);
			TS_ASSERT(pix.value == bgColor.value);
		}
	}

	void test_Undo_Before_LayerMove()
	{
		UCvPixel bg_color = ucvPixel(123,30,30,255);
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &bg_color);
		TS_ASSERT(f);

		LPUPDATE_DATA pData = f->CreateUpdateData();
		pData->isAll = true;
		f->PushUpdateData(pData);

		RECT updated_rc;
		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		//create correct image
		const IplImage* fimg = f->GetDrawImg();
		IplImage* correct_img = cvCreateImage(cvSize(fimg->width, fimg->height), fimg->depth, fimg->nChannels);
		cvCopy(fimg, correct_img);

		ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(f->GetSelectLayer().lock());
		{//move layer
			MoveLayerHandle* move_layer_handle = (MoveLayerHandle*) f->CreateImgFileHandle(IFH_MOVE_LAYER);
			f->DoImgFileHandle( move_layer_handle );
			move_layer_handle->SetMoveXY(100, 200);
			move_layer_handle->Update();

			pLayer->ExtendLayer();
		}

		{//layer edit
			RECT edit_rc;
			edit_rc.left = 150;
			edit_rc.top = 100;
			edit_rc.right = edit_rc.left + 200;
			edit_rc.bottom = edit_rc.top + 400;

			UCvPixel fillColor = ucvPixel(100, 120, 140, 240);

			EditLayerHandle* pHandle = (EditLayerHandle*)f->CreateImgFileHandle(IFH_EDIT_LAYER);
			f->DoImgFileHandle(pHandle);

			EditNode* pNode = pHandle->CreateEditNode();
			pNode->edit_img.RectFillColor(
				edit_rc.left - pNode->node_rect.left,
				edit_rc.top - pNode->node_rect.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top,
				&fillColor);
			pHandle->Update(&edit_rc);
			pHandle->EndEdit();
		}

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		f->Undo(2);

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		fimg = f->GetDrawImg();
		IplImage* edited_img = cvCreateImage(cvSize(fimg->width, fimg->height), fimg->depth, fimg->nChannels);
		cvCopy(fimg, edited_img);

		TS_ASSERT(isEqualIplImage(edited_img, correct_img));

		cvReleaseImage(&correct_img);
		cvReleaseImage(&edited_img);
	}

	void test_Undo_After_LayerMove()
	{
		UCvPixel bg_color = ucvPixel(123,30,30,255);
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &bg_color);
		TS_ASSERT(f);

		LPUPDATE_DATA pData = f->CreateUpdateData();
		pData->isAll = true;
		f->PushUpdateData(pData);

		RECT updated_rc;
		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		//create correct image
		const IplImage* fimg = f->GetDrawImg();
		IplImage* correct_img = cvCreateImage(cvSize(fimg->width, fimg->height), fimg->depth, fimg->nChannels);
		cvCopy(fimg, correct_img);


		{//layer edit
			RECT edit_rc;
			edit_rc.left = 150;
			edit_rc.top = 100;
			edit_rc.right = edit_rc.left + 200;
			edit_rc.bottom = edit_rc.top + 400;

			UCvPixel fillColor = ucvPixel(100, 120, 140, 240);

			EditLayerHandle* pHandle = (EditLayerHandle*)f->CreateImgFileHandle(IFH_EDIT_LAYER);
			f->DoImgFileHandle(pHandle);

			EditNode* pNode = pHandle->CreateEditNode();
			pNode->edit_img.RectFillColor(
				edit_rc.left - pNode->node_rect.left,
				edit_rc.top - pNode->node_rect.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top,
				&fillColor);
			pHandle->Update(&edit_rc);
			pHandle->EndEdit();
		}

		ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(f->GetSelectLayer().lock());
		{//move layer
			MoveLayerHandle* move_layer_handle = (MoveLayerHandle*) f->CreateImgFileHandle(IFH_MOVE_LAYER);
			f->DoImgFileHandle( move_layer_handle );
			move_layer_handle->SetMoveXY(100, 200);
			move_layer_handle->Update();

			pLayer->ExtendLayer();
		}

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		f->Undo(2);

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		fimg = f->GetDrawImg();
		IplImage* edited_img = cvCreateImage(cvSize(fimg->width, fimg->height), fimg->depth, fimg->nChannels);
		cvCopy(fimg, edited_img);

		TS_ASSERT(isEqualIplImage(edited_img, correct_img));

		cvReleaseImage(&correct_img);
		cvReleaseImage(&edited_img);
	}

private:
	ImgEdit m_ImgEdit;
};
