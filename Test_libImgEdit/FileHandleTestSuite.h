#include <cxxtest/TestSuite.h>

#include "libImgEdit.h"
#include <Windows.h>

class FileHandleTestSuite : public CxxTest::TestSuite
{
public:
	FileHandleTestSuite()
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

	~FileHandleTestSuite()
	{
		//
		m_ImgEdit.SetBrushPopupWnd(NULL);
		//
		m_ImgEdit.End();
	}

	void test_EditLayerHandle()
	{
		ImgFile_Ptr pFile = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(0,0,0,0));
		if (pFile == NULL) {
			std::cerr << "can't create new file, in test_EditLayerHandle()" << std::endl;
			return;
		}

		RECT edit_rc;
		edit_rc.left = 200;
		edit_rc.top = 100;
		edit_rc.right = edit_rc.left + 300;
		edit_rc.bottom = edit_rc.top + 400;

		EditLayerHandle* pHandle = (EditLayerHandle*)pFile->CreateImgFileHandle(IFH_EDIT_LAYER);

		EditNode* pNode = pHandle->CreateEditNode(&edit_rc);
		pNode->edit_img.RectFillColor(
			edit_rc.left,
			edit_rc.top,
			edit_rc.right - edit_rc.left,
			edit_rc.bottom - edit_rc.top,
			&ucvPixel(100, 120, 140, 160));

		pFile->DoImgFileHandle(pHandle);

		pHandle->Update(&edit_rc);
		pHandle->EndEdit();

		{//check edited layer image
			IplImage* edited_rc_img = cvCreateImage(
				cvSize(edit_rc.right - edit_rc.left, edit_rc.bottom - edit_rc.top),
				IPL_DEPTH_8U,
				4);
			IImgLayer_Ptr pLayer = pFile->GetLayerAtIndex(0);
			pLayer->CopyLayerImage(
				edited_rc_img,
				0,
				0,
				edit_rc.left,
				edit_rc.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top);

			IplImage* correct_img = cvCreateImage(
				cvSize(edit_rc.right - edit_rc.left, edit_rc.bottom - edit_rc.top),
				IPL_DEPTH_8U,
				4);
			cvSet(correct_img, cvScalar(100, 120, 140, 160));

			double diff = GetMaxAbsDiff(edited_rc_img, correct_img);
			TS_ASSERT(diff <= 1.0);

			cvReleaseImage(&edited_rc_img);
			cvReleaseImage(&correct_img);
		}

		pFile->Undo(1);

		RECT updated_rc;
		pFile->LockUpdateData();
		pFile->Update(NULL, &updated_rc);
		pFile->ClearUpdateData();
		pFile->UnlockUpdateData();

		{//check undo layer image
			IImgLayer_Ptr pLayer = pFile->GetLayerAtIndex(0);

			RECT layer_rc;
			pLayer->GetLayerRect(&layer_rc);

			IplImage* layer_img = cvCreateImage(
				cvSize(layer_rc.right - layer_rc.left, layer_rc.bottom - layer_rc.top),
				IPL_DEPTH_8U,
				4);
			pLayer->CopyLayerImage(
				layer_img,
				0,
				0,
				layer_rc.left,
				layer_rc.top,
				layer_rc.right - layer_rc.left,
				layer_rc.bottom - layer_rc.top);

			IplImage* correct_img = cvCreateImage(
				cvSize(layer_rc.right - layer_rc.left, layer_rc.bottom - layer_rc.top),
				IPL_DEPTH_8U,
				4);
			cvSet(correct_img, cvScalar(0,0,0,0));

			TS_ASSERT(isEqualIplImage(layer_img, correct_img));

			cvReleaseImage(&layer_img);
			cvReleaseImage(&correct_img);
		}

		pFile->Redo(1);

		pFile->LockUpdateData();
		pFile->Update(NULL, &updated_rc);
		pFile->ClearUpdateData();
		pFile->UnlockUpdateData();

