// OS依存の汎用ルーチン(主にUI用)

#include <QtCore>
#include <QtGui>

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../pc6001v.h"
#include "../p6el.h"
#include "../joystick.h"

#include "signalproxy.h"

//エミュレータ内部用イベントキュー
QMutex eventMutex;
QQueue<Event> eventQueue;
QWaitCondition eventEmitted;

//経過時間タイマ
QElapsedTimer elapsedTimer;

std::map<int, PCKEYsym> VKTable;			// Qtキーコード  -> 仮想キーコード 変換テーブル
QVector<QRgb> PaletteTable;              //パレットテーブル

//エミュレータのイベントループから
//Qtのイベントループにシグナルを送るための仲介スレッド
SignalProxy signalProxy;

static const struct {	// SDLキーコード -> 仮想キーコード定義
    int InKey;			// SDLのキーコード
    PCKEYsym VKey;		// 仮想キーコード
} VKeyDef[] = {
    { Qt::Key_unknown,          KVC_UNKNOWN },

    { Qt::Key_1,                KVC_1 },			// 1	!
    { Qt::Key_2,                KVC_2 },			// 2	"
    { Qt::Key_3,				KVC_3 },			// 3	#
    { Qt::Key_4,				KVC_4 },			// 4	$
    { Qt::Key_5,				KVC_5 },			// 5	%
    { Qt::Key_6,				KVC_6 },			// 6	&
    { Qt::Key_7,				KVC_7 },			// 7	'
    { Qt::Key_8,				KVC_8 },			// 8	(
    { Qt::Key_9,				KVC_9 },			// 9	)
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
    { Qt::Key_AsciiCircum,		KVC_CARET },		// ^	~
    { Qt::Key_Backspace,		KVC_BACKSPACE },	// BackSpace
    { Qt::Key_At,				KVC_AT },			// @	`
    { Qt::Key_BracketLeft,		KVC_LBRACKET },		// [	{
    { Qt::Key_Semicolon,		KVC_SEMICOLON },	// ;	+
    { Qt::Key_Colon,			KVC_COLON },		// :	*
    { Qt::Key_Comma,			KVC_COMMA },		// ,	<
    { Qt::Key_Period,			KVC_PERIOD },		// .	>
    { Qt::Key_Slash,			KVC_SLASH },		// /	?
    { Qt::Key_Space,			KVC_SPACE },		// Space

    { Qt::Key_Escape,			KVC_ESC },			// ESC
    { Qt::Key_Zenkaku_Hankaku,	KVC_HANZEN },		// 半角/全角
    { Qt::Key_Tab,				KVC_TAB },			// Tab
    { Qt::Key_CapsLock,		KVC_CAPSLOCK },		// CapsLock
    { Qt::Key_Enter,			KVC_ENTER },		// Enter
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
    { Qt::Key_yen,          KVC_YEN },			// ¥	|
    { Qt::Key_BraceRight,	KVC_RBRACKET },		// ]	}
    { Qt::Key_Underscore,	KVC_UNDERSCORE },	// ¥	_
//	{ 				,		KVC_MUHENKAN },		// 無変換
//	{ 				,		KVC_HENKAN },		// 変換
//	{ 				,		KVC_HIRAGANA },		// ひらがな

    // 追加キー
    { Qt::Key_Meta,			KVX_LMETA },		// L-Meta
    //Qtでは右Metaキーコードは定義されていない
//    { SDLK_RMETA,			KVX_RMETA },		// R-Meta
    { Qt::Key_Menu,			KVX_MENU }			// Menu
};


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_Init( void )
{
    //エミュレータのイベントループから
    //Qtのイベントループにシグナルを送るための仲介スレッド
    signalProxy.moveToThread(qApp->thread());
    QObject::connect(&signalProxy, SIGNAL(imageUpdated(HWINDOW, int, int, double, QImage)), qApp, SLOT(layoutBitmap(HWINDOW, int, int, double, QImage)));

    //経過時間タイマーをスタート
    elapsedTimer.start();

    // Qtキーコード  -> 仮想キーコード 変換テーブル初期化
    for( int i=0; i < COUNTOF(VKeyDef); i++ )
        VKTable[VKeyDef[i].InKey] = VKeyDef[i].VKey;

    return true;
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
        return KVC_UNKNOWN;
    }
    return VKTable[scode];
}

