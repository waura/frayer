#include "StdAfx.h"
// FPS計測クラス実装ファイル

#include "FPSCounter.h"

// コンストラクタ
CFPSCounter::CFPSCounter(unsigned int smp)
{
    // サンプル数の設定
    SetSampleNum(smp);

    // 計測する時計の選択
    if(QueryPerformanceCounter( &m_Counter ) != 0)
    {
        // QueryPerformanceCounter関数を使うフラグ
        m_iCounterFlag = FPSCOUNTER_QUERYPER_COUNTER;
        m_OldLongCount = m_Counter.QuadPart;        // 生成時の時刻（クロック数）を取得
        LARGE_INTEGER Freq;
        QueryPerformanceFrequency( &Freq );            // 1秒当たりクロック数を取得
        m_dFreq = (double)Freq.QuadPart;
    }
    else
    {
        // timeGetTime関数を使うフラグ
        m_iCounterFlag = FPSCOUNTER_TIMEGETTIME;

        // 精度を上げる
        timeBeginPeriod(1);

        // 生成時の時刻（ミリ秒）を取得
        m_dwTGTOldCount = timeGetTime();
    }

    // 計測
    GetFPS();
}


// デストラクタ
CFPSCounter::~CFPSCounter(void)
{
    if(m_iCounterFlag == FPSCOUNTER_TIMEGETTIME)
        timeEndPeriod(1);    // タイマーの精度を戻す
}


// FPS値を取得
double CFPSCounter::GetFPS()
{
    double Def = GetCurDefTime();
    if(Def == 0){
        // 計算できないのでを返す
        return 0;
    }

    return UpdateFPS( Def );
}


// 現在の差分時刻をミリ秒単位で取得
double CFPSCounter::GetCurDefTime()
{
    // 差分時間を計測
    if(m_iCounterFlag == FPSCOUNTER_QUERYPER_COUNTER)
    {
        // QueryPerformanceCounter関数による計測
        QueryPerformanceCounter( &m_Counter );                     // 現在の時刻を取得し、
        LONGLONG LongDef = m_Counter.QuadPart - m_OldLongCount;    // 差分カウント数を算出する。
        double dDef = (double)LongDef;                             // 倍精度浮動小数点に変換
        m_OldLongCount = m_Counter.QuadPart;                       // 現在の時刻を保持
        return dDef*1000 / m_dFreq;                                // 差分時間をミリ秒単位で返す
    }

    // timeGetTime関数による計測
    DWORD CurTime = timeGetTime();
    DWORD CurDef = CurTime - m_dwTGTOldCount;         // 差分カウント数を算出する
    m_dwTGTOldCount = CurTime;                        // 現在の時刻を保持
    return CurDef;
}


// FPSを更新
double CFPSCounter::UpdateFPS( double Def )
{
    // 最も古いデータを消去
    m_dwDefTimeLst.pop_front();

    // 新しいデータを追加
    m_dwDefTimeLst.push_back( Def );

    // FPS算出
    double FPS;
    double AveDef = (m_dwSumTimes + Def) / m_uiNum;
    if(AveDef != 0)
        FPS = 1000.0 / AveDef;

    // 共通加算部分の更新
    m_dwSumTimes += Def - *m_dwDefTimeLst.begin();

    return FPS;
}


// サンプル数を変更
void CFPSCounter::SetSampleNum( unsigned int smp)
{
    m_uiNum = smp;    // 平均を計算する個数
    m_dwDefTimeLst.resize(m_uiNum, 0.0);    // リスト初期化
    m_dwSumTimes = 0;
} 
