#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "ini.h"
#include "keydef.h"
#include "vsurface.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class CFG6 {
protected:
	cIni *Ini;								// INIオブジェクトポインタ
	
	// INIファイルに保存しないメンバ
	char IniPath[PATH_MAX];					// INIファイルパス
	char Caption[256];						// ウィンドウキャプション
	char DokoFile[PATH_MAX];				// どこでもSAVEファイル名
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	bool MonDisp;							// モニタウィンドウ表示状態 true:表示 false:非表示
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	
	// INIファイルに保存するメンバ
	char RomPath[PATH_MAX];					// ROMパス
	char ExtRomPath[PATH_MAX];				// 拡張ROMパス
	char ExtRomFile[PATH_MAX];				// 拡張ROMファイル名
	char WavePath[PATH_MAX];				// WAVEパス
	char TapePath[PATH_MAX];				// TAPEパス
	char TapeFile[PATH_MAX];				// TAPEファイル名
	char SaveFile[PATH_MAX];				// TAPE(SAVE)ファイル名
	char DiskPath[PATH_MAX];				// DISKパス
	char DiskFile1[PATH_MAX];				// DISK1ファイル名
	char DiskFile2[PATH_MAX];				// DISK2ファイル名
	char ImgPath[PATH_MAX];					// スクリーンショット格納パス
	char PrinterFile[PATH_MAX];				// プリンタファイル名
	
	// 定数
	static const P6KeyName P6KeyNameDef[];	// P6キーコード 名称定義
	static const PCKeyName PCKeyNameDef[];	// 仮想キーコード 名称定義
	static const VKeyConv KeyIni[];			// 仮想キーコード -> P6キーコード定義初期値
	static const COLOR24 STDColor[];		// 標準カラーデータ
	
	
	void InitIni( cIni *, bool );			// INIオブジェクト初期値設定
	const char *GetPCKeyName( PCKEYsym );	// 仮想キーコードから名称取得
	const char *GetP6KeyName( P6KEYsym );	// P6キーコードから名称取得
	PCKEYsym GetPCKeyCode( char * );		// キー名称から仮想キーコードを取得
	P6KEYsym GetP6KeyCode( char * );		// キー名称からP6キーコードを取得