		{//check redo layer image
			IplImage* edited_rc_img = cvCreateImage(
				cvSize(edit_rc.right - edit_rc.left, edit_rc.bottom - edit_rc.top),
				IPL_DEPTH_8U,
				4);
			IImgLayer_Ptr pLayer = pFile->GetLayerAtIndex(0);
			pLayer->CopyLayerImage(
				edited_rc_img,
				0,
				0,
				edit_rc.left,
				edit_rc.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top);

			IplImage* correct_img = cvCreateImage(
				cvSize(edit_rc.right - edit_rc.left, edit_rc.bottom - edit_rc.top),
				IPL_DEPTH_8U,
				4);
			cvSet(correct_img, cvScalar(100, 120, 140, 160));

			double diff = GetMaxAbsDiff(edited_rc_img, correct_img);
			TS_ASSERT(diff <= 1.0);

			cvReleaseImage(&edited_rc_img);
			cvReleaseImage(&correct_img);
		}
	}

	void test_EditLayerHandle_some_update()
	{
		ImgFile_Ptr pFile = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(140,120,200,255));
		if (pFile == NULL) {
			std::cerr << "can't create new file, in test_EditLayerHandle()" << std::endl;
			return;
		}

		//update
		RECT updated_rc;
		pFile->LockUpdateData();
		pFile->Update(NULL, &updated_rc);
		pFile->ClearUpdateData();
		pFile->UnlockUpdateData();

		IplImage* img1;
		{
			const IplImage* img =	pFile->GetDrawImg();
			img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 4);
			cvCopy(img, img1);
		}

		EditLayerHandle* pHandle = (EditLayerHandle*)pFile->CreateImgFileHandle(IFH_EDIT_LAYER);
		pFile->DoImgFileHandle(pHandle);

		EditNode* pNode = pHandle->CreateEditNode();
		for (int i = 0; i < 5; i++) {
			RECT edit_rc;
			edit_rc.left = 100 * i;
			edit_rc.top = 100 * i;
			edit_rc.right = edit_rc.left + 100;
			edit_rc.bottom = edit_rc.top + 100;
			pNode->edit_img.RectFillColor(
				edit_rc.left - pNode->node_rect.left,
				edit_rc.top - pNode->node_rect.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top,
				&ucvPixel(100, 120, 140, 160));
			pHandle->Update(&edit_rc);

			pFile->LockUpdateData();
			pFile->Update(NULL, &updated_rc);
			pFile->ClearUpdateData();
			pFile->UnlockUpdateData();
		}
		pHandle->EndEdit();

		IplImage* img2;
		{
			const IplImage* img =	pFile->GetDrawImg();
			img2 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 4);
			cvCopy(img, img2);
		}

		pFile->Undo(1);

		pFile->LockUpdateData();
		pFile->Update(NULL, &updated_rc);
		pFile->ClearUpdateData();
		pFile->UnlockUpdateData();

		IplImage* img3;
		{
			const IplImage* img =	pFile->GetDrawImg();
			img3 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 4);
			cvCopy(img, img3);
		}

		pFile->Redo(1);

		pFile->LockUpdateData();
		pFile->Update(NULL, &updated_rc);
		pFile->ClearUpdateData();
		pFile->UnlockUpdateData();

		IplImage* img4;
		{
			const IplImage* img =	pFile->GetDrawImg();
			img4 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 4);
			cvCopy(img, img4);
		}

		//cvSaveImage("img1.bmp", img1);
		//cvSaveImage("img2.bmp", img2);
		//cvSaveImage("img3.bmp", img3);
		//cvSaveImage("img4.bmp", img4);

		TS_ASSERT(isEqualIplImage(img1, img3));
		TS_ASSERT(isEqualIplImage(img2, img4));
	}

	void test_MergeLayerHandle()
	{
		ImgFile_Ptr f = m_ImgEdit.LoadImgFile("test_data\\merge_layer_handle_test.fyd");
		TS_ASSERT(f);

		LPUPDATE_DATA pData = f->CreateUpdateData();
		pData->isAll = true;
		f->PushUpdateData(pData);

		RECT updated_rc;
		f->LockUpdateData();
		f->Update(NULL, &updated_rc);
		f->ClearUpdateData();
		f->UnlockUpdateData();

		CvSize img_size = f->GetImgSize();
		IplImage* correct_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
		const IplImageExt* correct_imgext = f->GetRootLayerGroup()->GetLayerImage();
		correct_imgext->GetRangeIplImage(
			correct_img,
			0, 0,
			img_size.width, img_size.height,
			0, 0);

		IImgLayer_Ptr pLayer = f->GetLayerAtIndex(1);
		IImgLayer_weakPtr pUnderLayer = pLayer->GetUnderLayer();
		MergeLayerHandle* handle = (MergeLayerHandle*) f->CreateImgFileHandle(IFH_MERGE_LAYER);
		handle->AddMergingLayer(pLayer);
		handle->AddMergingLayer(pUnderLayer.lock());
		handle->Do(f);

		f->LockUpdateData();
		f->Update(NULL, &updated_rc);
		f->ClearUpdateData();
		f->UnlockUpdateData();

		IplImage* merged_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
		const IplImageExt* merged_imgext = handle->GetMergedLayer().lock()->GetLayerImage();
		merged_imgext->GetRangeIplImage(
			merged_img,
			0, 0,
			img_size.width, img_size.height,
			0, 0);

		TS_ASSERT(isEqualIplImage(correct_img, merged_img));
	}

	void test_MoveLayerHandle()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(255,255,255,255));
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

		RECT rc;
		pLayer->GetLayerRect(&rc);
		TS_ASSERT_EQUALS(rc.left, 0);
		TS_ASSERT_EQUALS(rc.top, 0);
		TS_ASSERT_EQUALS(rc.right, 1100);
		TS_ASSERT_EQUALS(rc.bottom, 1200);

		f->Undo(1);

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		pLayer->GetLayerRect(&rc);
		TS_ASSERT_EQUALS(rc.left, -100);
		TS_ASSERT_EQUALS(rc.top, -200);
		TS_ASSERT_EQUALS(rc.right, 1000);
		TS_ASSERT_EQUALS(rc.bottom, 1000);

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		RECT edit_rc;
		edit_rc.left = 0;
		edit_rc.top = 0;
		edit_rc.right = edit_rc.left + 300;
		edit_rc.bottom = edit_rc.top + 400;
		{//test edit
			EditLayerHandle* handle = (EditLayerHandle*)f->CreateImgFileHandle(IFH_EDIT_LAYER);

			EditNode* pNode = handle->CreateEditNode(&edit_rc);
			pNode->edit_img.RectFillColor(
				edit_rc.left - pNode->node_rect.left,
				edit_rc.top - pNode->node_rect.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top,
				&ucvPixel(100, 120, 140, 255));

			f->DoImgFileHandle(handle);

			handle->Update(&edit_rc);
			handle->EndEdit();
		}

		{//check edited layer image
			IplImage* edited_rc_img = cvCreateImage(
				cvSize(edit_rc.right - edit_rc.left, edit_rc.bottom - edit_rc.top),
				IPL_DEPTH_8U,
				4);
			IImgLayer_Ptr pLayer = f->GetLayerAtIndex(0);
			pLayer->CopyLayerImage(
				edited_rc_img,
				0,
				0,
				edit_rc.left,
				edit_rc.top,
				edit_rc.right - edit_rc.left,
				edit_rc.bottom - edit_rc.top);

			IplImage* correct_img = cvCreateImage(
				cvSize(edit_rc.right - edit_rc.left, edit_rc.bottom - edit_rc.top),
				IPL_DEPTH_8U,
				4);
			cvSet(correct_img, cvScalar(100, 120, 140, 255));
			
			double diff = GetMaxAbsDiff(edited_rc_img, correct_img);
			TS_ASSERT(diff <= 1.0);

			cvReleaseImage(&edited_rc_img);
			cvReleaseImage(&correct_img);
		}

		m_ImgEdit.DeleteImgFile(f);
	}

	void test_MoveLayerHandle2()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(255,255,255,255));
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

		ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(f->GetSelectLayer().lock());
		{//move layer
			MoveLayerHandle* move_layer_handle = (MoveLayerHandle*) f->CreateImgFileHandle(IFH_MOVE_LAYER);
			f->DoImgFileHandle( move_layer_handle );
			move_layer_handle->SetMoveXY(100, 200);
			move_layer_handle->Update();
			{//update image
				f->LockUpdateData();
				f->Update(NULL, &updated_rc);
				f->ClearUpdateData();
				f->UnlockUpdateData();
			}
			move_layer_handle->SetMoveXY(100, 200);
			move_layer_handle->Update();
			{//update image
				f->LockUpdateData();
				f->Update(NULL, &updated_rc);
				f->ClearUpdateData();
				f->UnlockUpdateData();
			}
			pLayer->ExtendLayer();
		}

		RECT rc;
		pLayer->GetLayerRect(&rc);
		TS_ASSERT_EQUALS(rc.left, 0);
		TS_ASSERT_EQUALS(rc.top, 0);
		TS_ASSERT_EQUALS(rc.right, 1000 + 100);
		TS_ASSERT_EQUALS(rc.bottom, 1000 + 200);
	}

private:
	ImgEdit m_ImgEdit;
};