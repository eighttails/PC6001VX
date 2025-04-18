// OS依存の汎用ルーチン(主にUI用)
#include "../typedef.h"
#include "mainwidget.h"

#include <cstdio>

#include <QtCore>
#include <QtWidgets>
#ifndef NOSOUND
#include <QtMultimedia>
#endif

#ifndef NOJOYSTICK
#include <SDL2/SDL.h>
#endif // NOJOYSTICK

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../pc6001v.h"
#include "../joystick.h"
#include "../config.h"
#include "../error.h"
#include "configdialog.h"

#include "renderview.h"
#include "p6vxapp.h"
#include "p6vxcommon.h"

#ifndef NOSOUND
#include "wavfile.h"
#endif

#include "babel.cpp"

//エミュレータ内部用イベントキュー
QQueue<Event> eventQueue;
QMutex eventQueueMutex;
QWaitCondition eventEmitted;
QMutex eventMutex;

//経過時間タイマ
QElapsedTimer elapsedTimer;

std::map<int, PCKEYsym> VKTable;		// Qtキーコード  -> 仮想キーコード 変換テーブル

#ifndef NOSOUND
//サウンド関連
#include "audiooutputwrapper.h"
QPointer<AudioOutputWrapper> audioOutput = nullptr;
QPointer<QThread> audioThread;
#endif

//ジョイスティック関連
#ifndef NOJOYSTICK
QMutex joystickMutex;
#endif // NOJOYSTICK

static const struct {	// Qtキーコード -> 仮想キーコード定義
	int InKey;			// Qtのキーコード
	PCKEYsym VKey;		// 仮想キーコード
} VKeyDef[] = {
{ Qt::Key_unknown,          KVC_UNKNOWN },

{ Qt::Key_1,                KVC_1 },			// 1	!
{ Qt::Key_Exclam,           KVC_1 },			// 1	!
{ Qt::Key_2,                KVC_2 },			// 2	"
{ Qt::Key_QuoteDbl,         KVC_2 },			// 2	"
{ Qt::Key_3,				KVC_3 },			// 3	#
{ Qt::Key_NumberSign,       KVC_3 },			// 3	#
{ Qt::Key_4,				KVC_4 },			// 4	$
{ Qt::Key_Dollar,           KVC_4 },			// 4	$
{ Qt::Key_5,				KVC_5 },			// 5	%
{ Qt::Key_Percent,          KVC_5 },			// 5	%
{ Qt::Key_6,				KVC_6 },			// 6	&
{ Qt::Key_Ampersand,        KVC_6 },			// 6	&
{ Qt::Key_7,				KVC_7 },			// 7	'
{ Qt::Key_Apostrophe,   	KVC_7 },			// 7	'
{ Qt::Key_8,				KVC_8 },			// 8	(
{ Qt::Key_ParenLeft,    	KVC_8 },			// 8	(
{ Qt::Key_9,				KVC_9 },			// 9	)
{ Qt::Key_ParenRight,       KVC_9 },			// 9	)
{ Qt::Key_0,				KVC_0 },			// 0

{ Qt::Key_A,				KVC_A },			// a	A
{ Qt::Key_B,				KVC_B },			// b	B
{ Qt::Key_C,				KVC_C },			// c	C
{ Qt::Key_D,				KVC_D },			// d	D
{ Qt::Key_E,				KVC_E },			// e	E
{ Qt::Key_F,				KVC_F },			// f	F
{ Qt::Key_G,				KVC_G },			// g	G
{ Qt::Key_H,				KVC_H },			// h	H
{ Qt::Key_I,				KVC_I },			// i	I
{ Qt::Key_J,				KVC_J },			// j	J
{ Qt::Key_K,				KVC_K },			// k	K
{ Qt::Key_L,				KVC_L },			// l	L
{ Qt::Key_M,				KVC_M },			// m	M
{ Qt::Key_N,				KVC_N },			// n	N
{ Qt::Key_O,				KVC_O },			// o	O
{ Qt::Key_P,				KVC_P },			// p	P
{ Qt::Key_Q,				KVC_Q },			// q	Q
{ Qt::Key_R,				KVC_R },			// r	R
{ Qt::Key_S,				KVC_S },			// s	S
{ Qt::Key_T,				KVC_T },			// t	T
{ Qt::Key_U,				KVC_U },			// u	U
{ Qt::Key_V,				KVC_V },			// v	V
{ Qt::Key_W,				KVC_W },			// w	W
{ Qt::Key_X,				KVC_X },			// x	X
{ Qt::Key_Y,				KVC_Y },			// y	Y
{ Qt::Key_Z,				KVC_Z },			// z	Z

{ Qt::Key_F1,				KVC_F1 },			// F1
{ Qt::Key_F2,				KVC_F2 },			// F2
{ Qt::Key_F3,				KVC_F3 },			// F3
{ Qt::Key_F4,				KVC_F4 },			// F4
{ Qt::Key_F5,				KVC_F5 },			// F5
{ Qt::Key_F6,				KVC_F6 },			// F6
{ Qt::Key_F7,				KVC_F7 },			// F7
{ Qt::Key_F8,				KVC_F8 },			// F8
{ Qt::Key_F9,				KVC_F9 },			// F9
{ Qt::Key_F10,				KVC_F10 },			// F10
{ Qt::Key_F11,				KVC_F11 },			// F11
{ Qt::Key_F12,				KVC_F12 },			// F12

{ Qt::Key_Minus,			KVC_MINUS },		// -	=
{ Qt::Key_Equal,			KVC_MINUS },		// -	=
{ Qt::Key_AsciiCircum,		KVC_CARET },		// ^	~
{ Qt::Key_Backspace,		KVC_BACKSPACE },	// BackSpace
{ Qt::Key_At,				KVC_AT },			// @	`
{ Qt::Key_QuoteLeft,		KVC_AT },			// @	`
{ Qt::Key_BracketLeft,		KVC_LBRACKET },		// [	{
{ Qt::Key_BraceLeft,		KVC_LBRACKET },		// [	{
{ Qt::Key_Semicolon,		KVC_SEMICOLON },	// ;	+
{ Qt::Key_Plus,             KVC_SEMICOLON },	// ;	+
{ Qt::Key_Colon,			KVC_COLON },		// :	*
{ Qt::Key_Asterisk,			KVC_COLON },		// :	*
{ Qt::Key_Comma,			KVC_COMMA },		// ,	<
{ Qt::Key_Less, 			KVC_COMMA },		// ,	<
{ Qt::Key_Period,			KVC_PERIOD },		// .	>
{ Qt::Key_Greater, 			KVC_PERIOD },		// .	>
{ Qt::Key_Slash,			KVC_SLASH },		// /	?
{ Qt::Key_Question,			KVC_SLASH },		// /	?
{ Qt::Key_Space,			KVC_SPACE },		// Space

{ Qt::Key_Escape,			KVC_ESC },			// ESC
{ Qt::Key_Zenkaku_Hankaku,	KVC_HANZEN },		// 半角/全角
{ Qt::Key_Tab,				KVC_TAB },			// Tab
{ Qt::Key_CapsLock,			KVC_CAPSLOCK },		// CapsLock
{ Qt::Key_Return,			KVC_ENTER },		// Enter
{ Qt::Key_Enter,			KVC_ENTER },		// Enter(テンキー側)
{ Qt::Key_Control,			KVC_LCTRL },		// L-Ctrl
//Qtでは右コントロールキーコードは定義されていない
//{ SDLK_RCTRL,			KVC_RCTRL },		// R-Ctrl
{ Qt::Key_Shift,			KVC_LSHIFT },		// L-Shift
//Qtでは右シフトキーコードは定義されていない
//{ SDLK_RSHIFT,			KVC_RSHIFT },		// R-Shift
{ Qt::Key_Alt,			KVC_LALT },			// L-Alt
//Qtでは右ALTキーコードは定義されていない
//{ SDLK_RALT,			KVC_RALT },			// R-Alt
{ Qt::Key_Print,			KVC_PRINT },		// PrintScreen
{ Qt::Key_ScrollLock,		KVC_SCROLLLOCK },	// ScrollLock
{ Qt::Key_Pause,			KVC_PAUSE },		// Pause
{ Qt::Key_Insert,			KVC_INSERT },		// Insert
{ Qt::Key_Delete,			KVC_DELETE },		// Delete

{ Qt::Key_End,				KVC_END },			// End
{ Qt::Key_Home,             KVC_HOME },			// Home
{ Qt::Key_PageUp,			KVC_PAGEUP },		// PageUp
{ Qt::Key_PageDown,         KVC_PAGEDOWN },		// PageDown

{ Qt::Key_Up,				KVC_UP },			// ↑
{ Qt::Key_Down,         	KVC_DOWN },			// ↓
{ Qt::Key_Left,         	KVC_LEFT },			// ←
{ Qt::Key_Right,			KVC_RIGHT },		// →

//Qtではテンキーを識別しないため、除外する
//  { SDLK_KP0,				KVC_P0 },			// [0]
//  { SDLK_KP1,				KVC_P1 },			// [1]
//  { SDLK_KP2,				KVC_P2 },			// [2]
//  { SDLK_KP3,				KVC_P3 },			// [3]
//  { SDLK_KP4,				KVC_P4 },			// [4]
//  { SDLK_KP5,				KVC_P5 },			// [5]
//  { SDLK_KP6,				KVC_P6 },			// [6]
//  { SDLK_KP7,				KVC_P7 },			// [7]
//  { SDLK_KP8,				KVC_P8 },			// [8]
//  { SDLK_KP9,				KVC_P9 },			// [9]
//  { SDLK_NUMLOCK,			KVC_NUMLOCK },		// NumLock
//  { SDLK_KP_PLUS,			KVC_P_PLUS },		// [+]
//  { SDLK_KP_MINUS,		KVC_P_MINUS },		// [-]
//  { SDLK_KP_MULTIPLY,		KVC_P_MULTIPLY },	// [*]
//  { SDLK_KP_DIVIDE,		KVC_P_DIVIDE },		// [/]
//  { SDLK_KP_PERIOD,		KVC_P_PERIOD },		// [.]
//  { SDLK_KP_ENTER,		KVC_P_ENTER },		// [Enter]

// 日本語キーボードのみ
{ Qt::Key_Backslash,    KVC_YEN },			// ¥	|
{ Qt::Key_BracketRight,	KVC_RBRACKET },		// ]	}
{ Qt::Key_BraceRight,	KVC_RBRACKET },		// ]	}
{ Qt::Key_Underscore,	KVC_UNDERSCORE },	// ¥	_
{ Qt::Key_Muhenkan,     KVC_MUHENKAN },		// 無変換
{ Qt::Key_Henkan,       KVC_HENKAN },		// 変換
{ Qt::Key_Hiragana_Katakana,	KVC_HIRAGANA },		// ひらがな

// 追加キー
{ Qt::Key_Meta,			KVX_LMETA },		// L-Meta
//Qtでは右Metaキーコードは定義されていない
//    { SDLK_RMETA,			KVX_RMETA },		// R-Meta
{ Qt::Key_Menu,			KVX_MENU }			// Menu
};


