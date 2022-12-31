/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <string>
#include <vector>

#include "ini.h"
#include "common.h"
#include "keydef.h"
#include "vsurface.h"


// 設定項目(数値)
typedef enum {
	CV_Model = 0,
	CV_FDDrive,
	CV_OverClock,
	CV_MaxBoost60,
	CV_MaxBoost62,
	CV_StopBit,
	CV_Mode4Color,
	CV_ScanLineBr,
	CV_WindowZoom,
	CV_FrameSkip,
	CV_SampleRate,
	CV_SoundBuffer,
	CV_MasterVol,
	CV_PsgVolume,
	CV_PsgLPF,
	CV_VoiceVolume,
	CV_TapeVolume,
	CV_TapeLPF,
	CV_AviBpp,
	CV_AviZoom,
	CV_AviFrameSkip,
	CV_AviScanLineBr,
	CV_ExCartridge
} TCValue;

// 設定項目(bool)
typedef enum {
	CB_CheckCRC = 0,
	CB_Romaji,
	CB_FDDWait,
	CB_TurboTAPE,
	CB_BoostUp,
	CB_ScanLine,
	CB_Filtering,
	CB_DispNTSC,
	CB_FullScreen,
	CB_DispStatus,
	CB_AviScanLine,
	CB_AviDispNTSC,
	CB_AviFiltering,
	CB_CkDokoLoad,
	CB_CkQuit,
	CB_SaveQuit
} TCBool;

// 設定項目(path)
typedef enum {
	CF_ExtRom = 0,
	CF_Tape,
	CF_Save,
	CF_Disk1,
	CF_Disk2,
	CF_Printer,
	CF_RomPath,
	CF_TapePath,
	CF_DiskPath,
	CF_ExtRomPath,
	CF_ImgPath,
	CF_WavePath,
	CF_FontPath,
	CF_DokoPath
} TCPath;


// 設定項目定義
template <typename T> struct CfgSet{
	std::string Section;	// セクション
	std::string Entry;		// ノード
	TextID Comment;			// コメント
};

template <> struct CfgSet<TCValue>{
	std::string Section;	// セクション
	std::string Entry;		// ノード
	TextID Comment;			// コメント
	int Default;			// 初期値
	int Max;				// 最大値
	int Min;				// 最小値
};

template <> struct CfgSet<TCBool>{
	std::string Section;	// セクション
	std::string Entry;		// ノード
	TextID Comment;			// コメント
	bool Default;			// 初期値
};

template <> struct CfgSet<TCPath>{
	std::string Section;	// セクション
	std::string Entry;		// ノード
	TextID Comment;			// コメント
	P6VPATH Default;		// 初期値
	bool IsFile;			// true:ファイル false:パス
};


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class CFG6 : public cIni {
protected:
	// INIファイルに保存しないメンバ
	P6VPATH IniPath;		// INIファイルパス
	P6VPATH DokoFile;		// どこでもSAVEファイル名
	std::string Caption;	// ウィンドウキャプション
	
	// ビデオキャプチャ用一時保存
	int tmp_AviZoom;
	int tmp_AviFrameSkip;
	bool tmp_AviScanLine;
	int tmp_AviScanLineBr;
	bool tmp_AviDispNTSC;
	bool tmp_AviFiltering;
	
	
	void InitIni( bool );									// INIオブジェクト初期値設定
	
	const std::string& GetPCKeyName( PCKEYsym );			// 仮想キーコードから名称取得
	const std::string& GetP6KeyName( P6KEYsym );			// P6キーコードから名称取得
	PCKEYsym GetPCKeyCode( const std::string& );			// キー名称から仮想キーコードを取得
	P6KEYsym GetP6KeyCode( const std::string& );			// キー名称からP6キーコードを取得
	
	template <typename T> void SetDefault( const T&, const bool );	// 初期値設定

public:
	CFG6();
	~CFG6();
	
	bool Init();											// 初期化(INIファイル読込み)
	bool Write();											// INIファイル書込み
	
	// メンバアクセス関数
	template <typename T> auto GetValue( const T& ) -> decltype(CfgSet<T>::Default);	// 値設定(数値,bool,path)
	template <typename T1,typename T2> void SetValue( const T1&, const T2& );			// 値設定(数値,bool,path)
	int GetMax( TCValue ) const;							// 最大値取得
	int GetMin( TCValue ) const;							// 最小値取得
	int GetDefault( TCValue ) const;						// 初期値取得
	
	// [KEY] ----------------------------------------------------------------
	P6KEYsym GetVKey( PCKEYsym );							// キー定義取得
	void SetVKey( PCKEYsym, P6KEYsym );						//         設定
	
	int GetVKeyDef( std::vector<VKeyConv>& );				// キー定義配列取得
	
	// [COLOR] --------------------------------------------------------------
	COLOR24 GetColor( int );								// カラーデータ取得
	void SetColor( int, const COLOR24& );					//             設定
	
	// その他 ---------------------------------------------------------------
	const std::string& GetCaption();						// ウィンドウキャプション取得
	const P6VPATH GetDokoFile();							// どこでもSAVEファイル名取得
	void SetDokoFile( const P6VPATH& );						//                       設定
	
	// ビデオキャプチャ用一時保存 -------------------------------------------
	void PushAviPara();										// 退避
	void PopAviPara();										// 復帰
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* );		// どこでもSAVE
	bool DokoLoad( cIni* );		// どこでもLOAD
	// ----------------------------------------------------------------------
};

#endif	// CONFIG_H_INCLUDED
