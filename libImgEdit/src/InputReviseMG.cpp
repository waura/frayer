#include "stdafx.h"
#include "InputReviseMG.h"

#include "IInputRevise.h"
#include "InputNoRevise.h"
#include "InputMovingAverageRevise.h"
#include "InputGaussianRevise.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


static InputNoRevise noRevise;
static InputMovingAverageRevise MA4Revise(4);
static InputMovingAverageRevise MA8Revise(8);
static InputGaussianRevise G4Revise(4, 5.0);
static InputGaussianRevise G8Revise(8, 5.0);


InputReviseMG::InputReviseMG()
{
	m_pSelectInputRevise = &noRevise;
}

InputReviseMG::~InputReviseMG()
{
}

//////////////////////////////
/*!
	補正方法の選択をします。
*/
void InputReviseMG::SelectInputRevise(const char *name)
{
	if(strcmp(name, "補正無し") == 0){
		m_pSelectInputRevise = &noRevise;
	}
	else if(strcmp(name, "平均補正4") == 0){
		m_pSelectInputRevise = &MA4Revise;
	}
	else if(strcmp(name, "平均補正8") == 0){
		m_pSelectInputRevise = &MA8Revise;
	}
	else if(strcmp(name, "G補正4") == 0){
		m_pSelectInputRevise = &G4Revise;
	}
	else if(strcmp(name, "G補正8") == 0){
		m_pSelectInputRevise = &G8Revise;
	}
}

//////////////////////////////
/*!
	選択中の補正方法の名前を得る。
*/
void InputReviseMG::GetInputReviseName(char* name)
{
	if(m_pSelectInputRevise == &noRevise){
		strcpy(name, "補正無し");
	}
	else if(m_pSelectInputRevise == &MA4Revise){
		strcpy(name, "平均補正4");
	}
	else if(m_pSelectInputRevise == &MA8Revise){
		strcpy(name, "平均補正8");
	}
	else if(m_pSelectInputRevise == &G4Revise){
		strcpy(name, "G補正4");
	}
	else if(m_pSelectInputRevise == &G8Revise){
		strcpy(name, "G補正8");
	}
}

//////////////////////////////
/*!
	選択中の補正方法クラスを返す。
	@return
*/
IInputRevise* InputReviseMG::GetInputRevise()
{
	return m_pSelectInputRevise;
}

///////////////////////////////
/*!
	補正方法を選択するコンボボックスをセットする
*/
void InputReviseMG::SetInputReviseComboBox(HWND hCombo)
{
	//add item
	::SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"補正無し");
	::SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"平均補正4");
	::SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"平均補正8");
	::SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"G補正4");
	::SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"G補正8");
	
	//select item
	char name[256];
	GetInputReviseName(name);
	int select_index = ::SendMessage(hCombo, CB_FINDSTRINGEXACT, 0, (LPARAM)name);
	::SendMessage(hCombo, CB_SETCURSEL, select_index, 0);
}
