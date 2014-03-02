#ifndef _IMGEDIT_H_
#define _IMGEDIT_H_

#include "LibIEDllHeader.h"

#include "IEBrush.h"
#include "IEBrushIO.h"
#include "ILayerSynthesizer.h"
#include "LayerSynthesizerMG.h"
#include "ImgFileIOMG.h"
#include "InputReviseMG.h"
#include "IEBrushMG.h"
#include "IEPaletteMG.h"
#include "IIECommand.h"
#include "IIEBrushPopupWnd.h"
#include "IECommandMG.h"
#include "EffectMG.h"
#include "LayerSynthesizerMG.h"
#include "IEToolMG.h"
#include "ImgFileIOMG.h"
#include "IEKeyMap.h"
#include "IEParamRegister.h"
#include "IEEventCapture.h"
#include "ImgEditConfig.h"

#include <list>


typedef std::list<ImgEditEventListener* > ImgEditEventListener_List;
typedef std::vector<ImgFile_Ptr> ImgFile_Vec;

enum IE_RESOURCE_ID{
	IE_COMMAND=0x01,
	IE_TOOL=0x02,
};

enum IE_CURSOR_ID{
	NORMAL_CURSOR=1,
	BRUSH_CURSOR=2,
};

enum {
	IMG_EDIT_MAX_FILE_WIDTH = 10000,
	IMG_EDIT_MAX_FILE_HEIGHT = 10000,
};

class _EXPORTCPP ImgEdit
{
public:
	ImgEdit();
	~ImgEdit();

	//////////////////////////////////
	/*!
		初期化処理
		@param[in] logFilePath
	*/
	bool Init(const char* configini_path, const char* exe_path, const char* log_dir);

	/////////////////////////////////
	/*!
		終了処理
	*/
	void End();

	/////////////////////////////////
	/*!
		イベントリスナーを登録する。
		@param[in] pListener 登録するリスナー
	*/
	void AddEventListener(ImgEditEventListener*  pListener);

	//////////////////////////////////
	/*!
		イベントリスナーを削除する
		@param[in] pListener 削除するリスナー
	*/
	void DeleteEventListener(ImgEditEventListener*  pListener);

	/////////////////////////////////
	/*!
		初期ディレクトリを指定する。
		@param[in] path 初期ディレクトリパス
	*/
	void SetStartDirectory(const char* path);

	///////////////////////////////
	/*!
		設定クラスへのポインタを返す。
		@return
	*/
	ImgEditConfig* GetIEConfig();

	///////////////////////////////
	/*!
		前景色と背景色を交換する
	*/
	void ExchangeColorFGForBG();

	///////////////////////////////
	/*!
		前景色を設定する
		@param[in] color 設定する色
	*/
	void SetSelectFGColor(IEColor color);

	///////////////////////////////
	/*!
		現在選択中の前景色を返す
		@return 選択中の色
	*/
	IEColor GetSelectFGColor() const {
		return m_SelectFGColor;
	}

	///////////////////////////////
	/*!
		現在選択中の前景色を返す
		@return 選択中の色
	*/
	IEColor GetSelectBGColor() const {
		return m_SelectBGColor;
	}

	////////////////////////////////
	/*!
		編集中のファイルを切り替える。
		@param[in] pFile 切り替え後の編集ファイル
	*/
	void SetActiveFile(ImgFile_Ptr pFile);

	////////////////////////////////
	/*!
		編集中のファイルのポインタを返す
		@return 編集中のファイルのポインタ
	*/
	ImgFile_Ptr GetActiveFile() const {
		return m_pNowActiveFile;
	}

	////////////////////////////////
	/*!
		ファイルオブジェクトを追加する。
		@param[in] pFile 追加するファイル
	*/
	void AddImgFile(ImgFile_Ptr pFile);

	//////////////////////////////
	/*!
		ファイルオブジェクトの削除、メモリ開放
		@param[in] pFile ファイルのポインタ
	*/
	void DeleteImgFile(ImgFile_Ptr pFile);

