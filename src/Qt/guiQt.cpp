// OS依存の汎用ルーチン(主にUI用)

#include <QtGui>
#include <new>

#include <SDL_syswm.h>

#include "../pc6001v.h"
#include "../breakpoint.h"
#include "../disk.h"
#include "../error.h"
#include "../graph.h"
#include "../ini.h"
#include "../memory.h"
#include "../movie.h"
#include "../osd.h"
#include "../pc60.h"
#include "../schedule.h"
#include "../tape.h"
#include "../usrevent.h"
#include "../vdg.h"

#include "configdialog.h"

///////////////////////////////////////////////////////////
// ローカル関数定義
///////////////////////////////////////////////////////////
//static HWND GetWindowHandle();			// ウィンドウハンドルを取得
static int ShowConfig();			// 環境設定プロパティシート表示

//static BOOL OsdReadINI(  HWND, int );	// 設定を読込む
//static BOOL OsdWriteINI( HWND, int );	// 設定を保存する

//static BOOL CALLBACK OsdCnfgProc1(   HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK OsdCnfgProc2(   HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK OsdCnfgProc3(   HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK OsdCnfgProc4(   HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK OsdCnfgProc5(   HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK OsdCnfgProcCol( HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK OsdCnfgProcEtc( HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK OsdCnfgProcIn(  HWND, UINT, WPARAM, LPARAM );
//static BOOL CALLBACK VerInfoProc(    HWND, UINT, WPARAM, LPARAM );

//static void OsdOwnerDrawBtn( HDC, RECT, COLORREF );	// オーナードローボタン 描画

///////////////////////////////////////////////////////////
// 仕方なしにスタティック変数
///////////////////////////////////////////////////////////
static cConfig *ccfg = NULL;				// 環境設定オブジェクトポインタ
static cConfig *ecfg = NULL;				// 環境設定オブジェクトポインタ(編集用)
static char ExtRomPathTemp[PATH_MAX] = "";	// 拡張ROMパス(Temp)
static char TapePathTemp[PATH_MAX] = "";	// TAPEパス(Temp)
static char DiskPathTemp[PATH_MAX] = "";	// DISKパス(Temp)

///////////////////////////////////////////////////////////
// メニューコマンドID
///////////////////////////////////////////////////////////
enum MenuCommand{
    ID_TAPEINSERT,	// TAPE 挿入
    ID_TAPEEJECT,	// TAPE 排出
    ID_DISKINSERT1,	// DISK1 挿入
    ID_DISKEJECT1,	// DISK1 排出
    ID_DISKINSERT2,	// DISK2 挿入
    ID_DISKEJECT2,	// DISK2 排出
    ID_ROMINSERT,	// 拡張ROM 挿入
    ID_ROMEJECT,	// 拡張ROM 排出
    ID_JOY101,		// ジョイスティック1
    ID_JOY102,
    ID_JOY103,
    ID_JOY104,
    ID_JOY105,
    ID_JOY199,
    ID_JOY201,		// ジョイスティック2
    ID_JOY202,
    ID_JOY203,
    ID_JOY204,
    ID_JOY205,
    ID_JOY299,
    ID_CONFIG,		// 環境設定
    ID_RESET,		// リセット
    ID_RESTART,		// 再起動
    ID_DOKOSAVE,	// どこでもSAVE
    ID_DOKOLOAD,	// どこでもLOAD
    ID_REPLAYSAVE,	// リプレイ保存
    ID_REPLAYLOAD,	// リプレイ再生
    ID_AVISAVE,		// ビデオキャプチャ
    ID_AUTOTYPE,	// 打込み代行
    ID_QUIT,		// 終了
    ID_NOWAIT,		// Wait有効無効変更
    ID_TURBO,		// Turbo TAPE
    ID_BOOST,		// Boost Up
    ID_SCANLINE,	// スキャンラインモード変更
    ID_DISP43,		// 4,3表示変更
    ID_STATUS,		// ステータスバー表示状態変更
    ID_M4MONO,		// モノクロ
    ID_M4RDBL,		// 赤/青
    ID_M4BLRD,		// 青/赤
    ID_M4PKGR,		// ピンク/緑
    ID_M4GRPK,		// 緑/ピンク
    ID_FSKP0,		// 0
    ID_FSKP1,		// 1
    ID_FSKP2,		// 2
    ID_FSKP3,		// 3
    ID_FSKP4,		// 4
    ID_FSKP5,		// 5
    ID_MONITOR,		// モニターモード
    ID_TIMERINT,	// タイマ割込み許可
    ID_VERSION,		// バージョン情報
};
Q_DECLARE_METATYPE(MenuCommand)

#define MENUIDPROPERTY "MenuID"

QAction* addCommand(QMenu* menu, QString label, MenuCommand id, bool checkable = false)
{
    QAction* action = menu->addAction(label);
    action->setProperty(MENUIDPROPERTY, qVariantFromValue<MenuCommand>(id));
    action->setCheckable(checkable);
    return action;
}

