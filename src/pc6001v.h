#ifndef PC6001V_H_INCLUDED
#define PC6001V_H_INCLUDED


/////////////////////////////////////////////////////////////////////////////
// ビルドオプション (不要な項目はコメントアウトする)
/////////////////////////////////////////////////////////////////////////////
// モニタモードなし
//#define	NOMONITOR


/////////////////////////////////////////////////////////////////////////////
// バージョン情報,機種名など
/////////////////////////////////////////////////////////////////////////////
#define	VERSION		"2.0Alpha3"
#define	APPNAME		"PC6001VX"
#define	P60NAME		"PC-6001"
#define	P62NAME		"PC-6001mk2"
#define	P66NAME		"PC-6601"
#define	P64NAME		"PC-6001mk2SR"
#define	P68NAME		"PC-6601SR"


/////////////////////////////////////////////////////////////////////////////
// オプション 初期値
/////////////////////////////////////////////////////////////////////////////
#define	DEFAULT_MODEL			(60)				/* 機種 60:PC-6001 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR */
#define	DEFAULT_EXTRAM			(1)					/* 拡張RAM 0:なし 1:あり */
#define	DEFAULT_REPEAT			(70)				/* キーリピートの間隔(単位:ms 0で無効) */
#define	DEFAULT_SAMPLE_RATE		(44100)				/* サンプリングレート */
#define	SOUND_BUFFER_SIZE		(1)					/* サウンドバッファ長倍率(基本長はVSYNC) */
#define	DEFAULT_MASTERVOL		(70)				/* マスター音量 */
#define	DEFAULT_PSGVOL			(60)				/* PSG音量 */
#define	DEFAULT_PSGLPF			(0)					/* PSG LPFカットオフ周波数(0で無効) */
#define	DEFAULT_VOICEVOL		(100)				/* 音声合成音量 */
#define	DEFAULT_TAPEVOL			(10)				/* TAPEモニタ音量 */
#define	DEFAULT_TAPELPF			(1540)				/* TAPE LPFカットオフ周波数(0で無効) */
#define	DEFAULT_TURBO			(true)				/* Turbo TAPE Yes:有効 No:無効 */
#define	DEFAULT_BOOST			(false)				/* BoostUp Yes:有効 No:無効 */
#define DEFAULT_MAXBOOST60		(8)					/* BoostUp最大倍率(N60モード) */
#define DEFAULT_MAXBOOST62		(5)					/* BoostUp最大倍率(N60m/N66モード) */
#define	DEFAULT_FDD				(0)					/* FDD接続台数 */
#define	DEFAULT_COLOR_MODE		(8) 				/* カラーモード 8:8bit 16:bit 24:24bit */
#define	DEFAULT_MODE4_COLOR		(1)					/* モード４カラーモード 0:モノ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク */
#define	DEFAULT_SCANLINE		(false) 				/* スキャンライン true:あり false:なし */
#define	DEFAULT_SCANLINEBR		(75)				/* スキャンライン輝度 (0-100)% */
#define	DEFAULT_DISPNTSC		(true) 				/* 4:3表示 true:有効 false:無効 */
#define	DEFAULT_FRAMESKIP		(0)					/* フレームスキップ */
#define	DEFAULT_OVERCLOCK		(100)				/* オーバークロック率 */
#define	DEFAULT_CHECKCRC		(true) 				/* CRCチェック */
#define	DEFAULT_ROMPATCH		(true) 				/* ROMパッチ */
#define	DEFAULT_FULLSCREEN		(false) 			/* フルスクリーン */
#define	DEFAULT_DISPSTATUS		(true) 				/* ステータスバー表示状態 */
#define	DEFAULT_AVIRLE			(true) 				/* RLEフラグ */
#define	DEFAULT_CKQUIT			(false) 			/* 終了時確認 */
#define	DEFAULT_SAVEQUIT		(true) 				/* 終了時INI保存 */
#define	DEFAULT_SOLDIER			(false) 			/* 戦士のカートリッジ使うフラグ */


#define	CPUM_CLOCK60			(3993600)			/* メインCPUクロック(Hz) 60 */
#define	CPUM_CLOCK62			(4000000)			/* メインCPUクロック(Hz) 62,66 */
#define	CPUS_CLOCK60			(CPUM_CLOCK60*2)	/* サブCPUクロック(Hz) 60 */
#define	CPUS_CLOCK62			(CPUM_CLOCK62*2)	/* サブCPUクロック(Hz) 62,66 */
#define	PSG_CLOCK60				(CPUM_CLOCK60/2)	/* PSGクロック(Hz) 60 */
#define	PSG_CLOCK62				(CPUM_CLOCK62/2)	/* PSGクロック(Hz) 62,66 */

#define	VSYNC_HZ				(60)				/* VSYNC周波数 */
#define	DEFAULT_BAUD			(1200)				/* CMTボーレート 初期値 */
#define	DEFAULT_CMT_HZ			(DEFAULT_BAUD/12)	/* CMT割込み周波数 初期値 (1byte -> StartBit:1 + DataBit:8 + StopBit:3 = 12bits とする) */

//#define	DEFAULT_FPS_CNT			(true)			/* FPSカウンタ true:表示 false:非表示 */

/////////////////////////////////////////////////////////////////////////////
// 各種ディレクトリ名 初期値
/////////////////////////////////////////////////////////////////////////////
#define	ROM_DIR			"rom"				/* ROMイメージ */
#define	TAPE_DIR		"tape"				/* TAPEイメージ */
#define	DISK_DIR		"disk"				/* DISKイメージ */
#define	EXTROM_DIR		"extrom"			/* 拡張ROMイメージ */
#define	IMAGE_DIR		"img"				/* スナップショット */
#define	FONT_DIR		"font"				/* フォント */
#define	WAVE_DIR		"wave"				/* WAVEファイル */


/////////////////////////////////////////////////////////////////////////////
// 各種ファイル名 初期値
/////////////////////////////////////////////////////////////////////////////
#define	IMG_EXT			"png"				/* 画像ファイル拡張子 */
#define	CONF_FILE		"pc6001vx.ini"		/* 設定ファイル */
#define	PRINTER_FILE	"printer.txt"		/* プリンタ出力ファイル */
#define	DOKODEMO_FILE	"dokodemo.dds"		/* どこでもSAVEファイル */
#define	SERIAL_FILE		"serial.txt"		/* シリアル出力ファイル */
#define	SAVE_FILE		"_csave.p6t"		/* TAPE(CSAVE)ファイル */
#define	FONTZ_FILE		"fontz12."IMG_EXT	/* 半角フォントファイル */
#define	FONTH_FILE		"fonth12."IMG_EXT	/* 全角フォントファイル */
#define	FONTV_FILE		"cgrom60s.60"		/* VDGフォントファイル */

#define	SUBCPU60		"subcpu.60"			/* サブCPU ROM(PC-6001) */
#define	SUBCPU62		"subcpu.62"			/* サブCPU ROM(PC-6001mk2) */
#define	SUBCPU66		"subcpu.66"			/* サブCPU ROM(PC-6601) */
#define	SUBCPU64		"subcpu.64"			/* サブCPU ROM(PC-6001mk2SR) */
#define	SUBCPU68		"subcpu.68"			/* サブCPU ROM(PC-6601SR) */


#endif	// PC6001V_H_INCLUDED
