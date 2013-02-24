// OS依存の汎用ルーチン(主にUI用)

#include <QtGui>
#include <QSharedMemory>
#include <string.h>

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../pc6001v.h"
#include "../joystick.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define	AUDIOFORMAT	AUDIO_S16MSB	// 16ビット符号あり
#else
#define	AUDIOFORMAT	AUDIO_S16		// 16ビット符号あり
#endif

//#define USESDLTIMER					// SDLのタイマ使用

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