////////////////////////////////////////////////////////////////
// 小文字による文字列比較
//
// 引数:	s1			比較する文字列へのポインタ1
//		s2			比較する文字列へのポインタ2
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

/////////////////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_Init( void )
{
	// 経過時間タイマーをスタート
	elapsedTimer.start();

	// Qtキーコード  -> 仮想キーコード 変換テーブル初期化
	for( int i=0; i < COUNTOF(VKeyDef); i++ )
		VKTable[VKeyDef[i].InKey] = VKeyDef[i].VKey;

	return OSD_Init_Sub();
}

/////////////////////////////////////////////////////////////////////////////
// 初期化Sub(ライブラリ依存処理等)
//
// 引数:	なし
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_Init_Sub( void )
{
#ifndef NOJOYSTICK
	if( SDL_Init( SDL_INIT_JOYSTICK ) )
		return false;
	SDL_JoystickEventState(SDL_DISABLE);
#endif // NOJOYSTICK
	return true;
}

////////////////////////////////////////////////////////////////
// 終了処理
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Quit( void )
{
	PRINTD( OSD_LOG, "[OSD][OSD_Quit]\n" );

	OSD_Quit_Sub();
}

/////////////////////////////////////////////////////////////////////////////
// 終了処理Sub(ライブラリ依存処理等)
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_Quit_Sub( void )
{
#ifndef NOJOYSTICK
	SDL_Quit();
#endif // NOJOYSTICK
}

/////////////////////////////////////////////////////////////////////////////
// 多重起動チェック
//
// 引数:	なし
// 返値:	bool			true:起動済み false:未起動
/////////////////////////////////////////////////////////////////////////////
bool OSD_IsWorking( void )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
#ifdef NOSINGLEAPP
	return false;
#else
	return app->isSecondary();
#endif
}

////////////////////////////////////////////////////////////////
// OSDキーコード -> 仮想キーコード変換
//
// 引数:	scode		環境依存のキーコード
// 返値:	PCKEYsym	仮想キーコード
////////////////////////////////////////////////////////////////
PCKEYsym OSD_ConvertKeyCode( int scode )
{
	if(VKTable.count(scode) == 0){
		qDebug("keycode %x unknown\n", scode);
		return KVC_UNKNOWN;
	}
	return VKTable[scode];
}






////////////////////////////////////////////////////////////////
// キャプション設定
//
// 引数:	hwnd			ウィンドウハンドル
//		str			キャプション文字列へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW hwnd, const char *str )
{
	QGraphicsView* view = reinterpret_cast<QGraphicsView*>(hwnd);
	if(view == nullptr) return;
	auto window = view->parentWidget();
	QMetaObject::invokeMethod(window, "setWindowTitle",
							  Q_ARG(QString, str));
}

/////////////////////////////////////////////////////////////////////////////
// 設定ファイルパス取得
//
// 引数:	なし
// 返値:	P6VPATH&		取得した文字列への参照(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const P6VPATH& OSD_GetConfigPath()
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetConfigPath]\n" );

	// Windowsの場合はexe本体と同じ場所。
	// それ以外(UNIX系を想定)は ~/.pc6001vx を返す
#if defined Q_OS_WIN
	QString confPath = qApp->applicationDirPath();
#elif defined Q_OS_IOS
	QString confPath = QDir::homePath() + QDir::separator() + QString("Documents");
#else
	QString confPath = QDir::homePath() + QDir::separator() + QString(".pc6001vx4");
#endif
	static P6VPATH cpath = QSTR2P6VPATH(QDir::toNativeSeparators(confPath));
	OSD_AddDelimiter( cpath );	// 念のため
	return cpath;
}