public:
	CFG6();								// コンストラクタ
	virtual ~CFG6();						// デストラクタ
	
	bool Init();							// 初期化(INIファイル読込み)
	bool Write();							// INIファイル書込み
	
	// メンバアクセス関数
	// 共通
	int GetModel();							// 機種取得
	void SetModel( int );					//     設定
	char *GetCaption();						// ウィンドウキャプション取得
	char *GetDokoFile();					// どこでもSAVEファイル名取得
	void SetDokoFile( const char * );		//                       設定
	int GetOverClock();						// オーバークロック率取得
	void SetOverClock( int );				//                   設定
	
	// メモリ関係
	char *GetRomPath();						// ROMパス取得
	void SetRomPath( const char * );		//        設定
	bool GetUseExtRam();					// 拡張RAMを使う取得
	void SetUseExtRam( bool );				//              設定
	char *GetExtRomPath();					// 拡張ROMパス取得
	void SetExtRomPath( const char * );		//            設定
	char *GetExtRomFile();					// 拡張ROMファイル名取得
	void SetExtRomFile( const char * );		//                  設定
	bool GetCheckCRC();						// CRCチェック取得
	void SetCheckCRC( bool );				//            設定
	bool GetRomPatch();						// ROMパッチ取得
	void SetRomPatch( bool );				//          設定
	
	// キーボード関係
	int GetKeyRepeat();						// キーリピート取得
	void SetKeyRepeat( int );				//             設定
	
	// サウンド関係
	int GetSampleRate();					// サンプリングレート取得
	void SetSampleRate( int );				//                   設定
	int GetSoundBuffer();					// サウンドバッファ長倍率取得
	void SetSoundBuffer( int );				//                       設定
	int GetMasterVol();						// マスター音量取得
	void SetMasterVol( int );				//             設定
	
	// PSG関係
	int GetPsgVol();						// PSG音量取得
	void SetPsgVol( int );					//        設定
	int GetPsgLPF();						// PSG LPFカットオフ周波数取得
	void SetPsgLPF( int );					//                        設定
	
	// 音声合成関係
	char *GetWavePath();					// WAVEパス取得
	void SetWavePath( const char * );		//         設定
	int GetVoiceVol();						// 音声合成音量取得
	void SetVoiceVol( int );				//             設定
	
	// CMT関係
	char *GetTapePath();					// TAPEパス取得
	void SetTapePath( const char * );		//         設定
	char *GetTapeFile();					// TAPEファイル名取得
	void SetTapeFile( const char * );		//               設定
	int GetCmtVol();						// TAPEモニタ音量取得
	void SetCmtVol( int );					//               設定
	bool GetTurboTAPE();					// Turbo TAPE 有効フラグ取得
	void SetTurboTAPE( bool );				//                      設定
	bool GetBoostUp();						// BoostUp 有効フラグ取得
	void SetBoostUp( bool );				//                   設定
	int GetMaxBoost1();						// BoostUp 最大倍率(N60モード)取得
	void SetMaxBoost1( int );				//                            設定
	int GetMaxBoost2();						// BoostUp 最大倍率(N60m/N66モード)取得
	void SetMaxBoost2( int );				//                                 設定
	char *GetSaveFile();					// TAPE(SAVE)ファイル名取得
	void SetSaveFile( const char * );		//                     設定
	
	// DISK関係
	char *GetDiskPath();					// DISKパス取得
	void SetDiskPath( const char * );		//         設定
	char *GetDiskFile( int );				// DISKファイル名取得
	void SetDiskFile( int, const char * );	//               設定
	int GetFddNum();						// FDD接続台数取得
	void SetFddNum( int );					//            設定
	
	// 画面関係
	char *GetImgPath();						// スクリーンショット格納パス取得
	void SetImgPath( const char * );		//                           設定
	int GetScrBpp();						// カラーモード取得
	void SetScrBpp( int );					//             設定
	int GetMode4Color();					// モード4カラーモード取得
	void SetMode4Color( int );				//                    設定
	bool GetScanLine();						// スキャンライン取得
	void SetScanLine( bool );				//               設定
	int GetScanLineBr();					// スキャンライン輝度取得
	void SetScanLineBr( int );				//                   設定
	bool GetDispNTSC();						// 4:3表示取得
	void SetDispNTSC( bool );				//        設定
	bool GetFullScreen();					// フルスクリーン取得
	void SetFullScreen( bool );				//               設定
	bool GetStatDisp();						// ステータスバー表示状態取得
	void SetStatDisp( bool );				//                       設定
	int GetFrameSkip();						// フレームスキップ取得
	void SetFrameSkip( int );				//                 設定

	// ビデオキャプチャ関係
	bool GetAviRle();						// RLEフラグ取得
	void SetAviRle( bool );					//          設定
	
	// プリンタ関係
	char *GetPrinterFile();					// プリンタファイル名取得
	void SetPrinterFile( const char * );	//                   設定
	
	// 確認関係
	bool GetCkQuit();						// 終了時確認取得
	void SetCkQuit( bool );					//           設定
	bool GetSaveQuit();						// 終了時INI保存取得
	void SetSaveQuit( bool );				//              設定
	
	// 特殊機能・拡張機能関係
	bool GetUseSoldier();					// 戦士のカートリッジ使うフラグ取得
	void SetUseSoldier( bool );				//                             設定
	
	// パレット関係
	COLOR24 *GetColor( int, COLOR24 * );	// カラーデータ取得
	void SetColor( int, const COLOR24 * );	//             設定
	
	// キー定義関係
	P6KEYsym GetVKey( PCKEYsym );			// キー定義取得
	void SetVKey( PCKEYsym, P6KEYsym );		//         設定
	int GetVKeyDef( VKeyConv ** );			// キー定義配列取得
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	bool GetMonDisp();						// モニタウィンドウ表示状態取得
	void SetMonDisp( bool );				// モニタウィンドウ表示状態設定
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};

#endif	// CONFIG_H_INCLUDED