	///////////////////////////////
	/*!
	*/
	void AddIEBrushMGEventListener(IEBrushMGEventListener*  pListener) {
		m_IEBrushMG.AddEventListener( pListener );
	}

	///////////////////////////////
	/*
	*/
	void DeleteIEBrushMGEventListener(IEBrushMGEventListener*  pListener) {
		m_IEBrushMG.DeleteEventListener( pListener );
	}

	void SetBrushGroupComboBox(HWND hCombo) const {
		m_IEBrushMG.SetBrushGroupComboBox( hCombo );
	}

	void SetSelectBrushGroup(int nIndex) {
		m_IEBrushMG.SetSelectBrushGroup( nIndex );
	}

	int GetSelectBrushGroupIndex() {
		return m_IEBrushMG.GetSelectBrushGroupIndex();
	}

	////////////////////////////////
	/*!
		ブラシを選択する
		@param[in] pBrush 選択するブラシのポインタ
	*/
	void SetSelectBrush(IEBrush_Ptr pBrush) {
		m_IEBrushMG.SetSelectBrush( pBrush );
	}

	////////////////////////////////
	/*!
		ブラシを選択する
		@param[in] index 選択するブラシのインデックス
	*/
	void SetSelectBrush(int index) {
		m_IEBrushMG.SetSelectBrush(index);
	}

	////////////////////////////////
	/*!
		選択中のブラシを返す
		@return 選択中のブラシ
	*/
	IEBrush_Ptr GetSelectBrush() const {
		return m_IEBrushMG.GetSelectBrush();
	};

	int GetSelectBrushIndex() {
		return m_IEBrushMG.GetSelectBrushIndex();
	}

	////////////////////////////////
	/*!
		保持しているブラシ数を返す
		@return 保持しているブラシ数
	*/
	int GetBrushSize() const {
		return m_IEBrushMG.GetBrushSize();
	};

	////////////////////////////////
	/*!
		インデックスを指定してブラシを返す
		@param[in] index ブラシインデックス
		@return 指定されたブラシ
	*/
	IEBrush_Ptr GetBrushAtIndex(int index) const {
		return m_IEBrushMG.GetBrushAtIndex( index );
	}

	////////////////////////////////
	/*!
		ブラシファイルを読み込む
		@param[in] filename ファイルパス
	*/
	void LoadBrush(const char* filename) {
		return m_IEBrushMG.LoadBrush( filename );
	}

	//////////////////////////////
	/*!
		ブラシを追加する。
		@param[in] pBruhs 追加するブラシ
	*/
	void AddBrush(IEBrush_Ptr pBrush) {
		m_IEBrushMG.AddBrush( pBrush );
	}

	//////////////////////////////
	/*!
		ブラシを削除する。
		@param[in] pBrush　削除するブラシ
	*/
	void DeleteBrush(IEBrush_Ptr pBrush) {
		m_IEBrushMG.DeleteBrush( pBrush );
	}

	//////////////////////////////
	/*!
		ブラシテクスチャの名前からインデックスを得る
		@param[in] texture_name
	*/
	int GetBrushTextureIndex(const char* texture_name) const {
		return m_IEBrushMG.GetBrushTextureIndex(texture_name);
	}

	//////////////////////////////
	/*!
		ブラシテクスチャのインデックスからパスを得る
		@param[in] texture_path
	*/
	void GetBrushTexturePath(char* dst_path, int max_path_size, int index) const {
		m_IEBrushMG.GetBrushTexturePath(dst_path, max_path_size, index);
	}

	//////////////////////////////
	/*!
		コンボボックスにブラシテクスチャ名を追加する
		@param[in] hCombo
	*/
	void SetBrushTextureComboBox(HWND hCombo) const {
		m_IEBrushMG.SetBrushTextureComboBox(hCombo);
	}


