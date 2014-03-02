#include <cxxtest/TestSuite.h>

#include "libImgEdit.h"
#include <Windows.h>

class WaterColorBrushDrawTestSuite : public CxxTest::TestSuite
{
public:
	WaterColorBrushDrawTestSuite()
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

	~WaterColorBrushDrawTestSuite()
	{
		//
		m_ImgEdit.SetBrushPopupWnd(NULL);
		//
		m_ImgEdit.End();
	}

	void test_EditAfterMoveLayer()
	{
		UCvPixel bg_color = ucvPixel(255,255,255,255);
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
			IIETool* pTool = m_ImgEdit.GetIEToolFromName("WaterColorBrushDraw");
			m_ImgEdit.SetSelectTool(pTool);

			m_ImgEdit.SetSelectBrushGroup(0);
			m_ImgEdit.SetSelectBrush(0);

			IEEMBrush_Ptr pBrush = CreateIEEMBrush();
			pBrush->SetHardness(50);
			pBrush->SetRadius(100);
			m_ImgEdit.AddBrush(pBrush);
			m_ImgEdit.SetSelectBrush(pBrush);
			IEColor old_select_color = m_ImgEdit.GetSelectFGColor();
			m_ImgEdit.SetSelectFGColor(ucvPixel(0,0,0,255));

			//draw brush
			IE_INPUT_DATA input_data;
			input_data.p = 1.0;
			input_data.altitude_rad = 0.0;
			input_data.azimuth_rad = 0.0;
			input_data.random = 1.0;

			input_data.x = 100;
			input_data.y = 100;
			m_ImgEdit.OnMouseLButtonDown(0, &input_data);

			input_data.x = 400;
			input_data.y = 100;
			m_ImgEdit.OnMouseLButtonUp(0, &input_data);

			//
			m_ImgEdit.SetSelectFGColor(old_select_color);
			m_ImgEdit.DeleteBrush(pBrush);
		}

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		{//check edited layer image
			const IplImageExt* pLayer_img_ext = pLayer->GetLayerImage();
			//{
			//	CvSize img_size = cvSize(1000, 1000);
			//	IplImage* layer_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
			//	pLayer->CopyLayerImage(layer_img, 0, 0, 0, 0, 1000, 1000);
			//	cvSaveImage(".\\test_data\\Test_frayer\\WaterColorBrushDraw\\test_EditAfterMoveLayer.bmp", layer_img);
			//	cvReleaseImage(&layer_img);
			//}

			IplImageExt correct_img_ext;
			LoadIplImageExt(".\\test_data\\Test_frayer\\WaterColorBrushDraw\\test_EditAfterMoveLayer.bmp", &correct_img_ext);

			CvSize img_size = correct_img_ext.GetSize();

			IplImage* correct_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
			correct_img_ext.GetRangeIplImage(correct_img, 0, 0, img_size.width, img_size.height, 0, 0);
			pLayer->CopyLayerImage(correct_img, 0, 0, 0, 0, img_size.width, img_size.height);

			IplImage* layer_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
			pLayer->CopyLayerImage(layer_img, 0, 0, 0, 0, img_size.width, img_size.height);

			double diff = GetMaxAbsDiff(layer_img, correct_img);
			TS_ASSERT(diff <= 1.0);

			cvReleaseImage(&layer_img);
			cvReleaseImage(&correct_img);
		}

		f->Undo(1);

		{//update image
			f->LockUpdateData();
			f->Update(NULL, &updated_rc);
			f->ClearUpdateData();
			f->UnlockUpdateData();
		}

		{//check undo layer image
			const IplImageExt* pLayer_img_ext = pLayer->GetLayerImage();
			//{
			//	CvSize img_size = cvSize(1000, 1000);
			//	IplImage* layer_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
			//	pLayer->CopyLayerImage(layer_img, 0, 0, 0, 0, 1000, 1000);
			//	cvSaveImage(".\\test_data\\Test_frayer\\NormalBrushDraw\\undo.bmp", layer_img);
			//	cvReleaseImage(&layer_img);
			//}
			
			CvSize img_size = cvSize(1000, 1000);
			IplImage* layer_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
			pLayer->CopyLayerImage(layer_img, 0, 0, 0, 0, img_size.width, img_size.height);
			IplImage* correct_img = cvCreateImage(img_size, IPL_DEPTH_8U, 4);
			cvSet(correct_img, cvScalar(bg_color.b, bg_color.g, bg_color.r, bg_color.a));

			double diff = GetMaxAbsDiff(layer_img, correct_img);
			TS_ASSERT(diff <= 2.0);

			cvReleaseImage(&layer_img);
			cvReleaseImage(&correct_img);
		}

		m_ImgEdit.DeleteImgFile(f);
	}

private:
	ImgEdit m_ImgEdit;
};