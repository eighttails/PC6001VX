#ifndef VOICE_H_INCLUDED
#define VOICE_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "sound.h"
#include "p6device.h"
#include "device/pd7752.h"

// [OUT]
//  E0H : 音声パラメータ転送
//  E1H : ?
//  E2H : モードセット : 0 0 0 0 F S1 S0
//			F     : 分析フレーム周期 0: 10ms/フレーム
//									 1: 20ms/フレーム
//			S1,S0 : 発声速度BIT     00: NORMAL SPEED
//									01: SLOW SPEED
//									10: FAST SPEED
//									11: 禁止

//  E3H : コマンドセット
//		・内部句選択コマンド : 0  0 P5 P4 P3 P2 P1 P0
//			P5-P0 : 句選択ビット(0-63)
//		・ストップコマンド   : 1  1  1  1  1  1  1  1
//		・外部句選択コマンド : 1  1  1  1  1  1  1  0
//

// [IN]
//  E0H : ステータスレジスタ : BSY REQ ~INT/EXT ERR 0 0 0 0
//			BSY      : 音声合成 1:処理中 0:停止中
//			REQ      : 音声パラメータ 1:入力要求 0:禁止
//			~INT/EXT : メッセージデータ 1:外部 0:内部
//			ERR      : エラーフラグ 1:エラー 0:エラーなし
//  E1H : ?
//  E2H : PortE2に書込んだ値?
//  E3H : PortE3に書込んだ値?


// 処理タイミングに関する挙動
// [本来のスペック]
// ・コマンド発行から1フレーム目のパラメータ入力完了まで2ms以内(NORMAL SPEED)
//   【疑問】コマンド発行からREQまでの時間は?
// ・フレーム内で最初のREQから全パラメータ入力まで3/4フレーム時間以内
//   【考察】入力完了から発声までは1/4フレーム時間以内で確実に処理される?
//   【疑問】次のREQまでの時間は?
//
// [実装スペック]
// ・コマンド発行から1フレーム目のパラメータ入力完了まで1フレーム時間以内
// ・1フレーム時間経過時点で発声(1フレーム分のサンプル生成)，同時にREQ=1
// ・発声から次フレームのパラメータ入力完了まで1フレーム時間以内


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class VCE6 : public P6DEVICE, public Device, public cD7752, public SndDev, public IDoko {
private:
	char FilePath[PATH_MAX];		// WAVEファイル格納パス
	
	BYTE io_E0H;
	BYTE io_E2H;
	BYTE io_E3H;
	
	int VStat;						// ステータス
	
	// 内部句関係
	int IVLen;						// サンプル数
	int *IVBuf;						// データバッファ
	int IVPos;						// 読込みポインタ
	
	// パラメータバッファ(1フレームのパラメータ数は7個)
	BYTE ParaBuf[7];				// パラメータバッファ
	int	Pnum;						// パラメータ数
	int Fnum;						// 繰り返しフレーム数
	BOOL PReady;					// パラメータセット完了
	
	D7752_SAMPLE *Fbuf;				// フレームバッファポインタ(10kHz 1フレーム)
	
	void VSetMode( BYTE );			// モードセット
	void VSetCommand( BYTE );		// コマンドセット
	void VSetData( BYTE );			// 音声パラメータ転送
	int VGetStatus();				// ステータスレジスタ取得
	void AbortVoice();				// 発声停止
	void UpConvert();				// サンプリングレート変換
	BOOL LoadVoice( int );			// 内部句WAV読込み
	void FreeVoice();				// 内部句WAV開放
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutE0H( int, BYTE );
	void OutE2H( int, BYTE );
	void OutE3H( int, BYTE );
	BYTE InE0H( int );
	BYTE InE2H( int );
	BYTE InE3H( int );
	
public:
	VCE6( VM6 *, const ID& );		// コンストラクタ
	~VCE6();						// デストラクタ
	
	void EventCallback( int, int );	// イベントコールバック関数
	
	BOOL Init( int, char * );		// 初期化
	void Reset( void );				// リセット
	
	int SoundUpdate( int );			// ストリーム更新
	
	// デバイスID
	enum IDOut{ outE0H=0, outE2H, outE3H };
	enum IDIn {  inE0H=0,  inE2H,  inE3H };
	
	// ------------------------------------------
	BOOL DokoSave( cIni * );	// どこでもSAVE
	BOOL DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};

#endif	// VOICE_H_INCLUDED