	//////////////////////////////
	/*!
		使用するブラシポップアップウィンドウをセットする。
		@param[in] pWnd ブラシポップアップウィンドウインターフェース
	*/
	void SetBrushPopupWnd(IIEBrushPopupWnd* pWnd);

	//////////////////////////////
	/*!
		ブラシポップアップウィンドウを返す
		@return ブラシポップアップインターフェースを返す
	*/
	IIEBrushPopupWnd* GetBrushPopupWnd();

	///////////////////////////////
	/*!
	*/
	void AddIEPaletteMGEventListener(IEPaletteMGEventListener*  pListener) {
		m_IEPaletteMG.AddEventListener( pListener );
	}

	///////////////////////////////
	/*
	*/
	void DeleteIEPaletteMGEventListener(IEPaletteMGEventListener*  pListener) {
		m_IEPaletteMG.DeleteEventListener( pListener );
	}
	///////////////////////////////
	/*!
		すべてのパレット名をコンボボックスに追加する
		@param[in] hCombo
	*/
	void SetPaletteComboBox(HWND hCombo) {
		m_IEPaletteMG.SetPaletteComboBox( hCombo );
	}

	///////////////////////////////
	/*!
		indexを指定してパレットを指定する
		@param[in] index
	*/
	void SetSelectPalette(int index) {
		m_IEPaletteMG.SetSelectPalette( index );
	}

	////////////////////////////////
	/*!
		選択されているパレットが保持している色数を返す
		@return 保持している色数
	*/
	int GetSelectPaletteSize() const {
		return m_IEPaletteMG.GetSelectPaletteSize();
	};

	//////////////////////////////
	/*!
		色情報を追加する。
		@param[in] color 追加する色
	*/
	void AddColorToPalette(const char* name, IEColor color) {
		m_IEPaletteMG.AddColor(name, color);
	}

	//////////////////////////////
	/*!
		色を削除する。
		@param[in] index　削除する色のインデックス
	*/
	void DeleteColorFromPalette(int index) {
		m_IEPaletteMG.DeleteColor( index );
	}

	//////////////////////////////
	/*!
		新規にファイルを作成
		@return 作成したファイルのポインタ(失敗すればNULLを返す)
	*/
	ImgFile_Ptr CreateNewFile();

	//////////////////////////////
	/*!
		サイズを指定して新規にファイルを作成
		@param[in] width 作成するファイルの幅
		@param[in] height 作成するファイルの高さ
		@param[in] fillcolor 塗りつぶす色
		@return 作成したファイルのポインタ(失敗すればNULLを返す)
	*/
	ImgFile_Ptr CreateNewFile(int width, int height, const UCvPixel* fillcolor);

	//////////////////////////////
	/*!
		画像ファイルを読み込む。
		@param[in] filePath 画像ファイルのパス
		@return 作成したファイルのポインタ(失敗すればNULLを返す)
	*/
	ImgFile_Ptr LoadImgFile(const char* filePath);

	//////////////////////////////
	/*!
		名前をつけて保存
		@param[in] filePath 保存する画像のファイルパス
		@param[in] pFile 保存するファイル
	*/
	void SaveAsImgFile(const char *filePath, ImgFile_Ptr pFile);

	//////////////////////////////
	/*!
		上書き保存
		@param[in] pFile 保存するファイル
	*/
	void SaveImgFile(ImgFile_Ptr pFile);

	//////////////////////////////
	/*!
		イベントキャプチャを開始する
		@param[in] capture_file_path イベントキャプチャの保存先ファイルパス
	*/
	void StartEventCapture(const char *capture_file_path); 

	//////////////////////////////
	/*!
		イベントキャプチャを終了する
	*/
	void EndEventCapture();

	void PlayEventCapture(const char *capture_file_path);
	void PlayEventCaptureAsync(const char *capture_file_path);

	//////////////////////////////
	/*!
		現在編集中のファイルのサイズ(幅x高さ)を返す。
		@return rc ファイルのサイズ
	*/
	CvSize GetNowActiveFileSize();

