#include <cxxtest/TestSuite.h>

#include "libImgEdit.h"
#include <Windows.h>

class FileIOTestSuite : public CxxTest::TestSuite
{
public:
	FileIOTestSuite()
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

	~FileIOTestSuite()
	{
		//
		m_ImgEdit.SetBrushPopupWnd(NULL);
		//
		m_ImgEdit.End();
	}

	void test_IEIOpsd()
	{
		ImgFile_Ptr pFile1 = m_ImgEdit.LoadImgFile("test_data\\Test_frayer\\FileIO\\psd_test1.psd");
		
		LPUPDATE_DATA pData1 = pFile1->CreateUpdateData();
		pData1->isAll = true;
		pFile1->PushUpdateData(pData1);

		RECT updated_rc;
		pFile1->LockUpdateData();
		pFile1->Update(NULL, &updated_rc);
		pFile1->ClearUpdateData();
		pFile1->UnlockUpdateData();

		m_ImgEdit.SaveAsImgFile("test_data\\Test_frayer\\FileIO\\psd_test1.fyd", pFile1);
		ImgFile_Ptr pFile2 = m_ImgEdit.LoadImgFile("test_data\\Test_frayer\\FileIO\\psd_test1.fyd");

		LPUPDATE_DATA pData2 = pFile2->CreateUpdateData();
		pData2->isAll = true;
		pFile2->PushUpdateData(pData2);

		pFile2->LockUpdateData();
		pFile2->Update(NULL, &updated_rc);
		pFile2->ClearUpdateData();
		pFile2->UnlockUpdateData();

		cvSaveImage("test_data\\Test_frayer\\FileIO\\file1.bmp", pFile1->GetDrawImg());
		cvSaveImage("test_data\\Test_frayer\\FileIO\\file2.bmp", pFile2->GetDrawImg());
		TS_ASSERT(isEqualIplImage(pFile1->GetDrawImg(), pFile2->GetDrawImg()));
	}

private:
	ImgEdit m_ImgEdit;
};
