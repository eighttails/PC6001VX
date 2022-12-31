/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <string>
#include <cstdio>
#include <vector>

#include "event.h"
#include "typedef.h"
#include "vsurface.h"


// メッセージ文字列取得用
enum TextID{
	T_EMPTY = 0,			// ""
	
	// 一般メッセージ
	T_QUIT,					// "終了してよろしいですか?"
	T_QUITC,				// "終了確認"
	T_RESTART0,				// "再起動してよろしいですか?"
	T_RESTART,				// "変更は再起動後に有効となります。\n今すぐ再起動しますか?"
	T_RESTARTC,				// "再起動確認"
	T_RESTARTI,				// "拡張カートリッジを挿入して再起動します。"
	T_RESTARTE,				// "拡張カートリッジを排出して再起動します。"
	T_RESETC,				// "リセット確認"
	T_DOKOC,				// "どこでもLOAD確認"
	T_DOKOSLOT,				// "どこでもLOADを実行してよろしいですか？"
	T_REPLAYRES,			// "リプレイを途中保存地点まで巻き戻します\nよろしいですか？"
	
	// INIファイル用メッセージ ------
	TINI_TITLE,				// "; === PC6001V 初期設定ファイル ===\n\n"
	// [CONFIG]
	TINI_Model,				// "機種 60:PC-6001 61:PC-6001A 62:PC-6001mkⅡ 66:PC-6601 64:PC-6001mkⅡSR 68:PC-6601SR"
	TINI_OverClock,			// "オーバークロック率 (1-1000)%"
	TINI_CheckCRC,			// "CRCチェック Yes:有効 No:無効"
	TINI_Romaji,			// "ローマ字入力 Yes:有効 No:無効"
	// [CMT]
	TINI_TurboTAPE,			// "Turbo TAPE Yes:有効 No:無効"
	TINI_BoostUp,			// "Boost Up Yes:有効 No:無効"
	TINI_MaxBoost60,		// "Boost Up 最大倍率(N60モード)"
	TINI_MaxBoost62,		// "Boost Up 最大倍率(N60m/N66モード)"
	TINI_StopBit,			// "ストップビット数 (2-10)bit"
	// [FDD]
	TINI_FDDrive,			// "ドライブ数 (0-2)"
	TINI_FDDWait,			// "アクセスウェイト Yes:有効 No:無効"
	// [DISPLAY]
	TINI_Mode4Color,		// "MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク"
	TINI_WindowZoom,		// "ウィンドウ表示倍率(%)"
	TINI_FrameSkip,			// "フレームスキップ"
	TINI_ScanLine,			// "スキャンライン Yes:あり No:なし"
	TINI_ScanLineBr,		// "スキャンライン輝度 (0-100)%"
	TINI_DispNTSC,			// "4:3表示 Yes:有効 No:無効"
	TINI_Filtering,			// "フィルタリング Yes:アンチエイリアシング No:ニアレストネイバー"
	TINI_FullScreen,		// "フルスクリーンモード Yes:有効 No:無効"
	TINI_DispStatus,		// "ステータスバー Yes:表示 No:非表示"
	// [MOVIE]
	TINI_AviBpp,			// "ビデオキャプチャ時色深度 (16,24,32)bit"
	TINI_AviZoom,			// "ビデオキャプチャ時ウィンドウ表示倍率(%)"
	TINI_AviFrameSkip,		// "ビデオキャプチャ時フレームスキップ"
	TINI_AviScanLine,		// "ビデオキャプチャ時スキャンライン Yes:あり No:なし"
	TINI_AviScanLineBr,		// "ビデオキャプチャ時スキャンライン輝度 (0-100)%"
	TINI_AviDispNTSC,		// "ビデオキャプチャ時4:3表示 Yes:有効 No:無効"
	TINI_AviFiltering,		// "ビデオキャプチャ時フィルタリング Yes:アンチエイリアシング No:ニアレストネイバー"
	// [SOUND]
	TINI_SampleRate,		// "サンプリングレート (44100/22050/11025)Hz"
	TINI_SoundBuffer,		// "サウンドバッファサイズ"
	TINI_MasterVolume,		// "マスター音量 (0-100)"
	TINI_PsgVolume,			// "PSG音量 (0-100)"
	TINI_PsgLPF,			// "PSG LPFカットオフ周波数(0で無効)"
	TINI_VoiceVolume,		// "音声合成音量 (0-100)"
	TINI_TapeVolume,		// "TAPEモニタ音量 (0-100)"
	TINI_TapeLPF,			// "TAPE LPFカットオフ周波数(0で無効)"
	// [FILES]
	TINI_ExtRom,			// "拡張ROMファイル名"
	TINI_tape,				// "TAPE(LODE)ファイル名(起動時に自動マウント)"
	TINI_save,				// "TAPE(SAVE)ファイル名(SAVE時に自動マウント)"
	TINI_disk1,				// "DISK1ファイル名(起動時に自動マウント)"
	TINI_disk2,				// "DISK2ファイル名(起動時に自動マウント)"
	TINI_printer,			// "プリンタ出力ファイル名"
	// [PATH]
	TINI_RomPath,			// "ROMイメージファイル格納パス"
	TINI_TapePath,			// "TAPEイメージファイル格納パス"
	TINI_DiskPath,			// "DISKイメージファイル格納パス"
	TINI_ExtRomPath,		// "拡張ROMイメージファイル格納パス"
	TINI_ImgPath,			// "スナップショットファイル格納パス"
	TINI_WavePath,			// "WAVEファイル格納パス"
	TINI_FontPath,			// "FONTファイル格納パス"
	TINI_DokoPath,			// "どこでもSAVEファイル格納パス"
	// [CHECK]
	TINI_CkDokoLoad,		// "どこでもLOAD(SLOT)実行時確認 Yes:する No:しない"
	TINI_CkQuit,			// "終了時確認 Yes:する No:しない"
	TINI_SaveQuit,			// "終了時INIファイルを保存 Yes:する No:しない"
	// [OPTION]
	TINI_ExCartridge,		// "拡張カートリッジ 0:なし"
	