	///////////////////////////////
	/*!
		デフォルトのレイヤーシンセサイザーを返す。
		@return default ILayerSynthesizer
	*/
	//const ILayerSynthesizer* GetDefaultLayerSynthesizer(){
	//	return LayerSynthesizerMG::GetDefaultLayerSynth();
	//}

	//////////////////////////////
	/*!
		補正方法選択コンボボックスをセットする
	*/
	void SetInputReviseComboBox(HWND hCombo);

	////////////////////////////////
	/*!
		補正方法の名前を指定して補正方法を変更
	*/
	void SelectInputRevise(const char* name);

	void GetInputReviseName(char* dst_name);

	///////////////////////////////
	/*!
	*/
	void AddIEToolMGEventListener(IEToolMGEventListener*  pListener){
		m_IEToolMG.AddEventListener( pListener );
	}

	///////////////////////////////
	/*
	*/
	void DeleteIEToolMGEventListener(IEToolMGEventListener*  pListener){
		m_IEToolMG.DeleteEventListener( pListener );
	}

	////////////////////////////////
	/*!
		ツールを指定する
		@param[in] id 指定するツールのポインタ
	*/
	void SetSelectTool(IIETool* pTool) {
		return m_IEToolMG.SetSelectTool( pTool );
	}

	////////////////////////////////
	/*!
		選択されているツールを返す
		@return 選択されているツールへのポインタ
	*/
	IIETool* GetSelectTool() {
		return m_IEToolMG.GetSelectTool();
	}

	/////////////////////////////////
	/*!
		保持しているツールグループの数を返す
	*/
	int GetSizeOfToolGroup() {
		return m_IEToolMG.GetSizeOfToolGroup();
	}

	IEToolGroup* GetToolGroupAtIndex(int index) {
		return m_IEToolMG.GetToolGroupAtIndex( index );
	}

	bool GetToolName(const char *disp_name, char* dst) {
		return m_IEToolMG.GetToolName(disp_name, dst);
	}

	/////////////////////////////////
	/*!
		ツール名を指定して、ツールへのポインタを得る
	*/
	IIETool* GetIEToolFromName(const char* tool_name) {
		return m_IEToolMG.GetIEToolFromName(tool_name);
	}

	/////////////////////////////////
	/*!
		ツールの表示名を取得する
	*/
	bool GetToolDisplayName(const char *tool_name, char* dst) {
		return m_IEToolMG.GetDisplayName(tool_name, dst);
	}

	////////////////////////////////
	/*!
		ツールエディットウィンドウハンドルをセット
		@param[in,out] pHandle
	*/
	void SetToolEditWndHandle(IToolEditWndHandle* pHandle){
		m_IEToolMG.SetToolEditWndHandle( pHandle );
	}

	void SetEffectTree(HWND hTree) {
		m_EffectMG.SetEditTree(hTree);
	}
	IEffect* CreateEffect(const char* effect_display_name) {
		return m_EffectMG.CreateEffect(effect_display_name);
	}

	void ReleaseEffect(IEffect* pEffect) {
		m_EffectMG.ReleaseEffect(pEffect);
	}

	//////////////////////////////////
	///*!
	//	マウスカーソルを選択中のブラシの形状へ変える
	//*/
	//VOID ChangeCusorToSelectBrush();

	////////////////////////////////
	/*!
		cm_idのコマンドを同期して実行する
		@param[in] cm_id 実行するコマンドのID
		@return コマンドを実行したらture そうでなければfalse
	*/
	bool RunCommand(int cm_id, void* pvoid);
	bool RunCommand(const char* cm_name, void* pvoid);

	////////////////////////////////
	/*!
		cm_idのコマンドを非同期に実行する
		@param[in] cm_id 実行するコマンドのID
		@return コマンドを実行したらture そうでなければfalse
	*/
	bool RunCommandAsync(int cm_id, void* pvoid);
	bool RunCommandAsync(const char* cm_name, void* pvoid);

