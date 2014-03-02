// FPS計測クラスヘッダーファイル

#pragma once
#pragma comment(lib, "winmm.lib")

#include <list>

using namespace std;


#define FPSCOUNTER_QUERYPER_COUNTER        1
#define FPSCOUNTER_TIMEGETTIME             2

class CFPSCounter
{
private:
    int m_iCounterFlag;                // 使用する計測関数の判定フラグ
    LARGE_INTEGER m_Counter;           // クロックカウント数
    double m_dFreq;                    // 1秒当たりクロックカウント数（周波数）
    LONGLONG m_OldLongCount;           // 以前のクロックカウント数
    DWORD m_dwTGTOldCount;             // 以前の時刻（ミリ秒）
    list<double> m_dwDefTimeLst;       // 時間リスト
    UINT m_uiNum;                      // 移動平均計算時のデータ数
    double m_dwSumTimes;               // 共通部分の合計値

public:
    CFPSCounter(unsigned int smp = 10);
public:
    virtual ~CFPSCounter(void);

    // FPS値を取得
    double GetFPS();

    // サンプル数を変更
    void SetSampleNum( unsigned int smp);

protected:
    // 現在の時刻を取得
    double GetCurDefTime();

    // FPSを更新
    double UpdateFPS( double Def );
};