/////////////////////////////////////////////////////////////////////////////
// パスの末尾にデリミタを追加
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AddDelimiter( P6VPATH& path )
{
	QString qPath = P6VPATH2QSTR(path);
	if (qPath.startsWith("content:")){
		// Content URIにおけるデリミタは%2Fとなる(OSのパスデリミタとは別)
		if (!qPath.endsWith("%2F")){
			qPath += "%2F";
		}
		path = QSTR2P6VPATH(qPath);
		return;
	} else if (!qPath.endsWith('/') && !qPath.endsWith(QDir::separator())){
		qPath += QDir::separator();
	}
	path = QSTR2P6VPATH(QDir::toNativeSeparators(qPath));
}


////////////////////////////////////////////////////////////////
// パスの末尾のデリミタを削除
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_DelDelimiter( P6VPATH& path )
{
	QString qPath = P6VPATH2QSTR(path);
	if (qPath.startsWith("content:")){
		// Content URIにおけるデリミタは%2Fとなる(OSのパスデリミタとは別)
		if (qPath.endsWith("%2F")){
			qPath.truncate(qPath.length() - 3);
			path = QSTR2P6VPATH(qPath);
			return;
		}
	} else if (qPath.endsWith('/') || qPath.endsWith(QDir::separator())){
		qPath.truncate(qPath.length() - 1);
	}
	path = QSTR2P6VPATH(QDir::toNativeSeparators(qPath));
}

/////////////////////////////////////////////////////////////////////////////
// 相対パス化
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_RelativePath( P6VPATH& path )
{
#ifdef Q_OS_WIN
	if( QDir( P6VPATH2QSTR(path) ).isRelative()
			|| QDir( P6VPATH2QSTR(path) ).path().startsWith(":")
			|| path == "" ) return;
	QDir dir(P6VPATH2QSTR(OSD_GetConfigPath()));
	QString relPath = dir.relativeFilePath(P6VPATH2QSTR(path));
	path = QSTR2P6VPATH(QDir::toNativeSeparators(relPath));
#else
	// Windows以外では相対パスを使った運用はしない
	OSD_AbsolutePath(path);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 絶対パス化
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AbsolutePath( P6VPATH& path )
{
	QString qPath = P6VPATH2QSTR(path);
	if( !QDir( qPath ).isRelative() || qPath.isEmpty() || qPath.startsWith("content:") ) return;
	QDir dir(P6VPATH2QSTR(OSD_GetConfigPath()));
	path = QSTR2P6VPATH(QDir::toNativeSeparators(QDir::cleanPath(dir.absoluteFilePath(qPath))));
}


/////////////////////////////////////////////////////////////////////////////
// パス結合
//
// 引数:	cpath			結合後パス
//			path1			パス1
//			path2			パス2
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AddPath( P6VPATH& cpath, const P6VPATH& path1, const P6VPATH& path2 )
{
	QDir dir(P6VPATH2QSTR(path1));
	auto qPath1 = P6VPATH2QSTR(path1);
	auto qPath2 = P6VPATH2QSTR(path2);
	QString path;
	if (qPath1.startsWith("content:")){
		if (qPath2.contains("*")){
			// Content URI におけるデリミタは%2Fとなる(OSのパスデリミタとは別)
			path = qPath1 + qPath2;
		} else {
			// ディレクトリ選択で取得した URI 内のファイルパスは
			// content://com.android.externalstorage.documents/tree/(primary or SDCard ID): + フォルダパス + /document/(primary or SDCard ID): + ファイルパス になる。
			QString mPath = qPath1;
			if (qPath1.endsWith("%2F")){
				qPath1.chop(3);
			}
			mPath.replace("content://com.android.externalstorage.documents/tree/", "/document/");
			path += qPath1 + mPath + "%2F" + qPath2;
		}
	} else {
		path = QDir::cleanPath(dir.filePath(P6VPATH2QSTR(path2)));
		if (!path.startsWith(":")) {
			path = QDir::toNativeSeparators(path);
		}
	}
	cpath = QSTR2P6VPATH(path);
}


/////////////////////////////////////////////////////////////////////////////
// パスからフォルダ名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFolderNamePart( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFolderNamePart]\n" );

	QFileInfo info(P6VPATH2QSTR(path));
	return QDir::cleanPath(info.dir().absolutePath()).toStdString();
}


/////////////////////////////////////////////////////////////////////////////
// パスからファイル名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFileNamePart( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNamePart]\n" );

	static QByteArray filename;
	QFileInfo info(P6VPATH2QSTR(path));
	filename = info.fileName().toUtf8();
	return filename.toStdString();
}


/////////////////////////////////////////////////////////////////////////////
// パスから拡張子名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFileNameExt( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNameExt]\n" );

	static QByteArray ext;
	QFileInfo info(P6VPATH2QSTR(path));
	ext = info.suffix().toUtf8();
	return ext.toStdString();
}

/////////////////////////////////////////////////////////////////////////////
// 拡張子名を変更
//
// 引数:	path			パス
//		ext				新しい拡張子への参照
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_ChangeFileNameExt( P6VPATH& path, const std::string& ext )
{
	PRINTD( OSD_LOG, "[OSD][OSD_ChangeFileNameExt] %s -> %s\n", OSD_GetFileNameExt( path ).c_str(), ext.c_str() );

	QString qPath = P6VPATH2QSTR(path);
	QFileInfo info(qPath);
	QString newPath = info.path() + QDir::separator() + info.completeBaseName() + "." + QString::fromStdString(ext);
	path = QSTR2P6VPATH(newPath);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ファイルを開く
//
// 引数:	path			パス
//			mode			モード文字列への参照
// 返値:	FILE*			ファイルポインタ
/////////////////////////////////////////////////////////////////////////////
FILE* OSD_Fopen( const P6VPATH& path, const std::string& mode )
{
	PRINTD( OSD_LOG, "[OSD][OSD_Fopen] %s(%s) ", P6VPATH2STR( path ).c_str(), mode.c_str() );
	QString strFileName = P6VPATH2QSTR(path);
	if (strFileName.startsWith(":")){
		auto file = QFile(strFileName);
		// リソース内のファイルはテンポラリファイルとしてコピーを作成
		QTemporaryFile* tempFile = QTemporaryFile::createNativeFile(file);
		tempFile->setAutoRemove(true);
		// アプリ終了時に削除されるように設定
		tempFile->setParent(qApp);
		return fopen(tempFile->fileName().toLocal8Bit(), mode.c_str());
	} else if (strFileName.contains("content:")){
		auto file = QFile(strFileName);
		// Androidの外部ストレージ内のファイルはテンポラリファイルとしてコピーを作成
		QTemporaryFile* tempFile = QTemporaryFile::createNativeFile(file);
		tempFile->setAutoRemove(true);
		// アプリ終了時に削除されるように設定
		tempFile->setParent(qApp);
		return fopen(tempFile->fileName().toLocal8Bit(), mode.c_str());
	} else {
		return fopen(strFileName.toLocal8Bit(), mode.c_str());
	}
}


/////////////////////////////////////////////////////////////////////////////
// ファイルストリームを開く
//
// 引数:	fs				ファイルストリームへの参照
//			path			パス
//			mode			モード
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FSopen( std::fstream& fs, const P6VPATH& path, const std::ios_base::openmode mode )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FSopen] %s\n", P6VPATH2STR( path ).c_str() );
	QString strFileName = P6VPATH2QSTR(path);
	qDebug() << "OSD_FSopen" << path ;

	if (strFileName.startsWith(":")){
		auto file = QFile(strFileName);
		if (!file.exists() && (mode | std::ios_base::in)) return false;
		// リソース内のファイルはテンポラリファイルとしてコピーを作成
		QTemporaryFile* tempFile = QTemporaryFile::createNativeFile(file);
		tempFile->setAutoRemove(true);
		// アプリ終了時に削除されるように設定
		tempFile->setParent(qApp);
		fs.open( tempFile->fileName().toLocal8Bit(), mode );
	} else if (strFileName.contains("content:")){
		auto file = QFile(EncodeContentURI(strFileName));
		if (!file.exists() && (mode | std::ios_base::in)) return false;
		// Androidの外部ストレージ内のファイルはテンポラリファイルとしてコピーを作成
		QTemporaryFile* tempFile = QTemporaryFile::createNativeFile(file);
		tempFile->setAutoRemove(true);
		// アプリ終了時に削除されるように設定
		tempFile->setParent(qApp);
		fs.open( tempFile->fileName().toLocal8Bit(), mode );
	} else {
		fs.open( strFileName.toLocal8Bit(), mode );
	}
	return fs.is_open() && fs.good();
}


