#ifndef _INPUTREVISEMG_H_
#define _INPUTREVISEMG_H_

#include "LibIEDllHeader.h"

class IInputRevise;

class _EXPORTCPP InputReviseMG {
public:
	InputReviseMG();
	virtual ~InputReviseMG();

	//////////////////////////////
	/*!
		補正方法の選択をします。
	*/
	void SelectInputRevise(const char* name);

	//////////////////////////////
	/*!
		選択中の補正方法の名前を得る。
	*/
	void GetInputReviseName(char* name);

	//////////////////////////////
	/*!
		選択中の補正方法クラスを返す。
		@return
	*/
	IInputRevise* GetInputRevise();

	///////////////////////////////
	/*!
		補正方法を選択するコンボボックスをセットする
	*/
	void SetInputReviseComboBox(HWND hCombo);

private:
	IInputRevise* m_pSelectInputRevise;
};

#endif //_INPUTREVISEMG_H_