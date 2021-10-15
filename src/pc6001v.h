/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef PC6001V_H_INCLUDED
#define PC6001V_H_INCLUDED


/////////////////////////////////////////////////////////////////////////////
// ビルドオプション (不要な項目はコメントアウトする)
/////////////////////////////////////////////////////////////////////////////
//#define	NOMONITOR	// モニタモードなし
//#define	USEFMGEN	// PSGにfmgenを使う


/////////////////////////////////////////////////////////////////////////////
// バージョン情報,機種名など
/////////////////////////////////////////////////////////////////////////////
#define	VERSION					"4.0.0-alpha"
#define	APPNAME					"PC6001VX"

#define	P60NAME					"PC-6001"
#define	P61NAME					"PC-6001A"
#define	P62NAME					"PC-6001mkⅡ"
#define	P66NAME					"PC-6601"
#define	P64NAME					"PC-6001mkⅡSR"
#define	P68NAME					"PC-6601SR"


/////////////////////////////////////////////////////////////////////////////
// オプション 初期値
/////////////////////////////////////////////////////////////////////////////
#define	DEFAULT_MODEL			(0)					// 機種 60:PC-6001 61:PC-6001A 62:PC-6001mkⅡ 66:PC-6601 64:PC-6001mkⅡSR 68:PC-6601SR (0:自動選定)
#define	DEFAULT_USEEXTRAM		(1)					// 拡張RAM 0:なし 1:あり
#define	DEFAULT_SAMPLERATE		(44100)				// サンプリングレート
#define	DEFAULT_SOUNDBUF		(1)					// サウンドバッファ長倍率(基本長はVSYNC)
#define	DEFAULT_MASTERVOL		(70)				// マスター音量
#define	DEFAULT_PSGVOL			(60)				// PSG音量
#define	DEFAULT_PSGLPF			(0)					// PSG LPFカットオフ周波数(0で無効)
#define	DEFAULT_VOICEVOL		(100)				// 音声合成音量
#define	DEFAULT_TAPEVOL			(10)				// TAPEモニタ音量
#define	DEFAULT_TAPELPF			(1540)				// TAPE LPFカットオフ周波数(0で無効)
#define	DEFAULT_TURBO			(true)				// Turbo TAPE Yes:有効 No:無効
#define	DEFAULT_BOOST			(false)				// BoostUp Yes:有効 No:無効
#define DEFAULT_MAXBOOST60		(7)					// BoostUp最大倍率(N60モード)
#define DEFAULT_MAXBOOST62		(5)					// BoostUp最大倍率(N60m/N66モード)
#define	DEFAULT_BAUD			(1200)				// CMTボーレート
#define	DEFAULT_STOPBIT			(3)					// CMTストップビット数
#define	DEFAULT_FDDrive			(0)					// ドライブ数
#define	DEFAULT_FDDWAIT			(true)				// アクセスウェイト true:有効 false:無効
#define	DEFAULT_MODE4COLOR		(1)					// モード４カラーモード 0:モノ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク
#define	DEFAULT_SCANLINE		(true) 				// スキャンライン true:あり false:なし
#define	DEFAULT_SCANLINEBR		(75)				// スキャンライン輝度 (0-100)%
#define	DEFAULT_DISPNTSC		(true) 				// 4:3表示 true:有効 false:無効
#define	DEFAULT_FILTERING		(true) 				// フィルタリング true:アンチエイリアシング false:ニアレストネイバー
#define	DEFAULT_FRAMESKIP		(0)					// フレームスキップ
#define	DEFAULT_WINDOWZOOM		(200)				// ウィンドウ表示倍率
#define	DEFAULT_OVERCLOCK		(100)				// オーバークロック率
#define	DEFAULT_CHECKCRC		(true) 				// CRCチェック
#define	DEFAULT_FULLSCREEN		(false) 			// フルスクリーン
#define	DEFAULT_DISPSTATUS		(true) 				// ステータスバー表示状態
#define	DEFAULT_AVIBPP			(24) 				// ビデオキャプチャ色深度
#define	DEFAULT_AVIZOOM			(100) 				// ビデオキャプチャ時ウィンドウ表示倍率
#define	DEFAULT_AVIFRMSKIP		(1)					// ビデオキャプチャ時フレームスキップ
#define	DEFAULT_AVISCANLINE		(false)				// ビデオキャプチャ時スキャンライン true:あり false:なし
#define	DEFAULT_AVISCANLINEBR	(75)				// ビデオキャプチャ時スキャンライン輝度 (0-100)%
#define	DEFAULT_AVIDISPNTSC		(true) 				// ビデオキャプチャ時4:3表示 true:有効 false:無効
#define	DEFAULT_AVIFILTERING	(true) 				// ビデオキャプチャ時フィルタリング true:アンチエイリアシング false:ニアレストネイバー
#define	DEFAULT_CKQUIT			(false) 			// 終了時確認
#define	DEFAULT_SAVEQUIT		(true) 				// 終了時INI保存
#define	DEFAULT_EXCARTRIDGE		(0) 				// 拡張カートリッジ