	////////////////////////////////
	/*!
		名前cm_nameのコマンドのIDを得る。
		@param[in] cm_name IDを得るコマンドの名前
		@reutrn コマンドのID
	*/
	int GetCommandID(const char* cm_name);

	////////////////////////////////
	/*!
		最小のコマンドIDを返す。
		@return 最小のコマンドID
	*/
	int GetMinCommandID();

	////////////////////////////////
	/*!
		最大のコマンドのIDを返す。
		コマンドIDは連続であり、
		GetMinCommanID()+コマンド個数の値が返る
		@return 最大のコマンドID
	*/
	int GetMaxCommandID();


	IIECommand* GetRunningCommand();

	////////////////////////////////
	/*!
		エフェクトを非同期に実行する
		@param[in] cm_id 実行するコマンドのID
		@return コマンドを実行したらture そうでなければfalse
	*/
	bool RunEffectAsync(const char* effect_name);

	///////////////////////////////////
	///*!
	//	EffectEditDialogを返す。
	//*/
	//IEffectEditDialog* GetEffectEditDialog();

	/////////////////////////////////////
	/*!
		キーマップ操作オブジェクトへのポインタを返す。
	*/
	IEKeyMap* GetKeyMap();

	/////////////////////////////////////
	/*!
		パラメータ読み書きを行う
	*/
	bool ReadFromParamRegister(const IIEModule* pModule, const char* name, char* val);
	void WriteToParamRegister(const IIEModule* pModule, const char* name, const char* val);

	/////////////////////////////////////
	/*!
		すべてのファイルの画像更新
		@param[in] data 画像更新データフラグ
	*/
	void PushUpdateDataToAllFile(const LPUPDATE_DATA data);

	////////////////////////////////
	/*!
		アイドル時呼び出し
	*/
	VOID OnIdle();

	////////////////////////////////
	/*!
		左マウスボタンが押されたとき呼び出し
		@param[in] lpd マウスの入力情報
	*/
	VOID OnMouseLButtonDown(UINT nFlags, LPIE_INPUT_DATA lpd);

	////////////////////////////////
	/*!
		右マウスボタンが押されとき呼び出し
		@param[in] lpd マウスの入力情報
	*/
	VOID OnMouseRButtonDown(UINT nFlags, LPPOINT lps);
	
	////////////////////////////////
	/*!
		マウス移動時
		@param[in] lpd マウスの入力情報
	*/
	VOID OnMouseMove(UINT nFlags, LPIE_INPUT_DATA lpd);
	
	////////////////////////////////
	/*!
		左マウスボタンが離されるとき呼び出し
		@param[in] lpd マウスの入力情報
	*/
	VOID OnMouseLButtonUp(UINT nFlags, LPIE_INPUT_DATA lpd);
	
	////////////////////////////////
	/*!
		右マウスボタンが離されるとき呼び出し。
		@param[in] lps マウスの位置
	*/
	VOID OnMouseRButtonUp(UINT nFlags, LPPOINT lps);
	
	////////////////////////////////
	/*!
		キーボードのキーが押されたときに呼び出し
		@param[in] nChar 押されたキーの仮想コード
		@param[in] optChar 同時に押されているCtrl,Shiftキーなどの情報
		@return if hit a shortcut or tool  will return true, o/w return false
	*/
	bool OnKeyboardDown(UINT nChar, UINT optChar);

	////////////////////////////////
	/*!
		キーボードのキーが押されたときに呼び出し
		@param[in] nChar 押されたキーの仮想コード
		@param[in] optChar 同時に押されているCtrl,Shiftキーなどの情報
	*/
	VOID OnKeyboardUp(UINT nChar, UINT optChar);

	/////////////////////////////////
	/*!
		描画処理呼び出し
		@param[in] hdc
	*/
	VOID OnDraw(HDC hdc);

