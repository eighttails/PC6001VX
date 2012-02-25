// OS依存の汎用ルーチン(主にUI用)

#include <QtGui>
#include <QSharedMemory>
#include <string.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_endian.h>

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../pc6001v.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define	AUDIOFORMAT	AUDIO_S16MSB	// 16ビット符号あり
#else
#define	AUDIOFORMAT	AUDIO_S16		// 16ビット符号あり
#endif

//#define USESDLTIMER					// SDLのタイマ使用

///////////////////////////////////////////////////////////
// スタティック変数
///////////////////////////////////////////////////////////
std::map<int, PCKEYsym> VKTable;	// SDLキーコード  -> 仮想キーコード 変換テーブル

static SDL_Joystick *Joyinfo[MAX_JOY];		// ジョイスティック情報
///////////////////////////////////////////////////////////
// 定数
///////////////////////////////////////////////////////////
static const struct {	// SDLキーコード -> 仮想キーコード定義
    int InKey;			// SDLのキーコード
    PCKEYsym VKey;		// 仮想キーコード
} VKeyDef[] = {
    { SDLK_UNKNOWN,			KVC_UNKNOWN },

    { SDLK_1,				KVC_1 },			// 1	!
    { SDLK_2,				KVC_2 },			// 2	"
    { SDLK_3,				KVC_3 },			// 3	#
    { SDLK_4,				KVC_4 },			// 4	$
    { SDLK_5,				KVC_5 },			// 5	%
    { SDLK_6,				KVC_6 },			// 6	&
    { SDLK_7,				KVC_7 },			// 7	'
    { SDLK_8,				KVC_8 },			// 8	(
    { SDLK_9,				KVC_9 },			// 9	)
    { SDLK_0,				KVC_0 },			// 0

    { SDLK_a,				KVC_A },			// a	A
    { SDLK_b,				KVC_B },			// b	B
    { SDLK_c,				KVC_C },			// c	C
    { SDLK_d,				KVC_D },			// d	D
    { SDLK_e,				KVC_E },			// e	E
    { SDLK_f,				KVC_F },			// f	F
    { SDLK_g,				KVC_G },			// g	G
    { SDLK_h,				KVC_H },			// h	H
    { SDLK_i,				KVC_I },			// i	I
    { SDLK_j,				KVC_J },			// j	J
    { SDLK_k,				KVC_K },			// k	K
    { SDLK_l,				KVC_L },			// l	L
    { SDLK_m,				KVC_M },			// m	M
    { SDLK_n,				KVC_N },			// n	N
    { SDLK_o,				KVC_O },			// o	O
    { SDLK_p,				KVC_P },			// p	P
    { SDLK_q,				KVC_Q },			// q	Q
    { SDLK_r,				KVC_R },			// r	R
    { SDLK_s,				KVC_S },			// s	S
    { SDLK_t,				KVC_T },			// t	T
    { SDLK_u,				KVC_U },			// u	U
    { SDLK_v,				KVC_V },			// v	V
    { SDLK_w,				KVC_W },			// w	W
    { SDLK_x,				KVC_X },			// x	X
    { SDLK_y,				KVC_Y },			// y	Y
    { SDLK_z,				KVC_Z },			// z	Z

    { SDLK_F1,				KVC_F1 },			// F1
    { SDLK_F2,				KVC_F2 },			// F2
    { SDLK_F3,				KVC_F3 },			// F3
    { SDLK_F4,				KVC_F4 },			// F4
    { SDLK_F5,				KVC_F5 },			// F5
    { SDLK_F6,				KVC_F6 },			// F6
    { SDLK_F7,				KVC_F7 },			// F7
    { SDLK_F8,				KVC_F8 },			// F8
    { SDLK_F9,				KVC_F9 },			// F9
    { SDLK_F10,				KVC_F10 },			// F10
    { SDLK_F11,				KVC_F11 },			// F11
    { SDLK_F12,				KVC_F12 },			// F12

    { SDLK_MINUS,			KVC_MINUS },		// -	=
    { SDLK_CARET,			KVC_CARET },		// ^	~
    { SDLK_BACKSPACE,		KVC_BACKSPACE },	// BackSpace
    { SDLK_AT,				KVC_AT },			// @	`
    { SDLK_LEFTBRACKET,		KVC_LBRACKET },		// [	{
    { SDLK_SEMICOLON,		KVC_SEMICOLON },	// ;	+
    { SDLK_COLON,			KVC_COLON },		// :	*
    { SDLK_COMMA,			KVC_COMMA },		// ,	<
    { SDLK_PERIOD,			KVC_PERIOD },		// .	>
    { SDLK_SLASH,			KVC_SLASH },		// /	?
    { SDLK_SPACE,			KVC_SPACE },		// Space

    { SDLK_ESCAPE,			KVC_ESC },			// ESC
    { SDLK_BACKQUOTE,		KVC_HANZEN },		// 半角/全角
    { SDLK_TAB,				KVC_TAB },			// Tab
    { SDLK_CAPSLOCK,		KVC_CAPSLOCK },		// CapsLock
    { SDLK_RETURN,			KVC_ENTER },		// Enter
    { SDLK_LCTRL,			KVC_LCTRL },		// L-Ctrl
    { SDLK_RCTRL,			KVC_RCTRL },		// R-Ctrl
    { SDLK_LSHIFT,			KVC_LSHIFT },		// L-Shift
    { SDLK_RSHIFT,			KVC_RSHIFT },		// R-Shift
    { SDLK_LALT,			KVC_LALT },			// L-Alt
    { SDLK_RALT,			KVC_RALT },			// R-Alt
    { SDLK_PRINT,			KVC_PRINT },		// PrintScreen
    { SDLK_SCROLLOCK,		KVC_SCROLLLOCK },	// ScrollLock
    { SDLK_PAUSE,			KVC_PAUSE },		// Pause
    { SDLK_INSERT,			KVC_INSERT },		// Insert
    { SDLK_DELETE,			KVC_DELETE },		// Delete
    { SDLK_END,				KVC_END },			// End
    { SDLK_HOME,			KVC_HOME },			// Home
    { SDLK_PAGEUP,			KVC_PAGEUP },		// PageUp
    { SDLK_PAGEDOWN,		KVC_PAGEDOWN },		// PageDown

    { SDLK_UP,				KVC_UP },			// ↑
    { SDLK_DOWN,			KVC_DOWN },			// ↓
    { SDLK_LEFT,			KVC_LEFT },			// ←
    { SDLK_RIGHT,			KVC_RIGHT },		// →

    { SDLK_KP0,				KVC_P0 },			// [0]
    { SDLK_KP1,				KVC_P1 },			// [1]
    { SDLK_KP2,				KVC_P2 },			// [2]
    { SDLK_KP3,				KVC_P3 },			// [3]
    { SDLK_KP4,				KVC_P4 },			// [4]
    { SDLK_KP5,				KVC_P5 },			// [5]
    { SDLK_KP6,				KVC_P6 },			// [6]
    { SDLK_KP7,				KVC_P7 },			// [7]
    { SDLK_KP8,				KVC_P8 },			// [8]
    { SDLK_KP9,				KVC_P9 },			// [9]
    { SDLK_NUMLOCK,			KVC_NUMLOCK },		// NumLock
    { SDLK_KP_PLUS,			KVC_P_PLUS },		// [+]
    { SDLK_KP_MINUS,		KVC_P_MINUS },		// [-]
    { SDLK_KP_MULTIPLY,		KVC_P_MULTIPLY },	// [*]
    { SDLK_KP_DIVIDE,		KVC_P_DIVIDE },		// [/]
    { SDLK_KP_PERIOD,		KVC_P_PERIOD },		// [.]
    { SDLK_KP_ENTER,		KVC_P_ENTER },		// [Enter]

    // 日本語キーボードのみ
    { SDLK_BACKSLASH,		KVC_YEN },			// ¥	|
    { SDLK_RIGHTBRACKET,	KVC_RBRACKET },		// ]	}
    { SDLK_LESS,			KVC_UNDERSCORE },	// ¥	_
    //	{ 				,		KVC_MUHENKAN },		// 無変換
    //	{ 				,		KVC_HENKAN },		// 変換
    //	{ 				,		KVC_HIRAGANA },		// ひらがな

    // 英語キーボードのみ
    { SDLK_BACKSLASH,		KVE_BACKSLASH },	// ¥	|

    // 追加キー
    { SDLK_LMETA,			KVX_LMETA },		// L-Meta
    { SDLK_RMETA,			KVX_RMETA },		// R-Meta
    { SDLK_MENU,			KVX_MENU }			// Menu
};