///////////////////////////////////////////////////////////
// ポップアップメニュー表示
///////////////////////////////////////////////////////////
void VM6::ShowPopupMenu( int x, int y )
{
    QAction* selectedAction = NULL;

    QMenu menu;

    // システムメニュー
    QMenu* systemMenu = menu.addMenu("システム");
    addCommand(systemMenu, "リセット", ID_RESET);
    addCommand(systemMenu, "再起動", ID_RESTART);

    // どこでもLOAD,SAVEメニュー
    QMenu* dokoMenu = systemMenu->addMenu("どこでも");
    addCommand(dokoMenu, "LOAD...", ID_DOKOLOAD);
    addCommand(dokoMenu, "SAVE...", ID_DOKOSAVE);

    // リプレイメニュー
    QMenu* replayMenu = systemMenu->addMenu("リプレイ");
    QAction* repleyLoad = addCommand(replayMenu, (REPLAY::GetStatus() == REP_REPLAY) ? MSMEN_REP3: MSMEN_REP2, ID_REPLAYLOAD);
    // モニタモード or ブレークポインタが設定されている
    // またはリプレイ記録中だったらリプレイ再生無効
    if( cfg->GetMonDisp() || bp->ExistBreakPoint() || ( REPLAY::GetStatus() == REP_RECORD ) )
        repleyLoad->setEnabled(false);

    QAction* repleySave = addCommand(replayMenu, (REPLAY::GetStatus() == REP_RECORD) ? MSMEN_REP1 : MSMEN_REP0, ID_REPLAYSAVE);
    // モニタモード or ブレークポインタが設定されている
    // またはリプレイ再生中だったらリプレイ記録無効
    if( cfg->GetMonDisp() || bp->ExistBreakPoint() || ( REPLAY::GetStatus() == REP_REPLAY ) )
        repleySave->setEnabled(false);

    // ビデオキャプチャ
    addCommand(systemMenu, AVI6::IsAVI() ? MSMEN_AVI1 : MSMEN_AVI0, ID_AVISAVE);

    addCommand(systemMenu, "打込み代行...", ID_AUTOTYPE);
    menu.addSeparator();

    // TAPEメニュー
    QMenu* tapeMenu = menu.addMenu("TAPE");
    addCommand(tapeMenu, "挿入...", ID_TAPEINSERT);
    QAction* tapeEject = addCommand(tapeMenu, "取出", ID_TAPEEJECT);
    if(!*cmt->GetFile()) tapeEject->setEnabled(false);

    // DISKメニュー
    if (disk->GetDrives()){
        QMenu* diskMenu = menu.addMenu("DISK");
        for (int i = 0; i < disk->GetDrives(); i++){
            QString item = QString("Drive%1").arg(i + 1);
            QMenu* driveMenu = diskMenu->addMenu(item);
            addCommand(driveMenu, "挿入...", MenuCommand(ID_DISKINSERT1 + i));
            QAction* diskEject = addCommand(driveMenu, "取出", MenuCommand(ID_DISKEJECT1 + i));
            if (!*disk->GetFile(i)) diskEject->setEnabled(false);
        }
    }

    // 拡張ROMメニュー
    QMenu* extRomMenu = menu.addMenu("拡張ROM");
    addCommand(extRomMenu, "挿入...", ID_ROMINSERT);
    QAction* romEject = addCommand(extRomMenu, "取出", ID_ROMEJECT);
    if(!*mem->GetFile()) romEject->setEnabled(false);

    // ジョイスティックメニュー
    QMenu* joystickMenu = menu.addMenu("ジョイスティック");
    //------
    QMenu* joyMenu1 = joystickMenu->addMenu("1");
    QMenu* joyMenu2 = joystickMenu->addMenu("2");
    QActionGroup* joyGroup1 = new QActionGroup(&menu);
    QActionGroup* joyGroup2 = new QActionGroup(&menu);
    for( int i = 0; i < 5; i++ ){
        if( i < OSD_GetJoyNum() ){
            QAction* joyAction1 = addCommand(joyMenu1, OSD_GetJoyName( i ), MenuCommand(ID_JOY101 + i), true);
            QAction* joyAction2 = addCommand(joyMenu2, OSD_GetJoyName( i ), MenuCommand(ID_JOY201 + i), true);
            joyGroup1->addAction(joyAction1);
            joyGroup2->addAction(joyAction2);
            if(joy->GetID(0) == i) joyAction1->setChecked(true);
            if(joy->GetID(1) == i) joyAction2->setChecked(true);
        }
    }
    QAction* noJoy1 = addCommand(joyMenu1, "なし", ID_JOY199, true);
    QAction* noJoy2 = addCommand(joyMenu2, "なし", ID_JOY299, true);
    joyGroup1->addAction(noJoy1);
    joyGroup2->addAction(noJoy2);
    if (joy->GetID(0) < 0) noJoy1->setChecked(true);
    if (joy->GetID(1) < 0) noJoy2->setChecked(true);

    // 設定メニュー
    QMenu* settingsMenu = menu.addMenu("設定");
    QAction* statusBar = addCommand(settingsMenu, "ステータスバー", ID_STATUS, true);
    if (cfg->GetStatDisp()) statusBar->setChecked(true);
    QAction* disp43 = addCommand(settingsMenu, "4:3表示", ID_DISP43, true);
    if (cfg->GetDispNTSC()) disp43->setChecked(true);
    QAction* scanLine = addCommand(settingsMenu, "スキャンライン", ID_SCANLINE, true);
    if (cfg->GetScanLine()) scanLine->setChecked(true);

    QMenu* colorMenu = settingsMenu->addMenu("MODE4 カラー");
    QActionGroup* colorGroup = new QActionGroup(&menu);
    QStringList colorList = (QStringList()
                           << "モノクロ"
                           << "赤/青"
                           << "青/赤"
                           << "桃/緑"
                           << "緑/桃");
    for( int i = 0; i < 5; i++ ){
        QAction* color = addCommand(colorMenu, colorList[i], MenuCommand(ID_M4MONO + i), true);
        if (vdg->GetMode4Color() == i) color->setChecked(true);
    }

    QMenu* fpsMenu = settingsMenu->addMenu("フレームスキップ");
    QActionGroup* fpsGroup = new QActionGroup(&menu);
    QStringList fpsList = (QStringList()
                           << "0 (60fps)"
                           << "1 (30fps)"
                           << "2 (20fps)"
                           << "3 (15fps)"
                           << "4 (12fps)"
                           << "5 (10fps)");
    for( int i = 0; i < 6; i++ ){
        QAction* fps = addCommand(fpsMenu, fpsList[i], MenuCommand(ID_FSKP0 + i), true);
        if (cfg->GetFrameSkip() == i) fps->setChecked(true);
    }
    settingsMenu->addSeparator();

    QAction* noWait = addCommand(settingsMenu, "ウェイト無効", ID_NOWAIT, true);
    if (!sche->GetWaitEnable()) noWait->setChecked(true);
    QAction* turboTape = addCommand(settingsMenu, "Turbo TAPE", ID_TURBO, true);
    if (cfg->GetTurboTAPE()) turboTape->setChecked(true);
    QAction* boostUp =  addCommand(settingsMenu, "Boost Up", ID_BOOST, true);
    if (cmt->IsBoostUp()) boostUp->setChecked(true);
    addCommand(settingsMenu, "環境設定...", ID_CONFIG);

    // デバッグメニュー
    QMenu* debugMenu = menu.addMenu("デバッグ");
    QAction* monitorMode =  addCommand(debugMenu, "モニタモード", ID_MONITOR, true);
    if (cfg->GetMonDisp()) monitorMode->setChecked(true);
    QAction* timerInt = addCommand(debugMenu, "タイマ割込み禁止", ID_TIMERINT, true);
    if (!cfg->GetTimerIntr()) timerInt->setChecked(true);
    menu.addSeparator();

    // ヘルプメニュー
    QMenu* helpMenu = menu.addMenu("ヘルプ");
    addCommand(helpMenu, "バージョン情報...", ID_VERSION);

    selectedAction = menu.exec(QCursor::pos());

    if (selectedAction == NULL) return;


    // 項目ごとの処理
    char str[PATH_MAX];
    MenuCommand id = selectedAction->property(MENUIDPROPERTY).value<MenuCommand>();
    switch( id ){
    case ID_TAPEINSERT:		// TAPE 挿入
        if( !OSD_FileExist( TapePathTemp ) )
            strncpy( TapePathTemp, cfg->GetTapePath(), PATH_MAX );
        if( OSD_FileSelect( NULL, FD_TapeLoad, str, TapePathTemp ) )
            if( !cmt->Mount( str ) ) Error::SetError( Error::TapeMountFailed );
        break;

    case ID_TAPEEJECT:		// TAPE 排出
        cmt->Unmount();
        break;

    case ID_DISKINSERT1:	// DISK1 挿入
        if( !OSD_FileExist( DiskPathTemp ) )
            strncpy( DiskPathTemp, cfg->GetDiskPath(), PATH_MAX );
        if( OSD_FileSelect( NULL, FD_Disk, str, DiskPathTemp ) )
            if( !disk->Mount( 0, str ) ) Error::SetError( Error::DiskMountFailed );
        break;

    case ID_DISKEJECT1:		// DISK1 排出
        disk->Unmount( 0 );
        break;

    case ID_DISKINSERT2:	// DISK2 挿入
        if( !OSD_FileExist( DiskPathTemp ) )
            strncpy( DiskPathTemp, cfg->GetDiskPath(), PATH_MAX );
        if( OSD_FileSelect( NULL, FD_Disk, str, DiskPathTemp ) )
            if( !disk->Mount( 1, str ) ) Error::SetError( Error::DiskMountFailed );
        break;

    case ID_DISKEJECT2:		// DISK2 排出
        disk->Unmount( 1 );
        break;

    case ID_ROMINSERT:		// 拡張ROM 挿入
        if( !OSD_FileExist( ExtRomPathTemp ) )
            strncpy( ExtRomPathTemp, cfg->GetExtRomPath(), PATH_MAX );
        if( OSD_FileSelect( NULL, FD_ExtRom, str, ExtRomPathTemp ) ){
            // リセットを伴うのでメッセージ表示
            OSD_Message( MSG_RESETI, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
            if( !mem->MountExtRom( str ) )
                Error::SetError( Error::ExtRomMountFailed );
            else
                Reset();
        }
        break;

    case ID_ROMEJECT:		// 拡張ROM 排出
        // リセットを伴うのでメッセージ表示
        OSD_Message( MSG_RESETE, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
        mem->UnmountExtRom();
        Reset();
        break;

    case ID_JOY101:			// ジョイスティック1
    case ID_JOY102:
    case ID_JOY103:
    case ID_JOY104:
    case ID_JOY105:
        joy->Connect( 0, id - ID_JOY101 );
        break;

    case ID_JOY199:
        joy->Connect( 0, -1 );
        break;

    case ID_JOY201:			// ジョイスティック2
    case ID_JOY202:
    case ID_JOY203:
    case ID_JOY204:
    case ID_JOY205:
        joy->Connect( 1, id - ID_JOY201 );
        break;

    case ID_JOY299:
        joy->Connect( 1, -1 );
        break;

    case ID_CONFIG:			// 環境設定
        if( ShowConfig() > 0 )
            // 再起動?
            if( OSD_Message( MSG_RESTART, MSG_RESTARTC, OSDM_YESNO | OSDM_ICONQUESTION ) == OSDR_YES )
                SENDEVENT( SDL_RESTART )
                        break;

    case ID_RESET:			// リセット
        Reset();
        break;

    case ID_RESTART:		// 再起動
        SENDEVENT( SDL_RESTART )
                break;

    case ID_DOKOSAVE:		// どこでもSAVE
        if( OSD_FileSelect( NULL, FD_DokoSave, str, (char *)OSD_GetConfigPath() ) )
            DokoDemoSave( str );
        break;

    case ID_DOKOLOAD:		// どこでもLOAD
        if( OSD_FileSelect( NULL, FD_DokoLoad, str, (char *)OSD_GetConfigPath() ) )
            DokoDemoLoad( str );
        break;

    case ID_REPLAYSAVE:		// リプレイ保存
        if( REPLAY::GetStatus() == REP_IDLE ){
            if( OSD_FileSelect( NULL, FD_RepSave, str, (char *)OSD_GetConfigPath() ) ){
                if( DokoDemoSave( str ) ) REPLAY::StartRecord( str );
            }
        }else if( REPLAY::GetStatus() == REP_RECORD ){
            REPLAY::StopRecord();
        }
        break;

    case ID_REPLAYLOAD:		// リプレイ再生
        if( REPLAY::GetStatus() == REP_IDLE ){
            if( OSD_FileSelect( NULL, FD_RepLoad, str, (char *)OSD_GetConfigPath() ) ){
                if( DokoDemoLoad( str ) )
                    REPLAY::StartReplay( str );
                else
                    if( Error::GetError() == Error::DokoDiffModel ) Error::SetError( Error::ReplayDiffModel );
            }
        }else if( REPLAY::GetStatus() == REP_REPLAY ){
            REPLAY::StopReplay();
        }
        break;

    case ID_AVISAVE:		// ビデオキャプチャ
        if( !AVI6::IsAVI() ){
            if( OSD_FileSelect( NULL, FD_AVISave, str, (char *)OSD_GetConfigPath() ) ){
                AVI6::StartAVI( str, graph->GetSubBuffer(), FRAMERATE, cfg->GetSampleRate(), cfg->GetAviRle() );
            }
        }else{
            AVI6::StopAVI();
        }
        break;

    case ID_AUTOTYPE:		// 打込み代行
        if( OSD_FileSelect( NULL, FD_LoadAll, str, (char *)OSD_GetConfigPath() ) )
            if( !SetAutoKeyFile( str ) ) Error::SetError( Error::Unknown );
        break;

    case ID_QUIT:			// 終了
        SENDEVENT( SDL_QUIT )
                break;

    case ID_NOWAIT:			// Wait有効無効変更
        sche->SetWaitEnable( sche->GetWaitEnable() ? FALSE : TRUE );
        break;

    case ID_TURBO:			// Turbo TAPE
        cfg->SetTurboTAPE( cfg->GetTurboTAPE() ? FALSE : TRUE );
        break;

    case ID_BOOST:			// Boost Up
        cfg->SetBoostUp( cfg->GetBoostUp() ? FALSE : TRUE );
        cmt->SetBoost( cmt->IsBoostUp() ? FALSE : TRUE );
        break;

    case ID_SCANLINE:		// スキャンラインモード変更
        // ビデオキャプチャ中は無効
        if( !AVI6::IsAVI() ){
            cfg->SetScanLine( cfg->GetScanLine() ? FALSE : TRUE );
            graph->ResizeScreen();	// スクリーンサイズ変更
        }
        break;

    case ID_DISP43:			// 4:3表示変更
        // ビデオキャプチャ中は無効
        if( !AVI6::IsAVI() ){
            cfg->SetDispNTSC( cfg->GetDispNTSC() ? FALSE : TRUE );
            graph->ResizeScreen();	// スクリーンサイズ変更
        }
        break;

    case ID_STATUS:			// ステータスバー表示状態変更
        cfg->SetStatDisp( cfg->GetStatDisp() ? FALSE : TRUE );
        graph->ResizeScreen();	// スクリーンサイズ変更
        break;

        // MODE4カラー
    case ID_M4MONO:		// モノクロ
        cfg->SetMode4Color( 0 );
        vdg->SetMode4Color( 0 );
        break;
    case ID_M4RDBL:		// 赤/青
        cfg->SetMode4Color( 1 );
        vdg->SetMode4Color( 1 );
        break;
    case ID_M4BLRD:		// 青/赤
        cfg->SetMode4Color( 2 );
        vdg->SetMode4Color( 2 );
        break;
    case ID_M4PKGR:		// ピンク/緑
        cfg->SetMode4Color( 3 );
        vdg->SetMode4Color( 3 );
        break;
    case ID_M4GRPK:		// 緑/ピンク
        cfg->SetMode4Color( 4 );
        vdg->SetMode4Color( 4 );
        break;

        // フレームスキップ
        // ビデオキャプチャ中は無効
    case ID_FSKP0:		// 0
        if( !AVI6::IsAVI() ) cfg->SetFrameSkip( 0 );
        break;
    case ID_FSKP1:		// 1
        if( !AVI6::IsAVI() ) cfg->SetFrameSkip( 1 );
        break;
    case ID_FSKP2:		// 2
        if( !AVI6::IsAVI() ) cfg->SetFrameSkip( 2 );
        break;
    case ID_FSKP3:		// 3
        if( !AVI6::IsAVI() ) cfg->SetFrameSkip( 3 );
        break;
    case ID_FSKP4:		// 4
        if( !AVI6::IsAVI() ) cfg->SetFrameSkip( 4 );
        break;
    case ID_FSKP5:		// 5
        if( !AVI6::IsAVI() ) cfg->SetFrameSkip( 5 );
        break;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
    case ID_MONITOR:		// モニターモード
        ToggleMonitor();
        break;

    case ID_TIMERINT:		// タイマ割込み許可
        cfg->SetTimerIntr( cfg->GetTimerIntr() ? FALSE : TRUE );
        break;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

    case ID_VERSION:		// バージョン情報
        //#PENDING DialogBox( (HINSTANCE)GetWindowLong( NULL, GWL_HINSTANCE), MAKEINTRESOURCE(ID_VER), hwnd, (DLGPROC)VerInfoProc );
        break;
    }

    // ダイアログの後始末など、キューに溜まっているイベントを処理する
    while(QApplication::hasPendingEvents()){
        QApplication::processEvents();
    }
}


///////////////////////////////////////////////////////////
// ウィンドウハンドルを取得
///////////////////////////////////////////////////////////
//static HWND GetWindowHandle( void )
//{
//	SDL_SysWMinfo WinInfo;
	
//	SDL_VERSION( &WinInfo.version );
//  	if( SDL_GetWMInfo( &WinInfo ) ) return WinInfo.window;
//	else                            return NULL;
//}


///////////////////////////////////////////////////////////
// 環境設定プロパティシート表示
///////////////////////////////////////////////////////////
static int ShowConfig()
{
    // INIファイルを開く
    try{
        ecfg = new cConfig();
        if( !ecfg->Init() ) throw Error::IniReadFailed;
    }
    // new に失敗した場合
    catch( std::bad_alloc ){
        return -1;
    }
    // 例外発生
    catch( Error::Errno i ){
        delete ecfg;
        ecfg = NULL;
        return -1;
    }

    //#PENDING
    ConfigDialog dialog(ecfg);
    dialog.exec();
    int ret = dialog.result();
    // OKボタンが押されたならINIファイル書込み
    if( ret == QDialog::Accepted) ecfg->Write();

    delete ecfg;
    ecfg = NULL;

    return ret;

//	// ページ毎の設定を行なう
//	PROPSHEETPAGE psp[9];
//	PROPSHEETHEADER psh;
	
//	// 基本
//	psp[0].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[0].dwFlags     = PSP_DEFAULT;
//	psp[0].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[0].pszTemplate = MAKEINTRESOURCE(ID_CNFG1);
//	psp[0].pszIcon     = NULL;
//	psp[0].pfnDlgProc  = (DLGPROC)OsdCnfgProc1;
//	psp[0].pszTitle    = NULL;
//	psp[0].lParam      = 0;
	
//	// 画面
//	psp[1].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[1].dwFlags     = PSP_DEFAULT;
//	psp[1].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[1].pszTemplate = MAKEINTRESOURCE(ID_CNFG2);
//	psp[1].pszIcon     = NULL;
//	psp[1].pfnDlgProc  = (DLGPROC)OsdCnfgProc2;
//	psp[1].pszTitle    = NULL;
//	psp[1].lParam      = 0;
	
//	// サウンド
//	psp[2].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[2].dwFlags     = PSP_DEFAULT;
//	psp[2].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[2].pszTemplate = MAKEINTRESOURCE(ID_CNFG3);
//	psp[2].pszIcon     = NULL;
//	psp[2].pfnDlgProc  = (DLGPROC)OsdCnfgProc3;
//	psp[2].pszTitle    = NULL;
//	psp[2].lParam      = 0;
	
//	// 入力関係
//	psp[3].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[3].dwFlags     = PSP_DEFAULT;
//	psp[3].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[3].pszTemplate = MAKEINTRESOURCE(ID_CNFGIN);
//	psp[3].pszIcon     = NULL;
//	psp[3].pfnDlgProc  = (DLGPROC)OsdCnfgProcIn;
//	psp[3].pszTitle    = NULL;
//	psp[3].lParam      = 0;
	
//	// ファイル
//	psp[4].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[4].dwFlags     = PSP_DEFAULT;
//	psp[4].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[4].pszTemplate = MAKEINTRESOURCE(ID_CNFG4);
//	psp[4].pszIcon     = NULL;
//	psp[4].pfnDlgProc  = (DLGPROC)OsdCnfgProc4;
//	psp[4].pszTitle    = NULL;
//	psp[4].lParam      = 0;
	
//	// フォルダ
//	psp[5].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[5].dwFlags     = PSP_DEFAULT;
//	psp[5].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[5].pszTemplate = MAKEINTRESOURCE(ID_CNFG5);
//	psp[5].pszIcon     = NULL;
//	psp[5].pfnDlgProc  = (DLGPROC)OsdCnfgProc5;
//	psp[5].pszTitle    = NULL;
//	psp[5].lParam      = 0;
	
//	// 色1
//	psp[6].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[6].dwFlags     = PSP_DEFAULT;
//	psp[6].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[6].pszTemplate = MAKEINTRESOURCE(ID_CNFGCL1);
//	psp[6].pszIcon     = NULL;
//	psp[6].pfnDlgProc  = (DLGPROC)OsdCnfgProcCol;
//	psp[6].pszTitle    = NULL;
//	psp[6].lParam      = 0;
	
//	// 色2
//	psp[7].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[7].dwFlags     = PSP_DEFAULT;
//	psp[7].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[7].pszTemplate = MAKEINTRESOURCE(ID_CNFGCL2);
//	psp[7].pszIcon     = NULL;
//	psp[7].pfnDlgProc  = (DLGPROC)OsdCnfgProcCol;
//	psp[7].pszTitle    = NULL;
//	psp[7].lParam      = 0;
	
//	// その他
//	psp[8].dwSize      = sizeof(PROPSHEETPAGE);
//	psp[8].dwFlags     = PSP_DEFAULT;
//	psp[8].hInstance   = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psp[8].pszTemplate = MAKEINTRESOURCE(ID_CNFGETC);
//	psp[8].pszIcon     = NULL;
//	psp[8].pfnDlgProc  = (DLGPROC)OsdCnfgProcEtc;
//	psp[8].pszTitle    = NULL;
//	psp[8].lParam      = 0;
	
	
//	psh.dwSize     = sizeof(PROPSHEETHEADER);
//	psh.dwFlags    = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
//	psh.hwndParent = hwnd;
//	psh.hInstance  = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE);
//	psh.pszIcon    = NULL;
//	psh.pszCaption = (LPSTR) "環境設定";
//	psh.nPages     = sizeof(psp) / sizeof(PROPSHEETPAGE);
//	psh.nStartPage = 0;
//	psh.ppsp       = (LPCPROPSHEETPAGE) &psp;
	
//	// プロパティシート表示
//	int ret = PropertySheet( &psh );
//	// ret 1:OK 0:CANCEL
	
//	// OKボタンが押されたならINIファイル書込み
//	if( ret > 0) ecfg->Write();
	
//	delete ecfg;
//	ecfg = NULL;
	
//	return ret;
}


enum ConfPage{ PP_BASE, PP_DISP, PP_SOUND, PP_FILE, PP_FOLDER, PP_COL, PP_ETC, PP_INPUT };
Q_DECLARE_METATYPE(ConfPage)

///////////////////////////////////////////////////////////
// 設定を読込む
///////////////////////////////////////////////////////////
//static BOOL OsdReadINI( HWND hwnd, int page )
//{
//	int st;					// 状態取得用
//	BOOL yn;				// 状態取得用
//	char str[PATH_MAX];		// 文字列取得用
	
//	if( !ecfg ) return FALSE;
	
//	switch( page ){
//	case PP_BASE:	// 基本
//		// 機種
//		st = ecfg->GetModel();
//		SendMessage( GetDlgItem( hwnd, ID_RB01 ), BM_SETCHECK, (st==60)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB02 ), BM_SETCHECK, (st==62)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB03 ), BM_SETCHECK, (st==66)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB04 ), BM_SETCHECK, (st==64)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB05 ), BM_SETCHECK, (st==68)? BST_CHECKED:BST_UNCHECKED, 0 );
		
//		// FDD
//		st = ecfg->GetFddNum();
//		SendMessage( GetDlgItem( hwnd, ID_RB31 ), BM_SETCHECK, (st==0)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB32 ), BM_SETCHECK, (st==1)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB33 ), BM_SETCHECK, (st==2)? BST_CHECKED:BST_UNCHECKED, 0 );
		
//		// 拡張RAM使用
//		yn = ecfg->GetUseExtRam();
//		SendMessage( GetDlgItem( hwnd, ID_CB1 ), BM_SETCHECK, yn, 0 );
		
//		// 戦士のカートリッジ使用
//		yn = ecfg->GetUseSoldier();
//		SendMessage( GetDlgItem( hwnd, ID_CB13 ), BM_SETCHECK, yn, 0 );
		
//		break;
		
//	case PP_DISP:	// 画面
//		// カラーモード
//		st = ecfg->GetScrBpp();
//		SendMessage( GetDlgItem( hwnd, ID_RB34 ), BM_SETCHECK, (st== 8)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB35 ), BM_SETCHECK, (st==16)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB36 ), BM_SETCHECK, (st==24)? BST_CHECKED:BST_UNCHECKED, 0 );
		
//		// MODE4カラー
//		st = ecfg->GetMode4Color();
//		SendMessage( GetDlgItem( hwnd, ID_RB11 ), BM_SETCHECK, (st==0)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB12 ), BM_SETCHECK, (st==1)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB13 ), BM_SETCHECK, (st==2)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB14 ), BM_SETCHECK, (st==3)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB15 ), BM_SETCHECK, (st==4)? BST_CHECKED:BST_UNCHECKED, 0 );
		
//		// スキャンライン
//		yn = ecfg->GetScanLine();
//		SendMessage( GetDlgItem( hwnd, ID_CB2 ), BM_SETCHECK, yn, 0 );
		
//		// スキャンライン輝度
//		st = ecfg->GetScanLineBr();
//		sprintf( str, "%d", st );
//		SetDlgItemText( hwnd, ID_SCANLINEBR, str );
		
//		// 4:3表示
//		yn = ecfg->GetDispNTSC();
//		SendMessage( GetDlgItem( hwnd, ID_CB7 ), BM_SETCHECK, yn, 0 );
		
//		// フルスクリーン
//		yn = ecfg->GetFullScreen();
//		SendMessage( GetDlgItem( hwnd, ID_CB10 ), BM_SETCHECK, yn, 0 );
		
//		// ステータスバー表示状態
//		yn = ecfg->GetStatDisp();
//		SendMessage( GetDlgItem( hwnd, ID_CB11 ), BM_SETCHECK, yn, 0 );
		
//		// フレームスキップ
//		st = ecfg->GetFrameSkip();
//		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETPOS, TRUE, st );
//		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETTHUMBLENGTH, 12, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETRANGE, TRUE, MAKELPARAM(0,5) );
		
//		break;
		
//	case PP_SOUND:	// サウンド
//		// サンプリングレート
//		st = ecfg->GetSampleRate();
//		SendMessage( GetDlgItem( hwnd, ID_RB21 ), BM_SETCHECK, (st==44100)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB22 ), BM_SETCHECK, (st==22050)? BST_CHECKED:BST_UNCHECKED, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_RB23 ), BM_SETCHECK, (st==11025)? BST_CHECKED:BST_UNCHECKED, 0 );
		
//		// バッファサイズ
//		st = ecfg->GetSoundBuffer();
//		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETPOS, TRUE, st );
//		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETTHUMBLENGTH, 12, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETRANGE, TRUE, MAKELPARAM(1,5) );
		
//		// PSG LPFカットオフ周波数
//		st = ecfg->GetPsgLPF();
//		sprintf( str, "%d", st );
//		SetDlgItemText( hwnd, ID_EDIT1, str );
		
//		// マスター音量
//		st = ecfg->GetMasterVol();
//		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETPOS, TRUE, st/10 );
//		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETTHUMBLENGTH, 12, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETRANGE, TRUE, MAKELPARAM(0,10) );
		
//		// PSG音量
//		st = ecfg->GetPsgVol();
//		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETPOS, TRUE, st/10 );
//		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETTHUMBLENGTH, 12, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETRANGE, TRUE, MAKELPARAM(0,10) );
		
//		// 音声合成音量
//		st = ecfg->GetVoiceVol();
//		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETPOS, TRUE, st/10 );
//		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETTHUMBLENGTH, 12, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETRANGE, TRUE, MAKELPARAM(0,10) );
		
//		// TAPEモニタ音量
//		st = ecfg->GetCmtVol();
//		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETPOS, TRUE, st/10 );
//		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETTHUMBLENGTH, 12, 0 );
//		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETRANGE, TRUE, MAKELPARAM(0,10) );
		
//		break;
		
//	case PP_INPUT:	// 入力関係
//		// キーリピート間隔
//		st = ecfg->GetKeyRepeat();
//		sprintf( str, "%d", st );
//		SetDlgItemText( hwnd, ID_KEYREP, str );
		
//		break;
		
//	case PP_FOLDER:	// フォルダ
//		// ROMパス
//		strncpy( str, ecfg->GetRomPath(), PATH_MAX );
//		DelDelimiter( str );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_PATH1, str );
		
//		// TAPEパス
//		strncpy( str, ecfg->GetTapePath(), PATH_MAX );
//		DelDelimiter( str );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_PATH2, str );
		
//		// DISKパス
//		strncpy( str, ecfg->GetDiskPath(), PATH_MAX );
//		DelDelimiter( str );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_PATH3, str );
		
//		// 拡張ROMパス
//		strncpy( str, ecfg->GetExtRomPath(), PATH_MAX );
//		DelDelimiter( str );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_PATH4, str );
		
//		// IMGパス
//		strncpy( str, ecfg->GetImgPath(), PATH_MAX );
//		DelDelimiter( str );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_PATH5, str );
		
//		// WAVEパス
//		strncpy( str, ecfg->GetWavePath(), PATH_MAX );
//		DelDelimiter( str );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_PATH6, str );
		
//		// FONTパス
//		strncpy( str, ecfg->GetFontPath(), PATH_MAX );
//		DelDelimiter( str );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_PATH7, str );
		
//		break;
		
//	case PP_FILE:	// ファイル
//		// 拡張ROMファイル
//		strncpy( str, ecfg->GetExtRomFile(), PATH_MAX );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_FEXROM, str );
		
//		// TAPE(LOAD)ファイル名
//		strncpy( str, ecfg->GetTapeFile(), PATH_MAX );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_FTPLD, str );
		
//		// TAPE(SAVE)ファイル名
//		strncpy( str, ecfg->GetSaveFile(), PATH_MAX );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_FTPSV, str );
		
//		// DISKファイル名
//		strncpy( str, ecfg->GetDiskFile(), PATH_MAX );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_FDISK, str );
		
//		// プリンタファイル名
//		strncpy( str, ecfg->GetPrinterFile(), PATH_MAX );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_FPRINT, str );
		
//		// 全角フォントファイル名
//		strncpy( str, ecfg->GetFontFileZ(), PATH_MAX );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_FFONTZ, str );
		
//		// 半角フォントファイル名
//		strncpy( str, ecfg->GetFontFileH(), PATH_MAX );
//		UnDelimiter( str );
//		SetDlgItemText( hwnd, ID_FFONTH, str );
		
//		break;
		
//	case PP_COL:	// 色1
//		break;
		
//	case PP_ETC:	// その他
//		// オーバークロック率
//		st = min( max( 1, ecfg->GetOverClock() ), 1000 );
//		sprintf( str, "%d", st );
//		SetDlgItemText( hwnd, ID_OVERCLK, str );
		
//		// CRCチェック
//		yn = ecfg->GetCheckCRC();
//		SendMessage( GetDlgItem( hwnd, ID_CB4 ), BM_SETCHECK, yn, 0 );
		
//		// ROMパッチ
//		yn = ecfg->GetRomPatch();
//		SendMessage( GetDlgItem( hwnd, ID_CB9 ), BM_SETCHECK, yn, 0 );
		
//		// Turbo TAPE
//		yn = ecfg->GetTurboTAPE();
//		SendMessage( GetDlgItem( hwnd, ID_CB3 ), BM_SETCHECK, yn, 0 );
		
//		// Boost Up
//		yn = ecfg->GetBoostUp();
//		SendMessage( GetDlgItem( hwnd, ID_CB5 ), BM_SETCHECK, yn, 0 );
		
//		// BoostUp 最大倍率(N60モード)
//		st = min( max( 1, ecfg->GetMaxBoost1() ), 100 );
//		sprintf( str, "%d", st );
//		SetDlgItemText( hwnd, ID_BOOST60, str );
		
//		// BoostUp 最大倍率(N60m/N66モード)
//		st = min( max( 1, ecfg->GetMaxBoost2() ), 100 );
//		sprintf( str, "%d", st );
//		SetDlgItemText( hwnd, ID_BOOST62, str );
		
//		// ビデオキャプチャ RLE
//		yn = ecfg->GetAviRle();
//		SendMessage( GetDlgItem( hwnd, ID_CB6 ), BM_SETCHECK, yn, 0 );
		
//		// 終了時 確認する
//		yn = ecfg->GetCkQuit();
//		SendMessage( GetDlgItem( hwnd, ID_CB8 ), BM_SETCHECK, yn, 0 );
		
//		// 終了時 INIファイルを保存する
//		yn = ecfg->GetSaveQuit();
//		SendMessage( GetDlgItem( hwnd, ID_CB12 ), BM_SETCHECK, yn, 0 );
		
//		break;
//	}
	
//	return TRUE;
//}


///////////////////////////////////////////////////////////
// 設定を保存する
///////////////////////////////////////////////////////////
//static BOOL OsdWriteINI( HWND hwnd, int page )
//{
//	int st;					// 状態取得用
//	char str[PATH_MAX];		// 文字列取得用
	
//	if( !ecfg ) return FALSE;
	
//	switch( page ){
//	case PP_BASE:	// 基本
//		// 機種
//		if     ( IsDlgButtonChecked( hwnd, ID_RB02 ) ) st = 62;	// PC-6001mk2
//		else if( IsDlgButtonChecked( hwnd, ID_RB03 ) ) st = 66;	// PC-6601
//		else if( IsDlgButtonChecked( hwnd, ID_RB04 ) ) st = 64;	// PC-6001mk2SR
//		else if( IsDlgButtonChecked( hwnd, ID_RB05 ) ) st = 68;	// PC-6601SR
//		else                                           st = 60;	// 上記以外ならPC-6001
//		ecfg->SetModel( st );
		
//		// FDD
//		if     ( IsDlgButtonChecked( hwnd, ID_RB32 ) ) st = 1;		// 2台
//		else if( IsDlgButtonChecked( hwnd, ID_RB33 ) ) st = 2;		// 1台
//		else                                           st = 0;		// 上記以外ならなし
//		ecfg->SetFddNum( st );
		
//		// 拡張RAM使用
//		ecfg->SetUseExtRam( IsDlgButtonChecked( hwnd, ID_CB1 ) );
		
//		// 戦士のカートリッジ使用
//		ecfg->SetUseSoldier( IsDlgButtonChecked( hwnd, ID_CB13 ) );
		
//		break;
		
//	case PP_DISP:	// 画面
//		// カラーモード
//		if     ( IsDlgButtonChecked( hwnd, ID_RB36 ) ) st = 24;	// 24bit
//		else if( IsDlgButtonChecked( hwnd, ID_RB35 ) ) st = 16;	// 16bit
//		else                                           st =  8;	// 上記以外なら8bit
//		ecfg->SetScrBpp( st );
		
//		// MODE4カラー
//		if     ( IsDlgButtonChecked( hwnd, ID_RB12 ) ) st = 1;	// 赤/青
//		else if( IsDlgButtonChecked( hwnd, ID_RB13 ) ) st = 2;	// 青/赤
//		else if( IsDlgButtonChecked( hwnd, ID_RB14 ) ) st = 3;	// ピンク/緑
//		else if( IsDlgButtonChecked( hwnd, ID_RB15 ) ) st = 4;	// 緑/ピンク
//		else                                           st = 0;	// 上記以外ならモノクロ
//		ecfg->SetMode4Color( st );
		
//		// スキャンライン
//		ecfg->SetScanLine( IsDlgButtonChecked( hwnd, ID_CB2 ) );
		
//		// スキャンライン輝度
//		GetDlgItemText( hwnd, ID_SCANLINEBR, str, sizeof(str) );
//		st = strtol( str, NULL, 0 );
//		ecfg->SetScanLineBr( st );
		
//		// 4:3表示
//		ecfg->SetDispNTSC( IsDlgButtonChecked( hwnd, ID_CB7 ) );
		
//		// フルスクリーン
//		ecfg->SetFullScreen( IsDlgButtonChecked( hwnd, ID_CB10 ) );
		
//		// ステータスバー表示状態
//		ecfg->SetStatDisp( IsDlgButtonChecked( hwnd, ID_CB11 ) );
		
//		// フレームスキップ
//		st = SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_GETPOS, 0, 0 );
//		ecfg->SetFrameSkip( st );
		
//		break;
		
//	case PP_SOUND:	// サウンド
//		// サンプリングレート
//		if     ( IsDlgButtonChecked( hwnd, ID_RB22 ) ) st = 22050;	// 22050
//		else if( IsDlgButtonChecked( hwnd, ID_RB23 ) ) st = 11025;	// 11025
//		else                                           st = 44100;	// 上記以外なら44100
//		ecfg->SetSampleRate( st );
		
//		// バッファサイズ
//		st = SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_GETPOS, 0, 0 );
//		ecfg->SetSoundBuffer( st );
		
//		// PSG LPFカットオフ周波数
//		GetDlgItemText( hwnd, ID_EDIT1, str, sizeof(str) );
//		st = strtol( str, NULL, 0 );
//		ecfg->SetPsgLPF( st );
		
//		// マスター音量
//		st = SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_GETPOS, 0, 0 );
//		ecfg->SetMasterVol( st*10 );
		
//		// PSG音量
//		st = SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_GETPOS, 0, 0 );
//		ecfg->SetPsgVol( st*10 );
		
//		// 音声合成音量
//		st = SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_GETPOS, 0, 0 );
//		ecfg->SetVoiceVol( st*10 );
		
//		// TAPEモニタ音量
//		st = SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_GETPOS, 0, 0 );
//		ecfg->SetCmtVol( st*10 );
		
//		break;
		
//	case PP_INPUT:	// 入力関係
//		// キーリピート間隔
//		GetDlgItemText( hwnd, ID_KEYREP, str, sizeof(str) );
//		st = strtol( str, NULL, 0 );
//		ecfg->SetKeyRepeat( st );
		
//		break;
		
//	case PP_FOLDER:	// フォルダ
//		// ROMパス
//		GetDlgItemText( hwnd, ID_PATH1, str, sizeof(str) );
//		ecfg->SetRomPath( str );
		
//		// TAPEパス
//		GetDlgItemText( hwnd, ID_PATH2, str, sizeof(str) );
//		ecfg->SetTapePath( str );
		
//		// DISKパス
//		GetDlgItemText( hwnd, ID_PATH3, str, sizeof(str) );
//		ecfg->SetDiskPath( str );
		
//		// 拡張ROMパス
//		GetDlgItemText( hwnd, ID_PATH4, str, sizeof(str) );
//		ecfg->SetExtRomPath( str );
		
//		// IMGパス
//		GetDlgItemText( hwnd, ID_PATH5, str, sizeof(str) );
//		ecfg->SetImgPath( str );
		
//		// WAVEパス
//		GetDlgItemText( hwnd, ID_PATH6, str, sizeof(str) );
//		ecfg->SetWavePath( str );
		
//		// FONTパス
//		GetDlgItemText( hwnd, ID_PATH7, str, sizeof(str) );
//		ecfg->SetFontPath( str );
		
//		break;
		
//	case PP_FILE:	// ファイル
//		// 拡張ROMファイル
//		GetDlgItemText( hwnd, ID_FEXROM, str, sizeof(str) );
//		ecfg->SetExtRomFile( str );
		
//		// TAPE(LOAD)ファイル名
//		GetDlgItemText( hwnd, ID_FTPLD, str, sizeof(str) );
//		ecfg->SetTapeFile( str );
		
//		// TAPE(SAVE)ファイル名
//		GetDlgItemText( hwnd, ID_FTPSV, str, sizeof(str) );
//		ecfg->SetSaveFile( str );
		
//		// DISKファイル名
//		GetDlgItemText( hwnd, ID_FDISK, str, sizeof(str) );
//		ecfg->SetDiskFile( str );
		
//		// プリンタファイル名
//		GetDlgItemText( hwnd, ID_FPRINT, str, sizeof(str) );
//		ecfg->SetPrinterFile( str );
		
//		// 全角フォントファイル名
//		GetDlgItemText( hwnd, ID_FFONTZ, str, sizeof(str) );
//		ecfg->SetFontFileZ( str );
		
//		// 半角フォントファイル名
//		GetDlgItemText( hwnd, ID_FFONTH, str, sizeof(str) );
//		ecfg->SetFontFileH( str );
		
//		break;
		
//	case PP_COL:	// 色1
//		break;
		
//	case PP_ETC:	// その他
//		// オーバークロック率
//		GetDlgItemText( hwnd, ID_OVERCLK, str, sizeof(str) );
//		st = min( max( 1, strtol( str, NULL, 0 ) ), 1000 );
//		ecfg->SetOverClock( st );
		
//		// CRCチェック
//		ecfg->SetCheckCRC( IsDlgButtonChecked( hwnd, ID_CB4 ) );
		
//		// ROMパッチ
//		ecfg->SetRomPatch( IsDlgButtonChecked( hwnd, ID_CB9 ) );
		
//		// Turbo TAPE
//		ecfg->SetTurboTAPE( IsDlgButtonChecked( hwnd, ID_CB9 ) );
		
//		// Boost Up
//		ecfg->SetBoostUp( IsDlgButtonChecked( hwnd, ID_CB5 ) );
		
//		// BoostUp 最大倍率(N60モード)
//		GetDlgItemText( hwnd, ID_BOOST60, str, sizeof(str) );
//		st = min( max( 1, strtol( str, NULL, 0 ) ), 100 );
//		ecfg->SetMaxBoost1( st );
		
//		// BoostUp 最大倍率(N60m/N66モード)
//		GetDlgItemText( hwnd, ID_BOOST62, str, sizeof(str) );
//		st = min( max( 1, strtol( str, NULL, 0 ) ), 100 );
//		ecfg->SetMaxBoost2( st );
		
//		// ビデオキャプチャ RLE
//		ecfg->SetAviRle( IsDlgButtonChecked( hwnd, ID_CB6 ) );
		
//		// 終了時 確認する
//		ecfg->SetCkQuit( IsDlgButtonChecked( hwnd, ID_CB8 ) );
		
//		// 終了時 INIファイルを保存する
//		ecfg->SetSaveQuit( IsDlgButtonChecked( hwnd, ID_CB12 ) );
		
//		break;
//	}
	
//	return TRUE;
//}


///////////////////////////////////////////////////////////
// 環境設定ダイアログプロシージャ
///////////////////////////////////////////////////////////
//// 基本
//static BOOL CALLBACK OsdCnfgProc1( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_BASE ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
////		EnableWindow( GetDlgItem( hwnd, ID_CB1 ), SendMessage( GetDlgItem( hwnd, ID_RB01 ), BM_GETCHECK, 0, 0 ) );
//		break;
		
//	case WM_COMMAND:
///*
//		switch( wp ){
//		case ID_RB01:
//			EnableWindow( GetDlgItem( hwnd, ID_CB1 ), TRUE );
//			break;
			
//		case ID_RB02:
//		case ID_RB03:
//		case ID_RB04:
//		case ID_RB05:
//			EnableWindow( GetDlgItem( hwnd, ID_CB1 ), FALSE );
//		}
//*/
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:	// OKボタン処理
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_BASE ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

//// 画面
//static BOOL CALLBACK OsdCnfgProc2( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_DISP ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//		break;
		
//	case WM_COMMAND:
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_DISP ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

//// サウンド
//static BOOL CALLBACK OsdCnfgProc3( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_SOUND ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_SOUND ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

//// ファイル
//static BOOL CALLBACK OsdCnfgProc4( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_FILE ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//		break;
		
//	case WM_COMMAND:
//		char Folder[PATH_MAX];
//		switch( wp ){
//		case ID_B11:	// ファイル参照ボタン(拡張ROMイメージ)
//			GetDlgItemText( hwnd, ID_FEXROM, Folder, sizeof(Folder) );
//			if( OSD_FileSelect( hwnd, FD_ExtRom, Folder, ecfg->GetExtRomPath() ) ){
//				UnDelimiter( Folder );
//				SetDlgItemText( hwnd, ID_FEXROM, Folder );
//			}
//			break;
			
//		case ID_B12:	// ファイル参照ボタン(TAPEイメージ(LOAD))
//			GetDlgItemText( hwnd, ID_FTPLD, Folder, sizeof(Folder) );
//			if( OSD_FileSelect( hwnd, FD_TapeLoad, Folder, ecfg->GetTapePath() ) ){
//				UnDelimiter( Folder );
//				SetDlgItemText( hwnd, ID_FTPLD, Folder );
//			}
//			break;
			
//		case ID_B13:	// ファイル参照ボタン(TAPEイメージ(SAVE))
//			GetDlgItemText( hwnd, ID_FTPSV, Folder, sizeof(Folder) );
//			if( OSD_FileSelect( hwnd, FD_TapeSave, Folder, ecfg->GetTapePath() ) ){
//				UnDelimiter( Folder );
//				SetDlgItemText( hwnd, ID_FTPSV, Folder );
//			}
//			break;
			
//		case ID_B14:	// ファイル参照ボタン(DISKイメージ)
//			GetDlgItemText( hwnd, ID_FDISK, Folder, sizeof(Folder) );
//			if( OSD_FileSelect( hwnd, FD_Disk, Folder, ecfg->GetDiskPath() ) ){
//				UnDelimiter( Folder );
//				SetDlgItemText( hwnd, ID_FDISK, Folder );
//			}
//			break;
			
//		case ID_B15:	// ファイル参照ボタン(プリンタ出力ファイル)
//			GetDlgItemText( hwnd, ID_FPRINT, Folder, sizeof(Folder) );
//			if( OSD_FileSelect( hwnd, FD_Printer, Folder, NULL ) ){
//				UnDelimiter( Folder );
//				SetDlgItemText( hwnd, ID_FPRINT, Folder );
//			}
//			break;
			
//		case ID_B16:	// ファイル参照ボタン(全角フォントファイル)
//			GetDlgItemText( hwnd, ID_FFONTZ, Folder, sizeof(Folder) );
//			if( OSD_FileSelect( hwnd, FD_FontZ, Folder, ecfg->GetFontPath() ) ){
//				UnDelimiter( Folder );
//				SetDlgItemText( hwnd, ID_FFONTZ, Folder );
//			}
//			break;
			
//		case ID_B17:	// ファイル参照ボタン(半角フォントファイル)
//			GetDlgItemText( hwnd, ID_FFONTH, Folder, sizeof(Folder) );
//			if( OSD_FileSelect( hwnd, FD_FontH, Folder, ecfg->GetFontPath() ) ){
//				UnDelimiter( Folder );
//				SetDlgItemText( hwnd, ID_FFONTH, Folder );
//			}
//			break;
//		}
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_FILE ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

//// フォルダ
//static BOOL CALLBACK OsdCnfgProc5( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_FOLDER ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//		break;
		
//	case WM_COMMAND:
//		char Folder[PATH_MAX];
//		switch( wp ){
//		case ID_B01:	// フォルダ参照ボタン(ROM)
//			GetDlgItemText( hwnd, ID_PATH1, Folder, sizeof(Folder) );
//			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH1, Folder );
//			break;
			
//		case ID_B02:	// フォルダ参照ボタン(TAPE)
//			GetDlgItemText( hwnd, ID_PATH2, Folder, sizeof(Folder) );
//			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH2, Folder );
//			break;
			
//		case ID_B03:	// フォルダ参照ボタン(DISK)
//			GetDlgItemText( hwnd, ID_PATH3, Folder, sizeof(Folder) );
//			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH3, Folder );
//			break;
			
//		case ID_B04:	// フォルダ参照ボタン(拡張ROM)
//			GetDlgItemText( hwnd, ID_PATH4, Folder, sizeof(Folder) );
//			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH4, Folder );
//			break;
			
//		case ID_B05:	// フォルダ参照ボタン(IMG)
//			GetDlgItemText( hwnd, ID_PATH5, Folder, sizeof(Folder) );
//			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH5, Folder );
//			break;
			
//		case ID_B06:	// フォルダ参照ボタン(WAVE)
//			GetDlgItemText( hwnd, ID_PATH6, Folder, sizeof(Folder) );
//			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH6, Folder );
//			break;
			
//		case ID_B07:	// フォルダ参照ボタン(FONT)
//			GetDlgItemText( hwnd, ID_PATH7, Folder, sizeof(Folder) );
//			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH7, Folder );
//			break;
//		}
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_FOLDER ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

//// 色1
//static BOOL CALLBACK OsdCnfgProcCol( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	static COLORREF Color = 0;
//	static COLORREF CustColors[16] = { RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
//									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
//									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
//									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ) };
//	static CHOOSECOLOR cc = { 0 };	// 色の選択用
//	COLOR24 col;
	
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_COL ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		
//		// 色の選択 コモンダイアログ 初期化
//		cc.lStructSize  = sizeof(CHOOSECOLOR);
//		cc.hwndOwner    = hwnd;
//		cc.rgbResult    = Color;
//		cc.lpCustColors = CustColors;
//		cc.Flags        = CC_FULLOPEN | CC_RGBINIT;
		
//		break;
		
//	case WM_DRAWITEM:	// オーナードロー
//		if( wp >= ID_COL16 && wp <= ID_COL64 && ecfg->GetColor( wp-ID_COL16+16, &col ) ){
//			// オーナードローボタン 描画
//			OsdOwnerDrawBtn( ((LPDRAWITEMSTRUCT)lp)->hDC,
//							 ((LPDRAWITEMSTRUCT)lp)->rcItem,
//							 RGB( col.r, col.g, col.b ) );
//			return TRUE;
//		}
//		return FALSE;
		
//	case WM_COMMAND:
//		if( wp >= ID_COL16 && wp <= ID_COL64 && ecfg->GetColor( wp-ID_COL16+16, &col ) ){
//			Color = RGB( col.r, col.g, col.b );
//			// 色の選択 コモンダイアログ表示
//			cc.rgbResult = Color;
//			if( ChooseColor( &cc ) ){
//				Color = cc.rgbResult;
//				col.r = GetRValue( Color );
//				col.g = GetGValue( Color );
//				col.b = GetBValue( Color );
//				ecfg->SetColor( wp-ID_COL16+16, &col );
//				InvalidateRect( hwnd, NULL, TRUE );
//			}
//		}
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_COL ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

//// その他
//static BOOL CALLBACK OsdCnfgProcEtc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_ETC ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_ETC ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

//// 入力関係
//static BOOL CALLBACK OsdCnfgProcIn( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	switch( msg ){
//	case WM_INITDIALOG:
//		// 設定を読込む
//		if( !OsdReadINI( hwnd, PP_INPUT ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//		break;
		
//	case WM_NOTIFY:
//		switch( ((NMHDR *)lp)->code ){
//		case PSN_APPLY:
//			// 設定を保存する
//			if( !OsdWriteINI( hwnd, PP_INPUT ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			return TRUE;
//		}
//	}
//	return FALSE;
//}


//// オーナードローボタン 描画
//static void OsdOwnerDrawBtn( HDC hdc, RECT rc, COLORREF col )
//{
//	HPEN hOldPen   = (HPEN)SelectObject( hdc, (HPEN)GetStockObject( BLACK_PEN ) );
//	HBRUSH hBrs    = CreateSolidBrush( col );
//	HBRUSH hOldBrs = (HBRUSH)SelectObject( hdc, hBrs );
	
//	Rectangle( hdc, rc.left, rc.top, rc.right, rc.bottom );
	
//	SelectObject( hdc, hOldPen );
//	SelectObject( hdc, hOldBrs );
//	DeleteObject( hBrs );
//}



/////////////////////////////////////////////////////////////
//// バージョン情報ダイアログプロシージャ
/////////////////////////////////////////////////////////////
//static BOOL CALLBACK VerInfoProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
//{
//	static HCURSOR hCursor = NULL;
	
//	switch( msg ){
//	case WM_INITDIALOG:
//		{
//		// バージョン文字列設定
//		SetDlgItemText( hwnd, ID_VERSTR, APPNAME " Ver." VERSION );
		
//		// アイコン設定
//		const char *ilp;
//		switch( ccfg->GetModel() ){
//		case 62: ilp = MAKEINTRESOURCE(P62ICON); break;
//		case 66: ilp = MAKEINTRESOURCE(P66ICON); break;
//		case 64: ilp = MAKEINTRESOURCE(P64ICON); break;
//		case 68: ilp = MAKEINTRESOURCE(P68ICON); break;
//		default: ilp = MAKEINTRESOURCE(P60ICON);
//		}
//		HICON hNewIcon = LoadIcon( (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE), ilp );
//		HICON hOldIcon = (HICON)SendMessage( GetDlgItem( hwnd, ID_VERICON ), STM_SETICON, (WPARAM)hNewIcon, 0 );
//		if( hOldIcon ) DeleteObject( hOldIcon );
		
//		// 指型カーソル取得
//		HINSTANCE hInstHelp;
//		char WinDir[PATH_MAX];
		
//		GetWindowsDirectory( WinDir, PATH_MAX-13 );
//		lstrcat( WinDir, "\\winhlp32.exe");
//		hInstHelp = LoadLibrary( WinDir );
//		if( hInstHelp ){
//			hCursor = (HCURSOR)CopyImage( LoadImage( hInstHelp, MAKEINTRESOURCE(106), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR ), IMAGE_CURSOR, 0, 0, LR_COPYDELETEORG );
//			FreeLibrary( hInstHelp );
//		}else
//			hCursor = (HCURSOR)LoadImage( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR );
//		break;
//		}
//	case WM_SETCURSOR:	// カーソル変更
//		if( (HWND)wp == GetDlgItem( hwnd, ID_URL ) ){
//			SetCursor( hCursor );
//			SetWindowLong( hwnd, DWL_MSGRESULT, MAKELONG( TRUE, 0 ) );
//			return TRUE;
//		}
//		return FALSE;
//		break;
		
//	case WM_CTLCOLORSTATIC:	// URLの色を変える
//		{
//		HDC hDC = (HDC)wp;
//		if( (HWND)lp == GetDlgItem( hwnd, ID_URL ) ){
////			SelectObject( hDC, hFontHtml );		// フォントを変える
//			SetTextColor( hDC, RGB(0,0,255) );	// 色を変える
//			SetBkMode( hDC, TRANSPARENT );
//			return (BOOL)(HBRUSH)GetStockObject( NULL_BRUSH );
//		}
//		return (BOOL)(HBRUSH)( COLOR_BTNFACE + 1 );
//		}
//		break;
		
//	case WM_COMMAND:
//    	switch( LOWORD(wp) ){
//		case IDOK:
//		case IDCANCEL:
//			if( hCursor ) DestroyCursor( hCursor );	// カーソル開放
//			EndDialog( hwnd, TRUE );
//			break;
//		case ID_URL:
//			{
//			char Url[256];
//			GetWindowText( GetDlgItem( hwnd, ID_URL ), Url, 256 );
//			ShellExecute( hwnd, "open", Url, NULL, NULL, SW_SHOWNORMAL );
//			InvalidateRect( GetDlgItem( hwnd, ID_URL ), NULL, FALSE );
//			return TRUE;
//			}
//		}
//	}
//	return FALSE;
//}