/////////////////////////////////////////////////////////////////////////////
// フォルダを作成
//
// 引数:	path			パス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_CreateFolder( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_CreateFolder] %s\n", P6VPATH2STR( path ).c_str() );

	std::error_code ec;

	P6VPATH tpath = path;
	OSD_DelDelimiter( tpath );	// デリミタ付きだと作成されない?
	OSD_AbsolutePath( tpath );
	PRINTD( OSD_LOG, "-> %s\n", P6VPATH2STR( tpath ).c_str() );

	// 設定ファイルパスより外側には作成しない
	auto configPath = P6VPATH2STR( OSD_GetConfigPath() );
	if( P6VPATH2STR( tpath ).compare( 0, configPath.length(), configPath ) ) return false;

	QDir dir;
	return dir.mkpath(P6VPATH2QSTR(tpath));
}

/////////////////////////////////////////////////////////////////////////////
// ファイルの存在チェック
//
// 引数:	fullpath		パス
// 返値:	bool			true:存在する false:存在しない
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileExist( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileExist]\n" );

	QString pathString = P6VPATH2QSTR(fullpath);

	// ワイルドカードを含む場合
	if (pathString.contains("*")){
		QFileInfoList list;
		if (pathString.startsWith("content:")){
			// ファイル名からフォルダ名を取り出す場合 document -> tree の変換が必要
			auto dir = QDir(pathString.section("%2F", 0, -2).replace(
				"com.android.externalstorage.documents/document/",
				"com.android.externalstorage.documents/tree/"));
			auto wildcard = pathString.section("%2F", -1, -1);
			list = dir.entryInfoList(QStringList(wildcard), QDir::Files);
		} else {
			QFileInfo info(pathString);
			QDir dir = info.absoluteDir();
			QString wildcard = info.fileName();
			list = dir.entryInfoList(QStringList(wildcard), QDir::Files);
		}
		if (list.empty()){
			qDebug() << pathString << " does not exist.";
		}
		return !list.empty();
	} else {
		QFile file(pathString);
		if (!file.exists()){
			qDebug() << pathString << " does not exist.";
		}
		return file.exists();
	}
}

/////////////////////////////////////////////////////////////////////////////
// ファイルサイズ取得
//
// 引数:	fullpath		パス
// 返値:	DWORD			ファイルサイズ
/////////////////////////////////////////////////////////////////////////////
DWORD OSD_GetFileSize( const P6VPATH& fullpath )
{
	QFileInfo info(P6VPATH2QSTR(fullpath));
	return info.size(); // 存在しない場合は0が返る
}

/////////////////////////////////////////////////////////////////////////////
// ファイルの読取り専用チェック
//
// 引数:	fullpath		パス
// 返値:	bool			true:読取り専用 false:読み書き
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileReadOnly( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileReadOnly]\n" );

	QFileInfo info(P6VPATH2QSTR(fullpath));
	return !info.isWritable();
}



/////////////////////////////////////////////////////////////////////////////
// ファイルを削除
//
// 引数:	fullpath		パス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileDelete( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileDelete] %s\n", P6VPATH2STR( fullpath ).c_str() );

	QFile file(P6VPATH2QSTR(fullpath));
	return file.remove();
}