////////////////////////////////////////////////////////////////
// 指定時間待機
//
// 引数:	tms			待機時間(ms)
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Delay( DWORD tms )
{
    //QThread::mleepはスレッドクラスからしか使えないので、仕方なく
    class MySleepThread : public QThread
    {
    public:
        static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    };
    MySleepThread::msleep(tms);
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


////////////////////////////////////////////////////////////////
// イベント取得(イベントが発生するまで待つ)
//
// 引数:	ev			イベント情報共用体へのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_GetEvent( Event *ev )
{
    //イベントが発行されるまで待つ
    eventMutex.lock();
    eventEmitted.wait(&eventMutex);
    *ev = eventQueue.front();
    eventQueue.pop_front();
    eventMutex.unlock();

    return true;
}
////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev			イベントタイプ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_PushEvent(EventType ev)
{
    Event event;
    event.type = ev;
    return OSD_PushEvent(event);
}

////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev			イベント
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_PushEvent(const Event& ev)
{
    //イベントキューにプッシュ
    eventMutex.lock();
    eventQueue.push_back(ev);
    eventEmitted.wakeAll();
    eventMutex.unlock();
    return true;
}



////////////////////////////////////////////////////////////////
// マウスカーソル表示/非表示
//
// 引数:	disp		true:表示 false:非表示
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_ShowCursor( bool disp )
{
    qApp->setOverrideCursor(disp ? Qt::ArrowCursor : Qt::BlankCursor);
}


////////////////////////////////////////////////////////////////
// キャプション設定
//
// 引数:	wh			ウィンドウハンドル
//			str			キャプション文字列へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW wh, const char *str )
{
    //#PENDING
}



////////////////////////////////////////////////////////////////
// ウィンドウ作成
//
// 引数:	pwh			ウィンドウハンドルへのポインタ
//			w			幅
//			h			高さ
//			bpp			色深度
//			fsflag		true:フルスクリーン false:ウィンドウ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_CreateWindow( HWINDOW *pwh, int w, int h, int bpp, bool fsflag )
{
    //#PENDING sceneのリーク対策
    static QGraphicsScene* scene = new QGraphicsScene();
    static QGraphicsView* view = new QGraphicsView(scene);
    scene->moveToThread(qApp->thread());
    view->moveToThread(qApp->thread());
    scene->setSceneRect(0, 0, w, h);
    *pwh = view;
    view->show();
    return true;
}


////////////////////////////////////////////////////////////////
// ウィンドウ破棄
//
// 引数:	wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_DestroyWindow( HWINDOW wh )
{
}


////////////////////////////////////////////////////////////////
// ウィンドウの幅を取得
//
// 引数:	wh			ウィンドウハンドル
// 返値:	int			幅
////////////////////////////////////////////////////////////////
int OSD_GetWindowWidth( HWINDOW wh )
{
    //QtではSceneRectの幅を返す
    QGraphicsView* view = static_cast<QGraphicsView*>(wh);
    Q_ASSERT(view);

    return view->scene()->width();
}


////////////////////////////////////////////////////////////////
// ウィンドウの高さを取得
//
// 引数:	wh			ウィンドウハンドル
// 返値:	int			高さ
////////////////////////////////////////////////////////////////
int OSD_GetWindowHeight( HWINDOW wh )
{
    //QtではSceneRectの幅を返す
    QGraphicsView* view = static_cast<QGraphicsView*>(wh);
    Q_ASSERT(view);

    return view->scene()->width();
}


////////////////////////////////////////////////////////////////
// ウィンドウの色深度を取得
//
// 引数:	wh			ウィンドウハンドル
// 返値:	int			色深度
////////////////////////////////////////////////////////////////
int OSD_GetWindowBPP( HWINDOW wh )
{
    //内部カラーは8ビット固定
    return 8;
}


////////////////////////////////////////////////////////////////
// パレット設定
//
// 引数:	wh			ウィンドウハンドル
//			pal			パレットへのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_SetPalette( HWINDOW wh, VPalette *pal )
{
    PaletteTable.clear();
    for (int i=0; i < pal->ncols; i++){
        COLOR24& col = pal->colors[i];
        PaletteTable.push_back(qRgb(col.r, col.g, col.b));
    }
    return true;
}

////////////////////////////////////////////////////////////////
// ウィンドウ反映
//
// 引数:	wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_RenderWindow( HWINDOW wh )
{
    QGraphicsView* view = static_cast<QGraphicsView*>(wh);
    Q_ASSERT(view);
    view->update();
}


////////////////////////////////////////////////////////////////
// ウィンドウクリア
//  色は0(黒)で決め打ち
//
// 引数:	wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_ClearWindow( HWINDOW wh )
{
    QGraphicsView* view = static_cast<QGraphicsView*>(wh);
    Q_ASSERT(view);
    view->scene()->clear();
}


////////////////////////////////////////////////////////////////
// ウィンドウに転送
//
// 引数:	wh			ウィンドウハンドル
//			src			転送元サーフェス
//			pal			パレットへのポインタ
//			x,y			転送先座標
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_BlitToWindow( HWINDOW wh, VSurface *src, int x, int y, VPalette *pal )
{
    VRect src1,drc1;

    //#PENDING パフォーマンスが悪いようならインスタンスを作らないようにする
    QImage image(src->Width(), src->Height(), QImage::Format_Indexed8);
    image.setColorTable(PaletteTable);

    // 転送元範囲設定
    src1.x = 0;
    src1.y = 0;
    src1.w = src->Width();
    src1.h = src->Height();

    if( src1.w <= 0 || src1.h <= 0 ) return;

    // 転送先範囲設定
    drc1.x = 0;
    drc1.y = 0;

    BYTE *psrc = (BYTE *)src->GetPixels() + src->Pitch() * src1.y + src1.x * src->Bpp() / 8;

    for( int i=0; i < src1.h; i++ ){
        BYTE *pdst = (BYTE *)image.scanLine(i);
        memcpy( pdst, psrc, image.bytesPerLine() );
        psrc += src->Pitch();
    }

    //#PENDING 縦横比調整
    double aspect = 1.0;
    signalProxy.updateImage(wh, x, y, aspect, image);
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
// 設定及び諸ファイル(ROMなどの初期位置)フォルダ作成
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
bool OSD_CreateConfigPath()
{
    QString basePath = QString::fromUtf8(OSD_GetConfigPath());

    // ROMイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(ROM_DIR)))
        return false;

    // TAPEイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(TAPE_DIR)))
        return false;

    // DISKイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(DISK_DIR)))
        return false;

    // 拡張ROMイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(EXTROM_DIR)))
        return false;

    // スナップショット
    if(!QDir(basePath).mkpath(QString::fromUtf8(IMAGE_DIR)))
        return false;

    // フォント
    if(!QDir(basePath).mkpath(QString::fromUtf8(FONT_DIR)))
        return false;

    // WAVEファイル
    if(!QDir(basePath).mkpath(QString::fromUtf8(WAVE_DIR)))
        return false;

    return true;
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
    QString confPath = qApp->applicationDirPath() + QDir::separator();
#else
    QString confPath = QDir::homePath() + QDir::separator() + QString(".pc6001vx");
#endif
    sprintf( mpath, "%s", confPath.toUtf8().data() );
    AddDelimiter( mpath );	// 念のため
    UnDelimiter( mpath );
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

    QFileInfo info(QString::fromUtf8(path));
    return info.fileName().toUtf8().data();
}


////////////////////////////////////////////////////////////////
// ファイルの存在チェック
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	bool		true:存在する false:存在しない
////////////////////////////////////////////////////////////////
bool OSD_FileExist( const char *fullpath )
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
// 返値:	bool		true:読取り専用 false:読み書き
///////////////////////////////////////////////////////////////
bool OSD_FileReadOnly( const char *fullpath )
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
    QByteArray result = QFileDialog::getExistingDirectory(NULL, "フォルダを選択してください。", QDir::homePath()).toUtf8();
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
    //検索パスが指定されていない場合はホームフォルダとする
    QString pathStr = strlen(path) ? path : QDir::homePath();
    if(mode == FM_Save){
        result = QFileDialog::getSaveFileName(NULL, title, pathStr, filter);
    } else {
        result = QFileDialog::getOpenFileName(NULL, title, pathStr, filter);
    }
    if(result.isEmpty())    return NULL;

    QDir dir(result);
    if( path ) strcpy( path, dir.path().toUtf8().data() );
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
// フォントファイル作成
//
// 引数:	hfile		半角フォントファイルパス
//			zfile		全角フォントファイルパス
//			size		文字サイズ(半角文字幅ピクセル数)
// 返値:	bool		true:作成成功 false:作成失敗
////////////////////////////////////////////////////////////////
bool OSD_CreateFont( char *hfile, char *zfile, int size )
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

    return true;
}



///////////////////////////////////////////////////////////
// アイコン設定
//
// 引数:	wh			ウィンドウハンドル
//			model		機種 60,62,66
// 返値:	なし
///////////////////////////////////////////////////////////
void OSD_SetIcon( HWINDOW wh, int model )
{
    // 機種別P6オブジェクト確保
    const char* iconRes = NULL;
    switch( model ){
    case 62: iconRes = ":/res/PC-6001mk2.ico"; break;
    case 66: iconRes = ":/res/PC-6601.ico"; break;
    default: iconRes = ":/res/PC-6001.ico";
    }

    // アイコン設定
    QImage icon = QImage(iconRes).convertToFormat(QImage::Format_RGB16);
    //#PENDING
    //    SDL_Surface *p6icon = SDL_CreateRGBSurfaceFrom( icon.bits(), icon.width(), icon.height(), icon.depth(), icon.bytesPerLine(), 0, 0, 0, 0 );
    //    SDL_WM_SetIcon( p6icon, icon.alphaChannel().bits() );
    //    SDL_FreeSurface( p6icon );
}