	// どこでもSAVE用メッセージ ------
	TDOK_TITLE,				// "; === PC6001V どこでもSAVEファイル ===\n\n"
	
	// Error用メッセージ ------
	TERR_ERROR,				// "Error"
	TERR_WARNING,			// "Warning"
	TERR_NoError,			// "エラーはありません"
	TERR_Unknown,			// "原因不明のエラーが発生しました"
	TERR_MemAllocFailed,	// "メモリの確保に失敗しました"
	TERR_RomChange,			// "指定された機種のROMイメージが見つからないため機種を変更しました"
	TERR_NoRom,				// "ROMイメージが見つかりません\n設定とファイル名を確認してください"
	TERR_NoRomChange,		// "ROMイメージが見つかりません\n他の機種を検索しますか？"
	TERR_RomSizeNG,			// "ROMイメージのサイズが不正です"
	TERR_RomCrcNG,			// "ROMイメージのCRCが不正です\nCRCが一致しないROMを使用すると、予期せぬ不具合を引き起こす可能性があります。\nそれでも起動しますか？"
	TERR_LibInitFailed,		// "ライブラリの初期化に失敗しました"
	TERR_InitFailed,		// "初期化に失敗しました\n設定を確認してください"
	TERR_FontLoadFailed,	// "フォントの読込みに失敗しました"
	TERR_FontCreateFailed,	// "フォントファイルの作成に失敗しました"
	TERR_IniDefault,		// "INIファイルの読込みに失敗しました\nデフォルト設定で起動します"
	TERR_IniReadFailed,		// "INIファイルの読込みに失敗しました"
	TERR_IniWriteFailed,	// "INIファイルの保存に失敗しました"
	TERR_TapeMountFailed,	// "TAPEイメージのマウントに失敗しました"
	TERR_DiskMountFailed,	// "DISKイメージのマウントに失敗しました"
	TERR_ExtRomMountFailed,	// "拡張ROMイメージのマウントに失敗しました"
	TERR_DokoReadFailed,	// "どこでもLOADに失敗しました"
	TERR_DokoWriteFailed,	// "どこでもSAVEに失敗しました"
	TERR_DokoDiffVersion,	// "どこでもSAVEファイルのバージョンが異なるため\n正しく動作しない可能性があります\n続けますか？"
	TERR_ReplayPlayError,	// "リプレイ再生に失敗しました"
	TERR_ReplayRecError,	// "リプレイ記録に失敗しました"
	TERR_NoReplayData,		// "リプレイデータがありません"
	TERR_CaptureFailed		// "ビデオキャプチャに失敗しました"
};




/////////////////////////////////////////////////////////////////////////////
// 文字コード操作関数
/////////////////////////////////////////////////////////////////////////////
// SJIS -> P6
bool Sjis2P6( std::string&, const std::string& );


/////////////////////////////////////////////////////////////////////////////
// 文字列操作関数
/////////////////////////////////////////////////////////////////////////////
// 小文字による文字列比較
int StriCmp( const std::string&, const std::string& );


/////////////////////////////////////////////////////////////////////////////
// 画像ファイル操作関数
/////////////////////////////////////////////////////////////////////////////
// Img SAVE from Data
bool SaveImgData( const P6VPATH&, BYTE*, const int, const int, const int, VRect* );
// Img SAVE from VSurface
bool SaveImg( const P6VPATH&, VSurface*, VRect* );
// Img LOAD to VSurface
VSurface* LoadImg( const P6VPATH& );


/////////////////////////////////////////////////////////////////////////////
// 文字列取得関数等
/////////////////////////////////////////////////////////////////////////////
// メッセージ文字列取得
const std::string GetText( TextID );
// 色の名前取得
const std::string GetColorName( int );
// 拡張カートリッジの名前取得
const std::string GetExtCartName( int );
// キーの名前取得
const std::string GetKeyName( PCKEYsym );
// キーの文字コード取得
BYTE GetKeyChar( PCKEYsym, bool );


/////////////////////////////////////////////////////////////////////////////
// std::string 書式出力
//
// 引数:	fmt			書式文字列への参照
//			args		printf相当の引数
// 返値:	std::string	書式が反映された文字列
/////////////////////////////////////////////////////////////////////////////
template <typename ... Args> std::string Stringf( const std::string& fmt, Args ... args )
{
	std::vector<char> buf( std::snprintf( nullptr, 0, fmt.c_str(), args ... ) + 1 );
	std::snprintf( &buf[0], buf.size(), fmt.c_str(), args ... );
	return std::string( buf.data(), buf.data() + buf.size() - 1 );
}



#endif	// COMMON_H_INCLUDED