/////////////////////////////////////////////////////////////////////////////
// ファイルを探す
//
// 引数:	path			パス
//		file			探すファイル名
//		folders			見つかったパスを格納するvectorへの参照
//		size			ファイルサイズ (0:チェックしない)
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FindFile( const P6VPATH& path, const P6VPATH& file, std::vector<P6VPATH>& files, size_t size )
{
	std::string sfile = QString::fromStdString(OSD_GetFileNamePart( file )).toLocal8Bit().toStdString();
	std::transform( sfile.begin(), sfile.end(), sfile.begin(), ::tolower );	// 小文字
	auto qPath = P6VPATH2QSTR(path);
	if (qPath.startsWith("content:")){
		P6VPATH filePath;
		OSD_AddPath(filePath, path, file);
		if (QFileInfo(P6VPATH2QSTR(filePath)).exists()){
			files.push_back(filePath);
		}
	} else {
		QDirIterator it(qPath,  QDir::Files, QDirIterator::FollowSymlinks);
		while (it.hasNext()) {
			it.next();
			// パスからファイル名を抽出し、小文字に変換
			std::string tfile = it.fileName().toLower().toLocal8Bit().toStdString();

				   // ファイル名比較。
			if (sfile == tfile){
				// ファイル名一致＆サイズ一致してたら結果に登録
				QFileInfo info(it.filePath());
				if (size_t(info.size()) == size){
					files.push_back(QSTR2P6VPATH(it.filePath()));
				}
			}
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイル名を変更
//
// 引数:	fullpath1		変更元のパス
//		fullpath2		変更するパス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileRename( const P6VPATH& fullpath1, const P6VPATH& fullpath2 )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileRename] %s -> %s\n", P6VPATH2STR( fullpath1 ).c_str(), P6VPATH2STR( fullpath2 ).c_str() );

	QFile file(P6VPATH2QSTR(fullpath1));
	return file.rename(P6VPATH2QSTR(fullpath2));
}


/////////////////////////////////////////////////////////////////////////////
// フォルダの参照
//
// 引数:	hwnd		親のウィンドウハンドル
//		path		パス
// 返値:	bool		true:選択成功 false:エラーorキャンセル
/////////////////////////////////////////////////////////////////////////////
bool OSD_FolderDiaog( HWINDOW hwnd, P6VPATH& path )
{
	bool ret = false;
	char cpath[PATH_MAX];
	strcpy(cpath, P6VPATH2STR(path).c_str());
	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "folderDialog",
							  cType,
							  Q_RETURN_ARG(bool, ret),
							  Q_ARG(void*, hwnd),
							  Q_ARG(char*, cpath));
	if (ret) {
		path = STR2P6VPATH(cpath);
	}
	return ret;
}


/////////////////////////////////////////////////////////////////////////////
// 各種ファイル選択
//
// 引数:	hwnd		親のウィンドウハンドル
//		type		ダイアログの種類(FileDlg参照)
//		fullpath	フルパス
//		path		ファイル検索パス
// 返値:	bool		true:選択成功 false:エラーorキャンセル
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileSelect( HWINDOW hwnd, FileDlg type, P6VPATH& fullpath, P6VPATH& path )
{
	FileMode mode = FM_Load;
	const char *title   = nullptr;
	const char *filter  = nullptr;
	const char *ext     = nullptr;

	switch( type ){
	case FD_TapeLoad:	// TAPE(LOAD)選択
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ選択(LOAD)");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ (*.p6t *.cas *.p6);;"
											   "P6T形式 (*.p6t);;"
											   "ベタ形式 (*.cas *.p6);;"
											   "全てのファイル (*.*)");
		ext    = EXT_P6T;
		break;

	case FD_TapeSave:	// TAPE(SAVE)選択
		mode   = FM_Save;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ選択(SAVE)");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ (*.p6t *.cas *.p6);;"
											   "P6T形式 (*.p6t);;"
											   "ベタ形式 (*.cas *.p6);;"
											   "全てのファイル (*.*)");
		ext    = EXT_P6T;
		break;

	case FD_Disk:		// DISK選択
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "DISKイメージ選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "DISKイメージ (*.d88);;"
											   "D88形式 (*.d88);;"
											   "全てのファイル (*.*)");
		ext    = EXT_DISK;
		break;

	case FD_ExtRom:		// 拡張ROM選択
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "拡張ROMイメージ選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "全てのファイル (*.*)");
		break;

	case FD_Printer:	// プリンター出力ファイル選択
		mode   = FM_Save;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "プリンター出力ファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "全てのファイル (*.*)");
		ext    = EXT_TEXT;
		break;

	case FD_FontZ:		// 全角フォントファイル選択
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "全角フォントファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "PNGファイル (*.png);;"
											   "全てのファイル (*.*)");
		ext    = EXT_IMG;
		break;

	case FD_FontH:		// 半角フォントファイル選択
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "半角フォントファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "PNGファイル (*.png);;"
											   "全てのファイル (*.*)");
		ext    = EXT_IMG;
		break;

	case FD_DokoLoad:	// どこでもLOADファイル選択
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "どこでもLOADファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVEファイル (*.dds *.ddr);;"
											   "どこでもSAVE形式 (*.dds);;"
											   "リプレイファイル (*.ddr);;"
											   "全てのファイル (*.*)");
		ext    = EXT_DOKO;
		break;

	case FD_DokoSave:	// どこでもSAVEファイル選択
		mode   = FM_Save;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVEファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVE形式 (*.dds);;"
											   "全てのファイル (*.*)");
		ext    = EXT_DOKO;
		break;

	case FD_RepLoad:	// リプレイ再生ファイル選択
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "リプレイ再生ファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "リプレイファイル (*.ddr);;"
											   "全てのファイル (*.*)");
		ext    = EXT_REPLAY;
		break;

	case FD_RepSave:	// リプレイ保存ファイル選択
		mode   = FM_Save;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "リプレイ保存ファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "リプレイファイル (*.ddr);;"
											   "全てのファイル (*.*)");
		ext    = EXT_REPLAY;
		break;

	case FD_AVISave:	// ビデオキャプチャ出力ファイル選択
		mode   = FM_Save;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "ビデオキャプチャ出力ファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "WebMファイル (*.webm);;"
											   "全てのファイル (*.*)");
		ext    = EXT_VIDEO;
		break;

	case FD_LoadAll:	// 汎用LOAD
	default:
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "ファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "全てのファイル (*.*)");
		break;
	}

	bool ret = false;
	char cpath[PATH_MAX];
	strcpy(cpath, P6VPATH2STR(path).c_str());
	char cfullpath[PATH_MAX];
	strcpy(cfullpath, P6VPATH2STR(fullpath).c_str());

	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "fileDialog",
							  cType,
							  Q_RETURN_ARG(bool, ret),
							  Q_ARG(void*, hwnd),
							  Q_ARG(FileMode, mode),
							  Q_ARG(const char*, TRANS(title)),
							  Q_ARG(const char*, TRANS(filter)),
							  Q_ARG(char*, cfullpath),
							  Q_ARG(char*, cpath),
							  Q_ARG(const char*, ext));
	if (ret) {
		path = STR2P6VPATH(cpath);
		fullpath = STR2P6VPATH(cfullpath);
	}
	return ret;
}



/////////////////////////////////////////////////////////////////////////////
// メッセージ表示
//
// 引数:	hwnd		親のウィンドウハンドル
//		mes			メッセージ文字列への参照(UTF-8)
//		cap			ウィンドウキャプション文字列への参照(UTF-8)
//		type		表示形式指示のフラグ
// 返値:	int			押されたボタンの種類
//						OSDR_OK:     OKボタン
//						OSDR_CANCEL: CANCELボタン
//						OSDR_YES:    YESボタン
//						OSDR_NO:     NOボタン
/////////////////////////////////////////////////////////////////////////////
int OSD_Message( HWINDOW hwnd, const std::string& mes, const std::string& cap, int type )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	int ret = OSDR_OK;
	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "showMessageBox",
							  cType,
							  Q_RETURN_ARG(int, ret),
							  Q_ARG(void*, app->getView()),
							  Q_ARG(const char*, mes.c_str()),
							  Q_ARG(const char*, cap.c_str()),
							  Q_ARG(int, type));
	return ret;
}


/////////////////////////////////////////////////////////////////////////////
// キーリピート設定
//
// 引数:	repeat			キーリピートの間隔(ms) 0で無効
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_SetKeyRepeat( int repeat )
{
	QApplication::setKeyboardInputInterval(repeat);
}


/////////////////////////////////////////////////////////////////////////////
// 利用可能なジョイスティック数取得
//
// 引数:	なし
// 返値:	int				利用可能なジョイスティック数
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyNum( void )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	return SDL_NumJoysticks();
#else
	return 0;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティック名取得
//
// 引数:	int				インデックス
// 返値:	std::string		ジョイスティック名文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetJoyName( int index )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	const char* name = SDL_JoystickNameForIndex( index );
	std::string tname = name ? name : "(Unknown)";
	return tname;
#else
	return "";
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックオープンされてる？
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
// 返値:	bool			true:OPEN false:CLOSE
/////////////////////////////////////////////////////////////////////////////
bool OSD_OpenedJoy( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	return jinfo && SDL_JoystickGetAttached( reinterpret_cast<SDL_Joystick*>(jinfo) ) ? true : false;
#else
	return false;
#endif // NOJOYSTICK
}

/////////////////////////////////////////////////////////////////////////////
// ジョイスティックオープン
//
// 引数:	int				インデックス
// 返値:	HJOYINFO		ジョイスティック情報へのポインタ
/////////////////////////////////////////////////////////////////////////////
HJOYINFO OSD_OpenJoy( int index )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	return (HJOYINFO)SDL_JoystickOpen( index );
#else
	return (HJOYINFO)nullptr;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティッククローズ
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_CloseJoy( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	SDL_JoystickClose( reinterpret_cast<SDL_Joystick*>(jinfo) );
#endif // NOJOYSTICK
}