///////////////////////////////////////////////////////////
// メッセージ配列
///////////////////////////////////////////////////////////
// 一般メッセージ
const char *MsgStr[] = {
    "終了してよろしいですか?",
    "終了確認",
    "再起動してよろしいですか?",
    "変更は再起動後に有効となります。\n今すぐ再起動しますか?",
    "再起動確認",
    "拡張ROMを挿入してリセットします。",
    "拡張ROMを排出してリセットします。",
    "リセット確認"
};

// メニュー用メッセージ ------
const char *MsgMen[] = {
    "ビデオキャプチャ...",
    "ビデオキャプチャ停止",
    "記録...",	// (リプレイ)
    "記録停止",	// (リプレイ)
    "再生...",	// (リプレイ)
    "再生停止"	// (リプレイ)
};

// INIファイル用メッセージ ------
const char *MsgIni[] = {
    // [CONFIG]
    "; === PC6001V 初期設定ファイル ===\n\n",
    //	"機種 60:PC-6001 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR",
    "機種 60:PC-6001 62:PC-6001mk2 66:PC-6601",
    "FDD接続台数 (0-2)",
    "拡張RAM使用",
    "Turbo TAPE Yes:有効 No:無効",
    "BoostUp Yes:有効 No:無効",
    "BoostUp 最大倍率(N60モード)",
    "BoostUp 最大倍率(N60m/N66モード)",
    "オーバークロック率 (1-1000)%",
    "CRCチェック Yes:有効 No:無効",
    "ROMパッチ Yes:あてる No:あてない",
    // [DISPLAY]
    "カラーモード 8:8bit 16:16bit 24:24bit",
    "MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク",
    "スキャンライン Yes:あり No:なし",
    "スキャンライン輝度 (0-100)%",
    "4:3表示 Yes:有効 No:無効",
    "フルスクリーンモード Yes:有効 No:無効",
    "ステータスバー Yes:表示 No:非表示",
    "フレームスキップ",
    // [SOUND]
    "サンプリングレート (44100/22050/11025)Hz",
    "サウンドバッファサイズ",
    "マスター音量 (0-100)",
    "PSG音量 (0-100)",
    "音声合成音量 (0-100)",
    "TAPEモニタ音量 (0-100)",
    "PSG LPFカットオフ周波数(0で無効)",
    // [MOVIE]
    "RLEフラグ Yes:RLE有効 No:ベタ",
    // [FILES]
    "拡張ROMファイル名",
    "TAPE(LODE)ファイル名(起動時に自動マウント)",
    "TAPE(SAVE)ファイル名(SAVE時に自動マウント)",
    "DISKファイル名(起動時に自動マウント)",
    "プリンタ出力ファイル名",
    "全角フォントファイル名",
    "半角フォントファイル名",
    // [PATH]
    "ROMイメージ格納パス",
    "TAPEイメージ格納パス",
    "DISKイメージ格納パス",
    "拡張ROMイメージ格納パス",
    "スナップショット格納パス",
    "WAVEファイル格納パス",
    "FONT格納パス",
    // [CHECK]
    "終了時確認 Yes:する No:しない",
    "終了時INIファイルを保存 Yes:する No:しない",
    // [KEY]
    "キーリピートの間隔(単位:ms 0で無効)",
    "PS/2キーボード使う? Yes:PS/2 No:USB",
    // [OPTION]
    "戦士のカートリッジ Yes:有効 No:無効"

};