#define	MIN_MODEL				(60)				// 機種 最小値
#define	MAX_MODEL				(68)				// 機種 最大値
#define	MIN_FDDrive				(0)					// ドライブ数 最小値
#define	MAX_FDDrive				(2)					// ドライブ数 最大値
#define	MIN_BOOST				(1)					// BoostUp最大倍率 最小値
#define	MAX_BOOST				(10)				// BoostUp最大倍率 最大値
#define	MIN_STOPBIT				(2)					// CMTストップビット数 最小値
#define	MAX_STOPBIT				(10)				// CMTストップビット数 最大値
#define	MIN_SAMPLERATE			(11025)				// サンプリングレート 最小値
#define	MAX_SAMPLERATE			(44100)				// サンプリングレート 最大値
#define	MIN_SOUNDBUF			(1)					// サウンドバッファ長倍率 最小値
#define	MAX_SOUNDBUF			(5)					// サウンドバッファ長倍率 最大値
#define	MIN_VOLUME				(0)					// 音量 最小値
#define	MAX_VOLUME				(100)				// 音量 最大値
#define	MIN_LPF					(0)					// LPFカットオフ周波数 最小値
#define	MAX_LPF					(20000)				// LPFカットオフ周波数 最大値
#define	MIN_MODE4COLOR			(0)					// モード４カラーモード 最小値
#define	MAX_MODE4COLOR			(4)					// モード４カラーモード 最大値
#define	MIN_SCANLINEBR			(0)					// スキャンライン輝度 最小値
#define	MAX_SCANLINEBR			(100)				// スキャンライン輝度 最大値
#define	MIN_FRAMESKIP			(0)					// フレームスキップ 最小値
#define	MAX_FRAMESKIP			(5)					// フレームスキップ 最大値
#define	MIN_WINDOWZOOM			(50)				// ウィンドウ表示倍率 最小値
#define	MAX_WINDOWZOOM			(400)				// ウィンドウ表示倍率 最大値
#define	MIN_OVERCLOCK			(1)					// オーバークロック率 最小値
#define	MAX_OVERCLOCK			(1000)				// オーバークロック率 最大値
#define	MIN_AVIBPP				(16)				// ビデオキャプチャ色深度 最小値
#define	MAX_AVIBPP				(32)				// ビデオキャプチャ色深度 最大値
#define	MIN_AVIZOOM				(100)				// ビデオキャプチャ時ウィンドウ表示倍率 最小値
#define	MAX_AVIZOOM				(400)				// ビデオキャプチャ時ウィンドウ表示倍率 最大値

#define	CPUM_CLOCK60			(3993600)			// メインCPUクロック(Hz) 60,62
#define	CPUM_CLOCK66			(4000000)			// メインCPUクロック(Hz) 66
#define	CPUM_CLOCK64			(3580000)			// メインCPUクロック(Hz) 64,68
#define	CPUS_CLOCK60			(CPUM_CLOCK60*2)	// サブCPUクロック(Hz) 60,62
#define	CPUS_CLOCK66			(CPUM_CLOCK66*2)	// サブCPUクロック(Hz) 66
#define	CPUS_CLOCK64			(CPUM_CLOCK64*2)	// サブCPUクロック(Hz) 64,68
#define	PSG_CLOCK60				(CPUM_CLOCK60/2)	// PSGクロック(Hz) 60,62
#define	PSG_CLOCK66				(CPUM_CLOCK66/2)	// PSGクロック(Hz) 66
#define	PSG_CLOCK64				(4000000)			// PSGクロック(Hz) 64,68 CPUクロックとは別

#define	VSYNC_HZ				(59.922)			// VSYNC周波数


/////////////////////////////////////////////////////////////////////////////
// 各種ディレクトリ名 初期値
/////////////////////////////////////////////////////////////////////////////
#define	DIR_ROM					"rom"				// ROMイメージ
#define	DIR_TAPE				"tape"				// TAPEイメージ
#define	DIR_DISK				"disk"				// DISKイメージ
#define	DIR_EXTROM				"extrom"			// 拡張ROMイメージ
#define	DIR_IMAGE				"img"				// スナップショット
#define	DIR_WAVE				"wave"				// WAVEファイル
#define	DIR_FONT				"font"				// フォント
#define	DIR_DOKO				"doko"				// どこでもSAVEファイル
#define	DIR_CONFIG				"P6V"				// 設定ファイル


