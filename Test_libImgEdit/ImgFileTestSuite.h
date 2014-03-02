#include <cxxtest/TestSuite.h>

#include "libImgEdit.h"
#include <Windows.h>

class ImgFileTestSuite : public CxxTest::TestSuite
{
public:
	ImgFileTestSuite()
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

	~ImgFileTestSuite()
	{
		//
		m_ImgEdit.SetBrushPopupWnd(NULL);
		//
		m_ImgEdit.End();
	}

	void test_Update_return_val()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(255,255,255,255));

		LPUPDATE_DATA pData = f->CreateUpdateData();
		pData->isAll = true;
		f->PushUpdateData(pData);

		bool ret_val;
		RECT updated_rc;
		f->LockUpdateData();
		ret_val = f->Update(NULL,  &updated_rc);
		f->ClearUpdateData();
		f->LockUpdateData();

		TS_ASSERT(ret_val == true);

		f->LockUpdateData();
		ret_val = f->Update(NULL,  &updated_rc);
		f->ClearUpdateData();
		f->LockUpdateData();

		TS_ASSERT(ret_val == false);
	}

	void test_AddLayer()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(255,255,255,255));
		TS_ASSERT_EQUALS(f->GetLayerSize(), 1);
		f->SetNumOfHistory(100);

		AddNewLayerHandle* handle0 = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
		handle0->SetAddLayerIndex(0);
		f->DoImgFileHandle(handle0);
		TS_ASSERT_EQUALS(f->GetLayerAtIndex(0), handle0->GetNewLayer().lock());
		TS_ASSERT_EQUALS(f->GetLayerSize(), 2);
		
		AddNewLayerHandle* handle1 = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
		handle1->SetAddLayerIndex(1);
		f->DoImgFileHandle(handle1);
		TS_ASSERT_EQUALS(f->GetLayerAtIndex(1), handle1->GetNewLayer().lock());
		TS_ASSERT_EQUALS(f->GetLayerSize(), 3);
				
		AddNewLayerGroupHandle* handle2 = (AddNewLayerGroupHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER_GROUP);
		handle2->SetAddLayerIndex(2);
		f->DoImgFileHandle(handle2);
		TS_ASSERT_EQUALS(f->GetLayerAtIndex(3), handle2->GetNewLayeyGroup().lock());
		TS_ASSERT_EQUALS(f->GetLayerSize(), 4);
		
		AddNewLayerHandle* handle3 = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
		handle3->SetAddLayerIndex(3);
		f->DoImgFileHandle(handle3);
		TS_ASSERT_EQUALS(f->GetLayerAtIndex(3), handle3->GetNewLayer().lock());
		TS_ASSERT_EQUALS(f->GetLayerSize(), 5);

		//check layer line
		IImgLayer_Ptr pLayer = f->GetLayerAtIndex(4);
		TS_ASSERT_EQUALS(pLayer->GetLayerType(), IE_LAYER_TYPE::GROUP_LAYER);
		ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
		TS_ASSERT_EQUALS(pLGroup->GetNumOfChildLayers(), 1);

		//check undo of add layer handle
		f->Undo(2);
		TS_ASSERT_EQUALS(f->GetLayerIndex(handle2->GetNewLayeyGroup()), -1);
		TS_ASSERT_EQUALS(f->GetLayerIndex(handle3->GetNewLayer()), -1);
		TS_ASSERT_EQUALS(f->GetLayerSize(), 3);


		m_ImgEdit.DeleteImgFile(f);
	}

	void test_RemoveLayer()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(255,255,255,255));
		TS_ASSERT_EQUALS(f->GetLayerSize(), 1);
		f->SetNumOfHistory(100);

		{
			AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetAddLayerIndex(0);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(0), handle->GetNewLayer().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 2);
		}
		{
			AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetAddLayerIndex(1);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(1), handle->GetNewLayer().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 3);
		}
		{
			AddNewLayerGroupHandle* handle = (AddNewLayerGroupHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER_GROUP);
			handle->SetAddLayerIndex(2);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(3), handle->GetNewLayeyGroup().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 4);
		}
		{
			AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetAddLayerIndex(3);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(3), handle->GetNewLayer().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 5);
		}
		{
			RemoveLayerHandle* handle = (RemoveLayerHandle*)f->CreateImgFileHandle(IFH_REMOVE_LAYER);
			IImgLayer_Ptr pLayer = f->GetLayerAtIndex(3);
			handle->SetRemoveLayer(pLayer);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerIndex(pLayer), -1);
			TS_ASSERT_EQUALS(f->GetLayerSize(), 4);
		}
		{
			RemoveLayerHandle* handle = (RemoveLayerHandle*)f->CreateImgFileHandle(IFH_REMOVE_LAYER);
			IImgLayer_Ptr pLayer = f->GetLayerAtIndex(1);
			handle->SetRemoveLayer(pLayer);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerIndex(pLayer), -1);
			TS_ASSERT_EQUALS(f->GetLayerSize(), 3);
		}

		//check layer line
		IImgLayer_Ptr pLayer = f->GetLayerAtIndex(2);
		TS_ASSERT_EQUALS(pLayer->GetLayerType(), IE_LAYER_TYPE::GROUP_LAYER);
		ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
		TS_ASSERT_EQUALS(pLGroup->GetNumOfChildLayers(), 0);

		//check undo of remove layer handle
		f->Undo(2);
		TS_ASSERT_EQUALS(f->GetLayerSize(), 5);

		m_ImgEdit.DeleteImgFile(f);
	}

	void test_ChangeLayerLine()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(255,255,255,255));
		TS_ASSERT_EQUALS(f->GetLayerSize(), 1);
		f->SetNumOfHistory(100);

		{
			AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetAddLayerIndex(1);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(1), handle->GetNewLayer().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 2);
		}
		{
			AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetAddLayerIndex(2);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(2), handle->GetNewLayer().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 3);
		}
		{
			AddNewLayerGroupHandle* handle = (AddNewLayerGroupHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER_GROUP);
			handle->SetAddLayerIndex(3);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(4), handle->GetNewLayeyGroup().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 4);
		}
		{
			AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetAddLayerIndex(4);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(4), handle->GetNewLayer().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 5);
		}
		{
			AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetAddLayerIndex(4);
			f->DoImgFileHandle(handle);
			TS_ASSERT_EQUALS(f->GetLayerAtIndex(4), handle->GetNewLayer().lock());
			TS_ASSERT_EQUALS(f->GetLayerSize(), 6);
		}
		{
			ChangeLayerLineHandle* handle = (ChangeLayerLineHandle*)f->CreateImgFileHandle(IFH_CHANGE_LAYER_LINE);
			handle->SetChangeIndex(1, 4);
			f->DoImgFileHandle(handle);

			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(f->GetLayerAtIndex(6));
			TS_ASSERT_EQUALS(pLGroup->GetLayerType(), IE_LAYER_TYPE::GROUP_LAYER);
			TS_ASSERT_EQUALS(pLGroup->GetNumOfChildLayers(), 3);
		}

		IImgLayer_Ptr pLayer = f->GetRootLayerGroup()->GetLayerAtIndex(6);
		TS_ASSERT_EQUALS(pLayer->GetLayerType(), IE_LAYER_TYPE::GROUP_LAYER);
		ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
		TS_ASSERT_EQUALS(pLGroup->GetNumOfChildLayers(), 3);

		f->Undo(5);
		TS_ASSERT_EQUALS(f->GetLayerSize(), 2);

		m_ImgEdit.DeleteImgFile(f);
	}

	void test_MaskState()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(255,255,255,255));
		TS_ASSERT_EQUALS(f->GetMaskState(), IE_MASK_STATE_ID::INVALID);

		{
			EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)f->CreateImgFileHandle(IFH_EDIT_MASK);
			f->DoImgFileHandle( pEditMaskHandle );
	
			EditNode* pEditNode = pEditMaskHandle->CreateEditNode();
			pEditNode->edit_img.RectFillMask(
				100, 200,
				300, 300,
				200);

			RECT rc;
			rc.left = 100; rc.top = 200;
			rc.right = 400; rc.bottom = 500;
			pEditMaskHandle->Update(&rc);
			pEditMaskHandle->EndEdit();
		}
		TS_ASSERT_EQUALS(f->GetMaskState(), IE_MASK_STATE_ID::VALID);

		//clear mask
		{
			RECT rc;
			f->GetMaskRect(&rc);

			EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)f->CreateImgFileHandle(IFH_EDIT_MASK);
			f->DoImgFileHandle( pEditMaskHandle );
	
			EditNode* pEditNode = pEditMaskHandle->CreateEditNode(&rc);
			pEditNode->fourcc = IE_MASK_SYNTH_FOURCC('s','u','b',' ');
			pEditNode->edit_img.FillMask(255);

			pEditMaskHandle->Update(&rc);
			pEditMaskHandle->EndEdit();
		}
		TS_ASSERT_EQUALS(f->GetMaskState(), IE_MASK_STATE_ID::INVALID);

		{
			f->Undo(1);

			LPUPDATE_DATA pData = f->CreateUpdateData();
			f->PushUpdateData(pData);

			RECT updated_rc;
			f->LockUpdateData();
			f->Update(NULL,  &updated_rc);
			f->ClearUpdateData();
			f->LockUpdateData();

			TS_ASSERT_EQUALS(f->GetMaskState(), IE_MASK_STATE_ID::VALID);
		}

		{
			f->Redo(1);

			LPUPDATE_DATA pData = f->CreateUpdateData();
			f->PushUpdateData(pData);

			RECT updated_rc;
			f->LockUpdateData();
			f->Update(NULL,  &updated_rc);
			f->ClearUpdateData();
			f->LockUpdateData();

			TS_ASSERT_EQUALS(f->GetMaskState(), IE_MASK_STATE_ID::INVALID);
		}
	}

	void test_Redo_on_Used_Mask()
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

		{//mask edit
			EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)f->CreateImgFileHandle(IFH_EDIT_MASK);
			f->DoImgFileHandle( pEditMaskHandle );
	
			EditNode* pEditNode = pEditMaskHandle->CreateEditNode();
			pEditNode->edit_img.RectFillMask(
				100, 200,
				300, 300,
				200);

			RECT rc;
			rc.left = 100; rc.top = 200;
			rc.right = 400; rc.bottom = 500;
			pEditMaskHandle->Update(&rc);
			pEditMaskHandle->EndEdit();
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

		const IplImage* fimg = f->GetDrawImg();
		IplImage* correct_img = cvCreateImage(cvSize(fimg->width, fimg->height), fimg->depth, fimg->nChannels);
		cvCopy(fimg, correct_img);

		f->Undo(2);

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		f->Redo(2);

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