// どこでもSAVE用メッセージ ------
const char *MsgDok[] = {
    "; === PC6001V どこでもSAVEファイル ===\n\n"
};

// Error用メッセージ ------
const char *MsgErr[] = {
    "Error",
    "エラーはありません",
    "原因不明のエラーが発生しました",
    "メモリの確保に失敗しました",
    "指定された機種のROMイメージが見つからないため機種を変更しました\n設定を確認してください",
    "ROMイメージが見つかりません\n設定とファイル名を確認してください",
    "ROMイメージのサイズが不正です",
    "ROMイメージのCRCが不正です",
    "ライブラリ(SDL)の初期化に失敗しました",
    "初期化に失敗しました\n設定を確認してください",
    "フォントの読込みに失敗しました",
    "INIファイルの読込みに失敗しました\nデフォルト設定で起動します",
    "INIファイルの読込みに失敗しました",
    "INIファイルの保存に失敗しました",
    "TAPEイメージのマウントに失敗しました",
    "DISKイメージのマウントに失敗しました",
    "拡張ROMイメージのマウントに失敗しました",
    "どこでもLOADに失敗しました",
    "どこでもSAVEに失敗しました",
    "どこでもLOADに失敗しました\n保存時と機種が異なります",
    "どこでもLOADに失敗しました\n保存時とPC6001Vのバージョンが異なります",
    "リプレイ再生に失敗しました",
    "リプレイ再生に失敗しました\n保存時と機種が異なります",
    "リプレイ記録に失敗しました",
    "リプレイデータがありません"
};


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL OSD_Init( void )
{
    // SDLキーコード  -> 仮想キーコード 変換テーブル初期化
    VKTable.clear();
    for( int i=0; i < COUNTOF(VKeyDef); i++ ) VKTable[VKeyDef[i].InKey] = VKeyDef[i].VKey;

    // ジョイスティック情報クリア
    for( int i=0; i < MAX_JOY; i++ ) Joyinfo[i] = NULL;

    return TRUE;
}


////////////////////////////////////////////////////////////////
// 小文字による文字列比較
//
// 引数:	s1			比較する文字列へのポインタ1
//			s2			比較する文字列へのポインタ2
// 返値:	int			0:等しい 正数:s1>s2 負数:s1<s2
////////////////////////////////////////////////////////////////
int stricmp ( const char *s1, const char *s2 )
{
    const char *ps1 = s1, *ps2 = s2;

    if( !s1 ) return -1;
    if( !s2 ) return  1;

    // 先頭の文字
    char c1 = *ps1++;
    char c2 = *ps2++;

    // どちらかの文字列終端まで繰り返し
    while( c1 && c2 ){
        // 小文字に変換
        if( c1 >= 'A' && c1 <= 'Z' ) c1 += 'a' - 'A';
        if( c2 >= 'A' && c2 <= 'Z' ) c2 += 'a' - 'A';

        // 等しくなかったら比較終了
        if( c1 != c2 ) break;

        // 次の文字取得
        c1 = *ps1++;
        c2 = *ps2++;
    }

    return (int)c1 - (int)c2;
}


////////////////////////////////////////////////////////////////
// パスのデリミタを'/'に変換
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void Delimiter( char *path )
{
    QString strPath = QDir::fromNativeSeparators(QString::fromUtf8(path));
    strcpy(path, strPath.toUtf8().data());
}


////////////////////////////////////////////////////////////////
// パスのデリミタを'\'に変換
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void UnDelimiter( char *path )
{
    QString strPath = QDir::toNativeSeparators(QString::fromUtf8(path));
    strcpy(path, strPath.toUtf8().data());
}


////////////////////////////////////////////////////////////////
// パスの末尾にデリミタを追加
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void AddDelimiter( char *path )
{
    if( !strlen( path ) )
        strncpy( path, "/", PATH_MAX );
    else if( (path[strlen(path)-1] != '/' ) && ( path[strlen(path)-1] != '\\') )
        strncat( path, "/", PATH_MAX );
}


////////////////////////////////////////////////////////////////
// パスの末尾のデリミタを削除
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void DelDelimiter( char *path )
{
    if( strlen( path ) > 1 )
        while( ( path[strlen(path)-1] == '/' ) || ( path[strlen(path)-1] == '\\' ) )
            path[strlen(path)-1] = '\0';
}