	////////////////////////////////
	/*!
		選択領域の上にマウスがあるかどうか？
		@param[in] pFile ファイル
		@param[in] lpt マウスの入力情報
		@return マウスが選択領域の上ならtrue そうでないならfalse
	*/
	bool IsOnSelectImage(ImgFile_Ptr pFile, LPPOINT lpt);
	/////////////////////////////////
	/*!
		エディットウィンドウの背景色を返す。
		@return 背景色
	*/
	IEColor GetEditWndBGColor() const {
		return m_IEConfig.GetEditWndBGColor();
	}
	/////////////////////////////////
	/*!
		エディットウィンドウの背景色を設定する。
		@param[in] color 設定する色
	*/
	void SetEditWndBGColor(IEColor color);

	unsigned int GetNumOfHistory() const {
		return m_IEConfig.GetNumOfHistory();
	}

	/////////////////////////////////
	/*!
		ヒストリ数を設定する
		@param[in] num
	*/
	void SetNumOfHistory(unsigned int num);

	///////////////////////////////////
	/*!
		使用するカーソルを指定する。
		カーソルの変更、描画はGUIで実装する
		@param[in] coursor_id 使用するカーソルID
	*/
	void SetIECursor(IE_CURSOR_ID cursor_id){ m_IECursorID = cursor_id; };
	IE_CURSOR_ID GetIECursor() const { return m_IECursorID; };

private:

	///////////////////////////////////
	inline void ResourceLock(unsigned int id){
#ifdef _WIN32
		switch( id ){
			case IE_RESOURCE_ID::IE_COMMAND:
				::EnterCriticalSection( &m_cs_ie_command );
				break;
			case IE_RESOURCE_ID::IE_TOOL:
				::EnterCriticalSection( &m_cs_ie_tool );
				break;
			default:
				assert( 0 );
				break;
		}
#else
		while((m_LockResourceID & id) == id); //is_lock_resource
		m_LockResourceID |= id;
#endif //_WIN32
	}
	inline void ResourceUnlock(unsigned int id){
#ifdef _WIN32
		switch( id ){
			case IE_RESOURCE_ID::IE_COMMAND:
				::LeaveCriticalSection( &m_cs_ie_command );
				break;
			case IE_RESOURCE_ID::IE_TOOL:
				::LeaveCriticalSection( &m_cs_ie_tool );
				break;
			default:
				assert( 0 );
				break;
		}
#else
		m_LockResourceID &= (~id);
#endif //_WIN32
	}

	//////////////////////////////////
	/*!
		コマンド終了処理
	*/
	void EndCommand();

	std::string m_StartDirPath;
	std::string m_ConfigINIFilePath;

	ImgEditEventListener_List m_ImgEditEventListener_List;
	ImgFile_Vec m_ImgFile_Vec;  //保持しているファイル配列

	IIEBrushPopupWnd* m_pIIEBrushPopupWnd;

	InputReviseMG m_InputReviseMG;
	EffectMG m_EffectMG;
	IEToolMG m_IEToolMG;
	IEBrushMG m_IEBrushMG;
	IEPaletteMG m_IEPaletteMG;

	IEKeyMap m_IEKeyMap;
	IEParamRegister m_IEParamRegister;
	IEEventCapture m_IEEventCapture;
	
	IIECommand* m_pIIECommand;
	void* m_pIECommandArg;

	ImgFile_Ptr m_pNowActiveFile;

	IEColor m_SelectFGColor;
	IEColor m_SelectBGColor;

	ImgEditConfig m_IEConfig;

	bool m_isInit;
	bool m_isLDrag;
	bool m_isKeyDown;
	DWORD m_KeyDownTime;
	IE_CURSOR_ID m_IECursorID;

#ifdef _WIN32
	CRITICAL_SECTION m_cs_ie_command;
	CRITICAL_SECTION m_cs_ie_tool;
#else
	unsigned int m_LockResourceID;
#endif //_WIN32
};

#endif //_IMGEDIT_H_