/////////////////////////////////////////////////////////////////////////////
// 拡張子
/////////////////////////////////////////////////////////////////////////////
#define	EXT_IMG					"png"				// 画像ファイル拡張子
#define	EXT_RES					"resume"			// リプレイ途中保存用拡張子
#define	EXT_P6RAW				"p6"				// TAPEイメージ(RAW)
#define	EXT_CAS					"cas"				// TAPEイメージ(RAW)
#define	EXT_P6T					"p6t"				// TAPEイメージ(P6T)
#define	EXT_DISK				"d88"				// DISKイメージ
#define	EXT_ROM1				"rom"				// 拡張ROMイメージ
#define	EXT_ROM2				"bin"				// 拡張ROMイメージ
#define	EXT_DOKO				"dds"				// どこでもSAVE
#define	EXT_REPLAY				"ddr"				// リプレイ
#define	EXT_BASIC				"bas"				// BASICプログラム(打ち込み代行用)
#define	EXT_TEXT				"txt"				// テキストファイル
#define	EXT_VIDEO				"webm"				// ビデオキャプチャ


/////////////////////////////////////////////////////////////////////////////
// 各種ファイル名 初期値
/////////////////////////////////////////////////////////////////////////////
#define	FILE_SNAP				"P6V"				// スナップショットプレフィックス
#define	FILE_CONFIG				"pc6001v.ini"		// 設定ファイル
#define	FILE_PRINT				"printer.txt"		// プリンタ出力ファイル
#define	FILE_SERIAL				"serial.txt"		// シリアル出力ファイル
#define	FILE_SAVE				"_csave." EXT_P6T	// TAPE(CSAVE)ファイル
#define	FILE_FONTZ				"fontz12." EXT_IMG	// 半角フォントファイル
#define	FILE_FONTH				"fonth12." EXT_IMG	// 全角フォントファイル

#define	SUBCPU60				"subcpu.60"			// サブCPU ROM(PC-6001)
#define	SUBCPU61				"subcpu.61"			// サブCPU ROM(PC-6001A)
#define	SUBCPU62				"subcpu.62"			// サブCPU ROM(PC-6001mk2)
#define	SUBCPU66				"subcpu.66"			// サブCPU ROM(PC-6601)
#define	SUBCPU64				"subcpu.64"			// サブCPU ROM(PC-6001mk2SR)
#define	SUBCPU68				"subcpu.68"			// サブCPU ROM(PC-6601SR)


/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジ定義
/////////////////////////////////////////////////////////////////////////////
#define	EXCSOL					0b1000000000000000	// 戦士のカートリッジ
#define	EXCSND					0b0100000000000000	// サウンド出力あり
#define	EXCFIX					0b0010000000000000	// ROM固定
#define	EXCBUS					0b0001000000000000	// ROM バスアクセス
#define	EXCROM					0b0000100000000000	// ROMあり
#define	EXCRAM					0b0000010000000000	// RAMあり

#define	EXC6001					(              EXCFIX|EXCBUS|       EXCROM|1)	// PCS-6001R	拡張BASIC
#define	EXC6005					(                     EXCBUS|       EXCROM|2)	// PC-6005		ROMカートリッジ
#define	EXC6006					(                     EXCBUS|EXCRAM|EXCROM|3)	// PC-6006		拡張ROM/RAMカートリッジ
#define	EXC660101				(              EXCFIX|              EXCROM|4)	// PC-6601-01	拡張漢字ROMカートリッジ
#define	EXC6006SR				(                            EXCRAM|       5)	// PC-6006SR	拡張64KRAMカートリッジ
#define	EXC6007SR				(              EXCFIX|       EXCRAM|EXCROM|6)	// PC-6007SR	拡張漢字ROM&RAMカートリッジ
#define	EXC6053					(       EXCSND|EXCFIX|EXCBUS|       EXCROM|7)	// PC-6053		ボイスシンセサイザー
#define	EXC60M55				(       EXCSND|                            8)	// PC-60m55		FM音源カートリッジ

#define	EXCSOL1					(EXCSOL|              EXCBUS|EXCRAM|EXCROM|1)	// 戦士のカートリッジ
#define	EXCSOL2					(EXCSOL|              EXCBUS|EXCRAM|EXCROM|2)	// 戦士のカートリッジmkⅡ
#define	EXCSOL3					(EXCSOL|              EXCBUS|EXCRAM|EXCROM|3)	// 戦士のカートリッジmkⅢ


#endif	// PC6001V_H_INCLUDED