////////////////////////////////////////////////////////////////
// 多重起動チェック
//
// 引数:	なし
// 返値:	BOOL		TRUE:起動済み FALSE:未起動
////////////////////////////////////////////////////////////////
BOOL OSD_IsWorking( void )
{
    PRINTD( OSD_LOG, "[OSD][OSD_IsWorking]\n" );

    QString filePath = QString::fromUtf8(OSD_GetConfigPath());
    filePath += QDir::separator() + QString("pc6001vx.lock");

    QFile lockFile(filePath);
    if (lockFile.exists()) return TRUE;

    lockFile.open(QIODevice::WriteOnly);
    lockFile.close();

    return FALSE;
}


////////////////////////////////////////////////////////////////
// 終了処理
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Finish( void )
{
    PRINTD( OSD_LOG, "[OSD][OSD_Finish]\n" );

    QDir dir(QString::fromUtf8(OSD_GetConfigPath()));
    QString lockFile("pc6001vx.lock");

    if (dir.exists(lockFile))
        dir.remove(lockFile);
}

////////////////////////////////////////////////////////////////
// 設定及び諸ファイル(ROMなどの初期位置)フォルダ作成
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
BOOL OSD_CreateConfigPath()
{
    QString basePath = QString::fromUtf8(OSD_GetConfigPath());

    // ROMイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(ROM_DIR)))
        return FALSE;

    // TAPEイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(TAPE_DIR)))
        return FALSE;

    // DISKイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(DISK_DIR)))
        return FALSE;

    // 拡張ROMイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(EXTROM_DIR)))
        return FALSE;

    // スナップショット
    if(!QDir(basePath).mkpath(QString::fromUtf8(IMAGE_DIR)))
        return FALSE;

    // フォント
    if(!QDir(basePath).mkpath(QString::fromUtf8(FONT_DIR)))
        return FALSE;

    // WAVEファイル
    if(!QDir(basePath).mkdir(QString::fromUtf8(WAVE_DIR)))
        return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////
// 設定及び諸ファイル(ROMなどの初期位置)パス取得
//
// 引数:	なし
// 返値:	char *		取得した文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetConfigPath( void )
{
    PRINTD( OSD_LOG, "[OSD][OSD_GetModulePath]\n" );
    static char mpath[PATH_MAX] = "";	// モジュールパス取得用

    //Windowsの場合はexe本体と同じ場所。
    //それ以外(UNIX系を想定)は ~/.pc6001vx を返す
#ifdef WIN32
    QString confPath = QApplication::applicationDirPath() + QDir::separator();
#else
    QString confPath = QDir::homePath() + QDir::separator() + QString(".pc6001vx");
#endif
    sprintf( mpath, "%s", confPath.toUtf8().data() );
    AddDelimiter( mpath );	// 念のため
    return mpath;
}


////////////////////////////////////////////////////////////////
// フルパスからファイル名を取得
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	char *		ファイル名の開始ポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetFileNamePart( const char *path )
{
    PRINTD( OSD_LOG, "[OSD][OSD_GetFileNamePart]\n" );

    QFile file(path);
    return file.fileName().toUtf8().data();
}


////////////////////////////////////////////////////////////////
// ファイルの存在チェック
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	BOOL		TRUE:存在する FALSE:存在しない
////////////////////////////////////////////////////////////////
BOOL OSD_FileExist( const char *fullpath )
{
    PRINTD( OSD_LOG, "[OSD][OSD_FileExist]\n" );

    QString pathString = QString::fromUtf8(fullpath);

    //ワイルドカードを含む場合
    if (pathString.contains("*")){
        QFileInfo info(pathString);

        QDir dir = info.absoluteDir();
        QFile file(pathString);
        QString wildcard = info.fileName();

        QFileInfoList list = dir.entryInfoList(QStringList(wildcard), QDir::Files);
        return !list.empty();
    } else {
        QFile file(pathString);
        return file.exists();
    }
}


////////////////////////////////////////////////////////////////
// ファイルの読取り専用チェック
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	BOOL		TRUE:読取り専用 FALSE:読み書き
///////////////////////////////////////////////////////////////
BOOL OSD_FileReadOnly( const char *fullpath )
{
    PRINTD( OSD_LOG, "[OSD][OSD_FileReadOnly]\n" );

    QFileInfo info(QString::fromUtf8(fullpath));
    return !info.isWritable();
}


///////////////////////////////////////////////////////////
// フォルダの参照
//
// 引数:	hwnd		親のウィンドウハンドル
//			Result		フォルダへのパス格納バッファポインタ
// 返値:	char *		取得した文字列へのポインタ
///////////////////////////////////////////////////////////
const char *OSD_FolderDiaog( void *hwnd, char *Result )
{
    QByteArray result = QFileDialog::getExistingDirectory(NULL, "フォルダを選択してください。", OSD_GetConfigPath()).toUtf8();
    strcpy(Result, result);
    return result.data();
}


///////////////////////////////////////////////////////////
// ファイルの参照
//
// 引数:	hwnd		親のウィンドウハンドル
//			mode		モード FM_Load:ファイルを開く FM_Save:名前を付けて保存
//			title		ウィンドウキャプション文字列へのポインタ
//			filter		ファイルフィルタ文字列へのポインタ
//			fullpath	フルパス格納バッファポインタ
//			path		ファイル検索パス格納バッファポインタ
//			ext			拡張子文字列へのポインタ
// 返値:	char *		取得したファイル名へのポインタ(フルパスではない)
///////////////////////////////////////////////////////////
const char *OSD_FileDiaog( void *hwnd, FileMode mode, const char *title, const char *filter, char *fullpath, char *path, const char *ext )
{
    QString result;
    if(mode == FM_Save){
        result = QFileDialog::getSaveFileName(NULL, title, path, filter);
    } else {
        result = QFileDialog::getOpenFileName(NULL, title, path, filter);
    }
    if(result.isEmpty())    return NULL;

    QDir dir(result);
    if( path ) strcpy( fullpath, dir.path().toUtf8().data() );
    if( fullpath ) strcpy( fullpath, result.toUtf8().data() );

    QFile file(result);
    return file.fileName().toUtf8().data();
}


////////////////////////////////////////////////////////////////
// 各種ファイル選択
//
// 引数:	hwnd		親のウィンドウハンドル(NULLの場合はスクリーンサーフェスのハンドル取得を試みる)
//			type		ダイアログの種類(FileDlg参照)
//			fullpath	フルパス格納バッファポインタ
//			path		ファイル検索パス格納バッファポインタ
// 返値:	char *		取得したファイル名へのポインタ(フルパスではない)
////////////////////////////////////////////////////////////////
const char *OSD_FileSelect( void *hwnd, FileDlg type, char *fullpath, char *path )
{
    FileMode mode = FM_Load;
    const char *title   = NULL;
    const char *filter  = NULL;
    const char *ext     = NULL;

    switch( type ){
    case FD_TapeLoad:	// TAPE(LOAD)選択
        mode   = FM_Load;
        title  = "TAPEイメージ選択(LOAD)";
        filter = "TAPEイメージ (*.p6t *.cas *.p6);;"
                "P6T形式 (*.p6t);;"
                "ベタ形式 (*.cas *.p6);;"
                "全てのファイル (*.*)";
        ext    = "p6t";
        break;

    case FD_TapeSave:	// TAPE(SAVE)選択
        mode   = FM_Save;
        title  = "TAPEイメージ選択(SAVE)";
        filter = "TAPEイメージ (*.p6t *.cas *.p6);;"
                "P6T形式 (*.p6t);;"
                "ベタ形式 (*.cas *.p6);;"
                "全てのファイル (*.*)";
        ext    = "p6t";
        break;

    case FD_Disk:		// DISK選択
        mode   = FM_Load;
        title  = "DISKイメージ選択";
        filter = "DISKイメージ(*.d88);;"
                "D88形式 (*.d88);;"
                "全てのファイル (*.*)";
        ext    = "d88";
        break;

    case FD_ExtRom:		// 拡張ROM選択
        mode   = FM_Load;
        title  = "拡張ROMイメージ選択";
        filter = "全てのファイル (*.*)";
        break;

    case FD_Printer:	// プリンター出力ファイル選択
        mode   = FM_Save;
        title  = "プリンター出力ファイル選択";
        filter = "全てのファイル (*.*)";
        ext    = "txt";
        break;

    case FD_FontZ:		// 全角フォントファイル選択
        mode   = FM_Load;
        title  = "全角フォントファイル選択";
        filter = "PNGファイル (*.png);;"
                "全てのファイル (*.*)";
        ext    = "png";
        break;

    case FD_FontH:		// 半角フォントファイル選択
        mode   = FM_Load;
        title  = "半角フォントファイル選択";
        filter = "PNGファイル (*.png);;"
                "全てのファイル (*.*)";
        ext    = "png";
        break;

    case FD_DokoLoad:	// どこでもLOADファイル選択
        mode   = FM_Load;
        title  = "どこでもLOADファイル選択";
        filter = "どこでもSAVEファイル(*.dds *.ddr);;"
                "どこでもSAVE形式 (*.dds);;"
                "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)";
        ext    = "dds";
        break;

    case FD_DokoSave:	// どこでもSAVEファイル選択
        mode   = FM_Save;
        title  = "どこでもSAVEファイル選択";
        filter = "どこでもSAVE形式 (*.dds);;"
                "全てのファイル (*.*)";
        ext    = "dds";
        break;

    case FD_RepLoad:	// リプレイ再生ファイル選択
        mode   = FM_Load;
        title  = "リプレイ再生ファイル選択";
        filter = "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)";
        ext    = "ddr";
        break;

    case FD_RepSave:	// リプレイ保存ファイル選択
        mode   = FM_Save;
        title  = "リプレイ保存ファイル選択";
        filter = "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)";
        ext    = "ddr";
        break;

    case FD_AVISave:	// ビデオキャプチャ出力ファイル選択
        mode   = FM_Save;
        title  = "ビデオキャプチャ出力ファイル選択";
        filter = "AVIファイル (*.avi);;"
                "全てのファイル (*.*)";
        ext    = "avi";
        break;

    case FD_LoadAll:	// 汎用LOAD
    default:
        mode   = FM_Load;
        title  = "ファイル選択";
        filter = "全てのファイル (*.*)";
        break;
    }

    return OSD_FileDiaog( NULL, mode, title, filter, fullpath, path, ext );
}


////////////////////////////////////////////////////////////////
// メッセージ表示
//
// 引数:	mes			メッセージ文字列へのポインタ
//			cap			ウィンドウキャプション文字列へのポインタ
//			type		表示形式指示のフラグ
// 返値:	int			押されたボタンの種類
//							OSDR_OK:     OKボタン
//							OSDR_CANCEL: CANCELボタン
//							OSDR_YES:    YESボタン
//							OSDR_NO:     NOボタン
////////////////////////////////////////////////////////////////
int OSD_Message( const char *mes, const char *cap, int type )
{
    QMessageBox::StandardButtons Type = QMessageBox::Ok;
    QMessageBox::Icon IconType = QMessageBox::Information;

    // メッセージボックスのタイプ
    switch( type&0x000f ){
    case OSDM_OK:		Type = QMessageBox::Ok;                         break;
    case OSDM_OKCANCEL:		Type = QMessageBox::Ok | QMessageBox::Cancel;	break;
    case OSDM_YESNO:		Type = QMessageBox::Yes | QMessageBox::No;	break;
    case OSDM_YESNOCANCEL:	Type = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;    break;
    }

    // メッセージボックスのアイコンタイプ
    switch( type&0x00f0 ){
    case OSDM_ICONERROR:	IconType = QMessageBox::Critical;	break;
    case OSDM_ICONQUESTION:	IconType = QMessageBox::Question;	break;
    case OSDM_ICONWARNING:	IconType = QMessageBox::Warning;	break;
    case OSDM_ICONINFO:		IconType = QMessageBox::Information;	break;
    }

    QMessageBox mb(IconType, cap, mes, Type);
    int res = mb.exec();

    switch( res ){
    case QMessageBox::Ok:	return OSDR_OK;
    case QMessageBox::Yes:	return OSDR_YES;
    case QMessageBox::No:	return OSDR_NO;
    default:	return OSDR_CANCEL;
    }
}




////////////////////////////////////////////////////////////////
// キーリピート設定
//
// 引数:	repeat	キーリピートの間隔(ms) 0で無効
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetKeyRepeat( int repeat )
{
    SDL_EnableKeyRepeat( 500, repeat );
}


////////////////////////////////////////////////////////////////
// OSDキーコード -> 仮想キーコード変換
//
// 引数:	scode		SDLのキーコード
// 返値:	int			仮想キーコード
////////////////////////////////////////////////////////////////
int OSD_ConvertKeyCode( int scode )
{
    return VKTable[scode];
}


////////////////////////////////////////////////////////////////
// 利用可能なジョイスティック数取得
//
// 引数:	なし
// 返値:	int			利用可能なジョイスティック数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNum( void )
{
    return SDL_NumJoysticks();
}


////////////////////////////////////////////////////////////////
// ジョイスティック名取得
//
// 引数:	int			インデックス
// 返値:	char *		ジョイスティック名文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetJoyName( int index )
{
    return SDL_JoystickName( index );
}


////////////////////////////////////////////////////////////////
// ジョイスティックオープンされてる？
//
// 引数:	int			インデックス
// 返値:	BOOL		TRUE:OPEN FALSE:CLOSE
////////////////////////////////////////////////////////////////
BOOL OSD_OpenedJoy( int index )
{
    return SDL_JoystickOpened( index ) ? TRUE : FALSE;
}


////////////////////////////////////////////////////////////////
// ジョイスティックオープン
//
// 引数:	int			インデックス
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL OSD_OpenJoy( int index )
{
    if( SDL_JoystickOpened( index ) || Joyinfo[index] != NULL ) return TRUE;

    Joyinfo[index] = SDL_JoystickOpen( index );

    return Joyinfo[index] ? TRUE : FALSE;
}


////////////////////////////////////////////////////////////////
// ジョイスティッククローズ
//
// 引数:	int			インデックス
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_CloseJoy( int index )
{
    if( Joyinfo[index] ){
        SDL_JoystickClose( Joyinfo[index] );
        Joyinfo[index] = NULL;
    }
}


////////////////////////////////////////////////////////////////
// ジョイスティックの軸の数取得
//
// 引数:	int			インデックス
// 返値:	int			軸の数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumAxes( int index )
{
    if( index < MAX_JOY && Joyinfo[index] )
        return SDL_JoystickNumAxes( Joyinfo[index] );
    return 0;
}


////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの数取得
//
// 引数:	int			インデックス
// 返値:	int			ボタンの数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumButtons( int index )
{
    if( index < MAX_JOY && Joyinfo[index] )
        return SDL_JoystickNumButtons( Joyinfo[index] );
    return 0;
}


////////////////////////////////////////////////////////////////
// ジョイスティックの軸の状態取得
//
// 引数:	int			インデックス
//			int			軸の番号
// 返値:	int			軸の状態(-32768〜32767)
////////////////////////////////////////////////////////////////
int OSD_GetJoyAxis( int index, int num )
{
    if( index < MAX_JOY && Joyinfo[index] )
        return SDL_JoystickGetAxis( Joyinfo[index], num );
    return 0;
}


////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの状態取得
//
// 引数:	int			インデックス
//			int			ボタンの番号
// 返値:	BOOL		ボタンの状態 TRUE:ON FALSE:OFF
////////////////////////////////////////////////////////////////
BOOL OSD_GetJoyButton( int index, int num )
{
    if( index < MAX_JOY && Joyinfo[index] )
        return (BOOL)SDL_JoystickGetButton( Joyinfo[index], num );
    return FALSE;
}


////////////////////////////////////////////////////////////////
// オーディオデバイスオープン
//
// 引数:	obj			自分自身へのオブジェクトポインタ
//			callback	コールバック関数へのポインタ
//			rate		サンプリングレート
//			sample		バッファサイズ(サンプル数)
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL OSD_OpenAudio( void *obj, void (*callback)(void *, BYTE *, int ), int rate, int samples )
{
    SDL_AudioSpec ASpec;				// オーディオスペック

    ASpec.freq     = rate;				// サンプリングレート
    ASpec.format   = AUDIOFORMAT;		// フォーマット
    ASpec.channels = 1;					// モノラル
    ASpec.samples  = samples;			// バッファサイズ(サンプル数)
    ASpec.callback = callback;			// コールバック関数の指定
    ASpec.userdata = obj;				// コールバック関数に自分自身のオブジェクトポインタを渡す

    // オーディオデバイスを開く
    if( SDL_OpenAudio( &ASpec, NULL ) < 0 ) return FALSE;

    return TRUE;
}


////////////////////////////////////////////////////////////////
// オーディオデバイスクローズ
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_CloseAudio( void )
{
    SDL_CloseAudio();
}


////////////////////////////////////////////////////////////////
// 再生開始
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StartAudio( void )
{
    SDL_PauseAudio( 0 );
}


////////////////////////////////////////////////////////////////
// 再生停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StopAudio( void )
{
    SDL_PauseAudio( 1 );
}


////////////////////////////////////////////////////////////////
// Waveファイル読込み
// 　対応形式は 22050Hz以上,符号付き16bit,1ch
//
// 引数:	filepath	ファイルパス
//			buf			バッファポインタ格納ポインタ
//			len			ファイル長さ格納ポインタ
//			freq		サンプリングレート格納ポインタ
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL OSD_LoadWAV( const char *filepath, BYTE **buf, DWORD *len, int *freq )
{
    SDL_AudioSpec ws;

    if( !SDL_LoadWAV( filepath, &ws, buf, (Uint32 *)len ) ) return FALSE;

    if( ws.freq < 22050 || ws.format != AUDIO_S16 || ws.channels != 1 ){
        SDL_FreeWAV( *buf );
        return FALSE;
    }

    *freq    = ws.freq;

    return TRUE;
}


////////////////////////////////////////////////////////////////
// Waveファイル開放
//
// 引数:	buf			バッファへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_FreeWAV( BYTE *buf )
{
    SDL_FreeWAV( buf );
}


////////////////////////////////////////////////////////////////
// 指定時間待機
//
// 引数:	tms			待機時間(ms)
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Delay( DWORD tms )
{
#ifdef USESDLTIMER
    SDL_Delay( tms );
#else
    Sleep( tms );
#endif
}


////////////////////////////////////////////////////////////////
// プロセス開始からの経過時間取得
//
// 引数:	なし
// 返値:	DWORD		経過時間(ms)
////////////////////////////////////////////////////////////////
DWORD OSD_GetTicks( void )
{
#ifdef USESDLTIMER
    return SDL_GetTicks();
#else
    return GetTickCount();
#endif
}


////////////////////////////////////////////////////////////////
// 色の名前取得
//
// 引数:	num			色コード
// 返値:	char *		色の名前文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_ColorName( int num )
{
    static const char *JColorName[] = {
        "MODE1,2 黒(ボーダー)",
        "MODE1 Set1 緑",		"MODE1 Set1 深緑",		"MODE1 Set2 橙",		"MODE1 Set2 深橙",
        "MODE2 緑",				"MODE2 黄",				"MODE2 青",				"MODE2 赤",
        "MODE2 白",				"MODE2 シアン",			"MODE2 マゼンタ",		"MODE2 橙",
        "MODE3 Set1 緑",		"MODE3 Set1 黄",		"MODE3 Set1 青",		"MODE3 Set1 赤",
        "MODE3 Set2 白",		"MODE3 Set2 シアン",	"MODE3 Set2 マゼンタ",	"MODE3 Set2 橙",
        "MODE4 Set1 深緑",		"MODE4 Set1 緑",		"MODE4 Set2 黒",		"MODE4 Set2 白",
        "MODE4 Set1 にじみ 赤",	"MODE4 Set1 にじみ 黄",	"MODE4 Set1 にじみ -",	"MODE4 Set1 にじみ -",
        "MODE4 Set2 にじみ 赤",	"MODE4 Set2 にじみ 青",	"MODE4 Set2 にじみ 桃",	"MODE4 Set2 にじみ 緑",
        "mk2〜 透明(黒)",		"mk2〜 橙",				"mk2〜 青緑",			"mk2〜 黄緑",
        "mk2〜 青紫",			"mk2〜 赤紫",			"mk2〜 空色",			"mk2〜 灰色",
        "mk2〜 黒",				"mk2〜 赤",				"mk2〜 緑",				"mk2〜 黄",
        "mk2〜 青",				"mk2〜 マゼンタ",		"mk2〜 シアン",			"mk2〜 白" };

    if( num < 0 || num >= COUNTOF( JColorName ) ) return NULL;
    else                                          return JColorName[num];

}


////////////////////////////////////////////////////////////////
// キーの名前取得
//
// 引数:	sym			仮想キーコード
// 返値:	char *		キー名前文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_KeyName( PCKEYsym sym )
{
    static const PCKeyName kname[] = {
        { KVC_1,			"1" },
        { KVC_2,			"2" },
        { KVC_3,			"3" },
        { KVC_4,			"4" },
        { KVC_5,			"5" },
        { KVC_6,			"6" },
        { KVC_7,			"7" },
        { KVC_8,			"8" },
        { KVC_9,			"9" },
        { KVC_0,			"0" },

        { KVC_A,			"A" },
        { KVC_B,			"B" },
        { KVC_C,			"C" },
        { KVC_D,			"D" },
        { KVC_E,			"E" },
        { KVC_F,			"F" },
        { KVC_G,			"G" },
        { KVC_H,			"H" },
        { KVC_I,			"I" },
        { KVC_J,			"J" },
        { KVC_K,			"K" },
        { KVC_L,			"L" },
        { KVC_M,			"M" },
        { KVC_N,			"N" },
        { KVC_O,			"O" },
        { KVC_P,			"P" },
        { KVC_Q,			"Q" },
        { KVC_R,			"R" },
        { KVC_S,			"S" },
        { KVC_T,			"T" },
        { KVC_U,			"U" },
        { KVC_V,			"V" },
        { KVC_W,			"W" },
        { KVC_X,			"X" },
        { KVC_Y,			"Y" },
        { KVC_Z,			"Z" },

        { KVC_F1,			"F1" },
        { KVC_F2,			"F2" },
        { KVC_F3,			"F3" },
        { KVC_F4,			"F4" },
        { KVC_F5,			"F5" },
        { KVC_F6,			"F6" },
        { KVC_F7,			"F7" },
        { KVC_F8,			"F8" },
        { KVC_F9,			"F9" },
        { KVC_F10,			"F10" },
        { KVC_F11,			"F11" },
        { KVC_F12,			"F12" },

        { KVC_MINUS,		"-" },
        { KVC_CARET,		"^" },
        { KVC_BACKSPACE,	"BackSpace" },
        { KVC_AT,			"@" },
        { KVC_LBRACKET,		"[" },
        { KVC_SEMICOLON,	";" },
        { KVC_COLON,		":" },
        { KVC_COMMA,		"," },
        { KVC_PERIOD,		"." },
        { KVC_SLASH,		"/" },
        { KVC_SPACE,		"Space" },

        { KVC_ESC,			"ESC" },
        { KVC_HANZEN,		"半角/全角" },
        { KVC_TAB,			"Tab" },
        { KVC_CAPSLOCK,		"CapsLock" },
        { KVC_ENTER,		"Enter" },
        { KVC_LCTRL,		"L-Ctrl" },
        { KVC_RCTRL,		"R-Ctrl" },
        { KVC_LSHIFT,		"L-Shift" },
        { KVC_RSHIFT,		"R-Shift" },
        { KVC_LALT,			"L-Alt" },
        { KVC_RALT,			"R-Alt" },
        { KVC_PRINT,		"PrintScreen" },
        { KVC_SCROLLLOCK,	"ScrollLock" },
        { KVC_PAUSE,		"Pause" },
        { KVC_INSERT,		"Insert" },
        { KVC_DELETE,		"Delete" },
        { KVC_END,			"End" },
        { KVC_HOME,			"Home" },
        { KVC_PAGEUP,		"PageUp" },
        { KVC_PAGEDOWN,		"PageDown" },

        { KVC_UP,			"↑" },
        { KVC_DOWN,			"↓" },
        { KVC_LEFT,			"←" },
        { KVC_RIGHT,		"→" },

        { KVC_P0,			"0(テンキー)" },
        { KVC_P1,			"1(テンキー)" },
        { KVC_P2,			"2(テンキー)" },
        { KVC_P3,			"3(テンキー)" },
        { KVC_P4,			"4(テンキー)" },
        { KVC_P5,			"5(テンキー)" },
        { KVC_P6,			"6(テンキー)" },
        { KVC_P7,			"7(テンキー)" },
        { KVC_P8,			"8(テンキー)" },
        { KVC_P9,			"9(テンキー)" },
        { KVC_NUMLOCK,		"NumLock" },
        { KVC_P_PLUS,		"+(テンキー)" },
        { KVC_P_MINUS,		"-(テンキー)" },
        { KVC_P_MULTIPLY,	"*(テンキー)" },
        { KVC_P_DIVIDE,		"/(テンキー)" },
        { KVC_P_PERIOD,		".(テンキー)" },
        { KVC_P_ENTER,		"Enter(テンキー)" },

        // 日本語キーボードのみ
        { KVC_YEN,			"\\" },
        { KVC_RBRACKET,		"]" },
        { KVC_UNDERSCORE,	"_" },
        { KVC_MUHENKAN,		"無変換" },
        { KVC_HENKAN,		"変換" },
        { KVC_HIRAGANA,		"ひらがな" },

        // 英語キーボードのみ
        { KVE_BACKSLASH,	"BackSlash" },

        // 追加キー
        { KVX_RMETA,		"L-Windows" },
        { KVX_LMETA,		"R-Windows" },
        { KVX_MENU,			"Menu" }
    };

    const char *str = NULL;
    for( int i=0; i<(int)(sizeof(kname)/sizeof(PCKeyName)); i++ ){
        if( kname[i].PCKey == sym ){
            str = kname[i].Name;
            break;
        }
    }
    return str;
}


////////////////////////////////////////////////////////////////
// フォントファイル作成
//
// 引数:	hfile		半角フォントファイルパス
//			zfile		全角フォントファイルパス
//			size		文字サイズ(半角文字幅ピクセル数)
// 返値:	BOOL		TRUE:作成成功 FALSE:作成失敗
////////////////////////////////////////////////////////////////
BOOL OSD_CreateFont( char *hfile, char *zfile, int size )
{
    // 半角
    if(hfile){
        QImage fonth(":/res/fonth12.png");
        fonth.save(hfile);
    }

    // 全角
    if(zfile){
        QImage fontz(":/res/fontz12.png");
        fontz.save(zfile);
    }

    return TRUE;
}