/////////////////////////////////////////////////////////////////////////////
// ジョイスティックの軸の数取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
// 返値:	int				軸の数
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyNumAxes( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	return SDL_JoystickNumAxes( reinterpret_cast<SDL_Joystick*>(jinfo) );
#else
	return 1;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの数取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
// 返値:	int				ボタンの数
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyNumButtons( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	return SDL_JoystickNumButtons( reinterpret_cast<SDL_Joystick*>(jinfo) );
#else
	return 0;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックの軸の状態取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
//			int				軸の番号
// 返値:	int				軸の状態(-32768～32767)
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyAxis( HJOYINFO jinfo, int num )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	// HAT(デジタルスティック)から値を取得
	SDL_JoystickUpdate();
	auto hat   = SDL_JoystickGetHat( reinterpret_cast<SDL_Joystick*>(jinfo), 0 );
	int hatVal = 0;
	switch( num ){
	case 0:
		if( hat & SDL_HAT_RIGHT ){ hatVal =  32767; }
		if( hat & SDL_HAT_LEFT  ){ hatVal = -32767; }
		break;

	case 1:
		if( hat & SDL_HAT_UP    ){ hatVal = -32767; }
		if( hat & SDL_HAT_DOWN  ){ hatVal =  32767; }
		break;

	default:;
	}

	// アナログスティックから値を取得
	int axisVal = SDL_JoystickGetAxis( reinterpret_cast<SDL_Joystick*>(jinfo), num );

	// 出力値はHAT優先
	return hatVal ? hatVal : axisVal;
#else
	return 0;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの状態取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
//			int				ボタンの番号
// 返値:	bool			ボタンの状態 true:ON false:OFF
/////////////////////////////////////////////////////////////////////////////
bool OSD_GetJoyButton( HJOYINFO jinfo, int num )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
	SDL_JoystickUpdate();
	return SDL_JoystickGetButton( reinterpret_cast<SDL_Joystick*>(jinfo), num ) ? true : false;
#else
	return false;
#endif // NOJOYSTICK
}

/////////////////////////////////////////////////////////////////////////////
// オーディオデバイスオープン
//
// 引数:	obj				自分自身へのオブジェクトポインタ
//			callback		コールバック関数へのポインタ
//			rate			サンプリングレート
//			sample			バッファサイズ(サンプル数)
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_OpenAudio( void* obj, CBF_SND callback, int rate, int samples )
{
#ifndef NOSOUND
	// 実行時に出る警告の抑止
	qRegisterMetaType<QAudio::State>();

	QAudioFormat format;
	format.setChannelConfig(QAudioFormat::ChannelConfigMono);
	format.setSampleRate(rate);
	format.setSampleFormat(QAudioFormat::Int16);

	if(!audioOutput.isNull()){
		audioOutput->deleteLater();
	}

	QAudioDevice device = QMediaDevices::defaultAudioOutput();
	audioOutput = new AudioOutputWrapper(device, format, callback, obj, samples);
	audioThread = new QThread(qApp);
	audioOutput->moveToThread(audioThread);
	audioThread->start();
#endif
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// オーディオデバイスクローズ
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_CloseAudio( void )
{
#ifndef NOSOUND
	if(!audioOutput.isNull()){
		QMetaObject::invokeMethod(audioOutput, "stop");
		audioOutput->deleteLater();
		audioThread->exit();
		audioThread->deleteLater();
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 再生開始
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_StartAudio( void )
{
#ifndef NOSOUND
	if(audioOutput){
		if(audioOutput->state() == QAudio::SuspendedState){
			QMetaObject::invokeMethod(audioOutput, "resume");
		} else {
			QMetaObject::invokeMethod(audioOutput, "start");
		}
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 再生停止
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_StopAudio( void )
{
#ifndef NOSOUND
	if(audioOutput){
		QMetaObject::invokeMethod(audioOutput, "suspend");
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 再生状態取得
//
// 引数:	なし
// 返値:	bool			true:再生中 false:停止中
/////////////////////////////////////////////////////////////////////////////
bool OSD_AudioPlaying( void )
{
#ifndef NOSOUND
	if(audioOutput){
		return audioOutput->state() == QAudio::ActiveState;
	} else {
		return false;
	}
#else
	return false;
#endif
}

#ifdef NOCALLBACK	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// オーディオキューサンプル数取得
// 引数:	なし
// 返値:	int				キューサンプル数
/////////////////////////////////////////////////////////////////////////////
int OSD_GetQueuedAudioSamples( void )
{
	return audioOutput->queuedAudioSamples();
}

/////////////////////////////////////////////////////////////////////////////
// オーディオストリーム書き込み
//
// 引数:	stream			オーディオデータのストリーム
//		samples			サンプル数
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_WriteAudioStream(BYTE *stream, int samples)
{
	audioOutput->writeAudioStream(stream, samples);
}

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/////////////////////////////////////////////////////////////////////////////
// Waveファイル読込み
// 　対応形式は 22050Hz以上,符号付き16bit,1ch
//
// 引数:	filepath		入力ファイルパス
//			buf				バッファポインタ格納ポインタ
//			len				ファイル長さ格納ポインタ
//			freq			サンプリングレート格納ポインタ
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_LoadWAV( const P6VPATH& filepath, BYTE** buf, DWORD* len, int* freq )
{
#ifndef NOSOUND
	WavFile w;
	if(!w.open(P6VPATH2STR(filepath).c_str())) return false;

	const QAudioFormat& format = w.fileFormat();
	size_t bodySize = w.size() - w.headerLength();
	*len = bodySize;
	*freq = format.sampleRate();
	BYTE* buffer = new BYTE[bodySize];
	memcpy(buffer, &w.readAll().data()[w.headerLength()], bodySize);
	*buf = buffer;
	return true;
#else
	return false;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// Waveファイル開放
//
// 引数:	buf				バッファへのポインタ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_FreeWAV( BYTE* buf )
{
	if(buf) delete[] buf;
}


/////////////////////////////////////////////////////////////////////////////
// オーディオをロックする
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_LockAudio( void )
{
	// 何もしない
}


/////////////////////////////////////////////////////////////////////////////
// オーディオをアンロックする
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_UnlockAudio( void )
{
	// 何もしない
}


////////////////////////////////////////////////////////////////
// 指定時間待機
//
// 引数:	tms			待機時間(ms)
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Delay( DWORD tms )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	int delayResolution = app->getSetting(P6VXApp::keyTimerResolution).toInt();
	int ms = (tms / delayResolution) * delayResolution;
	if (ms >= 0) {
		QThread::msleep(ms);
	}
}


////////////////////////////////////////////////////////////////
// プロセス開始からの経過時間取得
//
// 引数:	なし
// 返値:	DWORD		経過時間(ms)
////////////////////////////////////////////////////////////////
DWORD OSD_GetTicks( void )
{
	return elapsedTimer.elapsed();
}

/////////////////////////////////////////////////////////////////////////////
// タイマ追加
//
// 引数:	interval		割込み間隔(ms)
//			callback		コールバック関数
//			param			コールバック関数に渡す引数
// 返値:	TIMERID			タイマID(失敗したら0)
/////////////////////////////////////////////////////////////////////////////
TIMERID OSD_AddTimer( DWORD interval, CBF_TMR callback, void* param )
{
	// 何もしない(失敗を返す)
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// タイマ削除
//
// 引数:	id				タイマID
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_DelTimer( TIMERID id )
{
	// 何もしない(失敗を返す)
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ作成
//
// 引数:	hwnd			ウィンドウハンドルへのポインタ
//			w				ウィンドウの幅
//			h				ウィンドウの高さ
//			sw				スクリーンの幅
//			sh				スクリーンの高さ
//			fsflag			true:フルスクリーン false:ウィンドウ
//			filter			true:拡縮時にフィルタリングする false:しない
//			scanbr			スキャンライン輝度
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_CreateWindow( HWINDOW* hwnd, const int w, const int h, const int sw, const int sh, const bool fsflag, const bool filter, const int scanbr )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	RenderView* view = app->getView();
	QGraphicsScene* scene = view->scene();

	scene->setSceneRect(0, 0, w, h);
	*hwnd = view;

	QMetaObject::invokeMethod(qApp, "createWindow",
							  Q_ARG(HWINDOW, *hwnd),
							  Q_ARG(bool, fsflag));

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ破棄
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_DestroyWindow( HWINDOW hwnd )
{
	// 何もしない
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウの幅を取得
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	int				幅
/////////////////////////////////////////////////////////////////////////////
int OSD_GetWindowWidth( HWINDOW hwnd )
{
	// QtではSceneRectの幅を返す
	QGraphicsView* view = reinterpret_cast<QGraphicsView*>(hwnd);
	Q_ASSERT(view);

	return view->scene()->width();
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウの高さを取得
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	int				高さ
/////////////////////////////////////////////////////////////////////////////
int OSD_GetWindowHeight( HWINDOW hwnd )
{
	// QtではSceneRectの高さを返す
	QGraphicsView* view = reinterpret_cast<QGraphicsView*>(hwnd);
	Q_ASSERT(view);

	return view->scene()->height();
}


/////////////////////////////////////////////////////////////////////////////
// フルスクリーン?
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	bool			true:フルスクリーン表示 false:ウィンドウ表示
/////////////////////////////////////////////////////////////////////////////
bool OSD_IsFullScreen( HWINDOW hwnd )
{
	RenderView* view = reinterpret_cast<RenderView*>(hwnd);
	auto MainWidget = view ->parentWidget();
	return MainWidget->isFullScreen();
}


/////////////////////////////////////////////////////////////////////////////
// フィルタリング有効?
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	bool			true:有効 false:無効
/////////////////////////////////////////////////////////////////////////////
bool OSD_IsFiltering( HWINDOW hwnd )
{
	RenderView* view = reinterpret_cast<RenderView*>(hwnd);
	// シーングラフの中のメイン画面(原点に配置)オブジェクトに
	// フィルタリングがかかっているかどうかで判断
	QGraphicsPixmapItem* item =
			dynamic_cast<QGraphicsPixmapItem*>(view->scene()->itemAt(0, 0, QTransform()));
	if (!item) {
		return false;
	}
	auto mode = item->transformationMode();
	return mode == Qt::SmoothTransformation;
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウのサイズ変更可否設定
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	bool			true:変更可 false:変更不可
/////////////////////////////////////////////////////////////////////////////
void OSD_SetWindowResizable( HWINDOW hwnd, bool resize )
{
	// 何もしない(常にリサイズ可能)
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウクリア
//  色は0(黒)で決め打ち
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_ClearWindow( HWINDOW hwnd )
{
	QMetaObject::invokeMethod(qApp, "clearLayout",
							  Q_ARG(HWINDOW, hwnd));
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウ反映
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_RenderWindow( HWINDOW hwnd )
{
	// 何もしない
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウに転送(等倍)
//
// 引数:	hwnd			ウィンドウハンドル
//			src				転送元サーフェス
//			x				転送先x座標
//			y				転送先y座標
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_BlitToWindow( HWINDOW hwnd, VSurface* src, const int x, const int y )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	VRect src1,drc1;

	QImage image(src->Width(), src->Height(), QImage::Format_Indexed8);
	image.setColorTable(app->getPaletteTable());

	// 転送元範囲設定
	src1.x = 0;
	src1.y = 0;
	src1.w = src->Width();
	src1.h = src->Height();

	if( src1.w <= 0 || src1.h <= 0 ) return;

	// 転送先範囲設定
	drc1.x = 0;
	drc1.y = 0;

	BYTE *psrc = src->GetPixels().data() + src->Pitch() * src1.y + src1.x;

	const int length = image.bytesPerLine();
	for( int i=0; i < src1.h; i++ ){
		BYTE *pdst = (BYTE *)image.scanLine(i);
		memcpy( pdst, psrc, length );
		psrc += src->Pitch();
	}
	image = image.convertToFormat(QImage::Format_RGBX8888);
	// 表示用のQPixmapItemへの変換はメインスレッドでないとできないため、
	// スロットを呼び出してメインスレッドでSceneを更新する
	// (直接呼び出すと呼び出し側スレッドで実行されてしまう)
	QMetaObject::invokeMethod(qApp, "layoutBitmap",
							  Q_ARG(HWINDOW, hwnd),
							  Q_ARG(int, x),
							  Q_ARG(int, y),
							  Q_ARG(double, 1.0),
							  Q_ARG(double, 1.0),
							  Q_ARG(QImage, image));
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウに転送(拡大等)
//
// 引数:	hwnd			ウィンドウハンドル
//		src				転送元サーフェス
//		pos				転送先 座標/サイズ
//		scanen			スキャンラインフラグ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_BlitToWindowEx( HWINDOW hwnd, VSurface* src, const VRect* pos, const bool scanen )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	VRect src1,drc1;
	if( !src || !hwnd ) return;

	const int s = (scanen ? 2 : 1);   // スキャンライン時には2行ずつ処理する

	const BYTE *spt  = src->GetPixels().data();
	const int pp     = src->Pitch();

	const int ww     = src->Width();
	const int hh     = src->Height();


	QImage image(src->Width(), src->Height() * s, QImage::Format_Indexed8);
	image.fill(Qt::black);
	image.setColorTable(app->getPaletteTable());

	const int dpp    = image.bytesPerLine();

	// 転送元範囲設定
	src1.x = qMax( 0, -pos->x );
	src1.y = qMax( 0, -pos->y );
	src1.w = qMin( ww - src1.x, image.width() );
	src1.h = qMin( hh - src1.y, image.height() );

	if( src1.w <= 0 || src1.h <= 0 ) return;

	// 転送先範囲設定
	drc1.x = qMax( 0, pos->x );
	drc1.y = qMax( 0, pos->y );
	drc1.w = qMin( ww, (image.width() - drc1.x) );
	drc1.h = qMin( hh * s, image.height() - drc1.y );

	for( int y=0; y<drc1.h; y+=s ){
		BYTE *sof  = (BYTE *)spt  + pp * y / s + src1.x;

		memcpy( (BYTE *)image.scanLine(y), sof, dpp );

		if(scanen){
			BYTE *sdoff = (BYTE *)image.scanLine(y+1);
			memcpy( sdoff, sof, dpp );
			for( int x=0; x<drc1.w; x++ ){
				// スキャンライン用の暗い色は64ずらしたインデックスに登録されている
				(*sdoff++) += 64;
			}
		}
	}

	double imgXScale = double(pos->w) / image.width();
	double imgYScale = double(pos->h) / image.height();
	// 表示用のQPixmapItemへの変換はメインスレッドでないとできないため、
	// スロットを呼び出してメインスレッドでSceneを更新する
	// (直接呼び出すと呼び出し側スレッドで実行されてしまう)

	QMetaObject::invokeMethod(qApp, "layoutBitmap",
							  Q_ARG(HWINDOW, hwnd),
							  Q_ARG(int, pos->x),
							  Q_ARG(int, pos->y),
							  Q_ARG(double, imgXScale),
							  Q_ARG(double, imgYScale),
							  Q_ARG(QImage, image));
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウのイメージデータ取得
//
// 引数:	hwnd			ウィンドウハンドル
//			pixels			転送先配列への参照
//			pos				保存する領域情報へのポインタ
//			pxfmt			ピクセルフォーマット
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_GetWindowImage( HWINDOW hwnd, std::vector<BYTE>& pixels, VRect* pos, PixelFMT pxfmt )
{
	if( !hwnd ) return false;
	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "getWindowImage",
							  cType,
							  Q_ARG(HWINDOW, hwnd),
							  Q_ARG(QRect, QRect(pos->x, pos->y, pos->w, pos->h)),
							  Q_ARG(void*, (void*)pixels.data()));

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// アイコン設定
//
// 引数:	hwnd			ウィンドウハンドル
//			model			機種 60,61,62,66,64,68
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_SetIcon( HWINDOW hwnd, int model )
{
	// 機種別P6オブジェクト確保
	const char* iconRes = nullptr;
	switch( model ){
	case 68: iconRes = ":/res/PC-6601SR.ico"; break;
	case 66: iconRes = ":/res/PC-6601.ico"; break;
	case 64: iconRes = ":/res/PC-6001mk2SR.ico"; break;
	case 62: iconRes = ":/res/PC-6001mk2.ico"; break;
	default: iconRes = ":/res/PC-6001.ico";
	}

	// アイコン設定
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	QMetaObject::invokeMethod(app, "setWindowIcon",
							  Q_ARG(QIcon, QIcon(iconRes)));
}


/////////////////////////////////////////////////////////////////////////////
// キャプション設定
//
// 引数:	hwnd			ウィンドウハンドル
//			str				キャプション文字列への参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW hwnd, const std::string& str )
{
	QGraphicsView* view = reinterpret_cast<QGraphicsView*>(hwnd);
	if(view == nullptr) return;
	auto window = view->parentWidget();
	QMetaObject::invokeMethod(window, "setWindowTitle",
							  Q_ARG(QString, QString::fromStdString(str)));
}

/////////////////////////////////////////////////////////////////////////////
// マウスカーソル表示/非表示
//
// 引数:	disp			true:表示 false:非表示
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_ShowCursor( bool disp )
{
	if (disp) {
		QMetaObject::invokeMethod(qApp, "showMouseCursor");
	} else {
		QMetaObject::invokeMethod(qApp, "hideMouseCursor");
	}
}

/////////////////////////////////////////////////////////////////////////////
// OS依存のウィンドウハンドルを取得
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	void*			OS依存のウィンドウハンドル
/////////////////////////////////////////////////////////////////////////////
void* OSD_GetWindowHandle( HWINDOW hwnd )
{
	// 使わない
	assert(false);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// 環境設定ダイアログ表示
//
// 引数:	hwnd		ウィンドウハンドル
// 返値:	int			1:OK 0:CANCEL -1:ERROR
/////////////////////////////////////////////////////////////////////////////
int OSD_ConfigDialog( HWINDOW hwnd )
{
	// INIファイルを開く
	try{
		std::shared_ptr<CFG6> ecfg(new CFG6());
		if( !ecfg->Init() ) throw Error::IniReadFailed;

		QGraphicsView* view = reinterpret_cast<QGraphicsView*>(hwnd);
		auto window = view->parentWidget();

		ConfigDialog dialog(ecfg, window);
#ifdef ALWAYSFULLSCREEN
#ifdef Q_OS_ANDROID
		// Androidの場合はQt::WindowMaximizedを使わないと正しいサイズで描画されない。
		dialog.setWindowState(dialog.windowState() | Qt::WindowMaximized);
#else
		dialog.setWindowState(dialog.windowState() | Qt::WindowFullScreen);
#endif
#endif
		dialog.exec();
		int ret = dialog.result();
		// OKボタンが押されたならINIファイル書込み
		if( ret == QDialog::Accepted) {
			ecfg->Write();
			// レイアウトに関する設定を画面に反映
			qobject_cast<MainWidget*>(window)->updateLayout();
		}
		return ret;
	}
	// new に失敗した場合
	catch( std::bad_alloc& ){
		return -1;
	}
	// 例外発生
	catch( Error::Errno i ){
		return -1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// バージョン情報表示
//
// 引数:	hwnd		ウィンドウハンドル
//			mdl			機種
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_VersionDialog( HWINDOW hwnd, int mdl )
{
	QWidget* parent = reinterpret_cast<QWidget*>(hwnd);
	QMessageBox::about(
				parent, "About PC6001VX",
				APPNAME " Version " VERSION
				"\n"
				"https://eighttails.seesaa.net\n"
				"©2012-, Tadahito Yao (@eighttails)\n"
				"Based on PC6001V by Yumitaro."
				);
}


/////////////////////////////////////////////////////////////////////////////
// イベントキュークリア
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_FlushEvents( void )
{
	QMutexLocker lock(&eventQueueMutex);
	eventQueue.clear();
}


/////////////////////////////////////////////////////////////////////////////
// イベント取得(イベントが発生するまで待つ)
//
// 引数:	ev				イベント情報共用体へのポインタ
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_GetEvent( Event *ev )
{
	bool isEmpty = true;
	{
		QMutexLocker lock(&eventQueueMutex);
		isEmpty = eventQueue.empty();
	}
	// イベントキューが空の場合、イベントが発行されるまで待つ
	if (isEmpty){
		QMutexLocker elock(&eventMutex);
		eventEmitted.wait(&eventMutex);
	}
	{
		QMutexLocker lock(&eventQueueMutex);
		*ev = eventQueue.front();
		eventQueue.pop_front();
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev				イベントタイプ
//			...				イベントタイプに応じた引数
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_PushEvent(const Event& ev)
{
	// イベントキューにプッシュ
	QMutexLocker lock(&eventQueueMutex);
	eventQueue.push_back(ev);
	eventEmitted.wakeAll();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev				イベントタイプ
//			...				イベントタイプに応じた引数
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_PushEvent( EventType ev, ... )
{
	Event event;
	event.type = ev;
	va_list args;
	va_start( args, ev );

	switch( ev ){
	case EV_DEBUGMODEBP:
		event.bp.addr = va_arg( args, int );
		break;
	default:;
	}

	va_end( args );

	return OSD_PushEvent( event );
}


/////////////////////////////////////////////////////////////////////////////
// キューに指定のイベントが存在するか調査する
//
// 引数:	ev				イベントタイプ
// 返値:	bool			true:ある false:ない
/////////////////////////////////////////////////////////////////////////////
bool OSD_HasEvent( EventType ev )
{
	QMutexLocker lock(&eventQueueMutex);
	foreach(auto e, eventQueue) {
		if (e.type == ev) return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// イベント処理の状態を種類ごとに設定する
//
// 引数:	ev				イベントタイプ
//			st				ステータス
// 返値:	bool			変更前の状態 true:有効 false:無効
/////////////////////////////////////////////////////////////////////////////
bool OSD_EventState( EventType ev, EventState st )
{
	// 何もしない(呼ばれない)
	assert(false);
	return false;
}

////////////////////////////////////////////////////////////////
// フォントファイル作成
//
// 引数:	hfile		半角フォントファイルパス
//		zfile		全角フォントファイルパス
//		size		文字サイズ(半角文字幅ピクセル数)
// 返値:	bool		true:作成成功 false:作成失敗
////////////////////////////////////////////////////////////////
bool OSD_CreateFont( const char *hfile, const char *zfile, int size )
{
	// 何もしない
	// フォントファイルはQtのリソースから読み込む
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ShiftJIS -> UTF-8
//
// 引数:	str			文字列バッファへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_SJIStoUTF8( std::string& str )
{
	str = babel::sjis_to_utf8(str);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// UTF-8 -> ShiftJIS
//
// 引数:	str			文字列バッファへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_UTF8toSJIS( std::string& str )
{
	str = babel::utf8_to_sjis(str);
	return true;
}
