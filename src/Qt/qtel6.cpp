#include <QtGui>

#include "../pc6001v.h"
#include "../p6el.h"
#include "../p6vm.h"
#include "../config.h"
#include "../breakpoint.h"
#include "../disk.h"
#include "../error.h"
#include "../graph.h"
#include "../ini.h"
#include "../memory.h"
#include "../movie.h"
#include "../osd.h"
#include "../schedule.h"
#include "../tape.h"
#include "../vdg.h"

#include "configdialog.h"
#include "aboutdialog.h"

#include "qtel6.h"

///////////////////////////////////////////////////////////
// ポップアップメニュー表示
///////////////////////////////////////////////////////////
void EL6::ShowPopupMenu( int x, int y )
{
    QMetaObject::invokeMethod(qApp, "showPopupMenu",
                              Q_ARG(int, x),
                              Q_ARG(int, y));
}

///////////////////////////////////////////////////////////
// ローカル関数定義
///////////////////////////////////////////////////////////
static int ShowConfig();			// 環境設定プロパティシート表示

///////////////////////////////////////////////////////////
// 仕方なしにスタティック変数
///////////////////////////////////////////////////////////
static CFG6 *ccfg = NULL;				// 環境設定オブジェクトポインタ
static CFG6 *ecfg = NULL;				// 環境設定オブジェクトポインタ(編集用)
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
    ID_EXIT,        // 終了
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
    ID_SPR44,       // 44.1kHz
    ID_SPR22,       // 22.05kHz
    ID_SPR11,       // 11.025kHz
    ID_MONITOR,		// モニターモード
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
QtEL6::QtEL6()
{
}

void QtEL6::ShowPopupImpl(int x, int y)
{
    QAction* selectedAction = NULL;

    QMenu menu;

    // システムメニュー
    QMenu* systemMenu = menu.addMenu("システム");
    menu.addSeparator();
    addCommand(systemMenu, "リセット", ID_RESET);
    addCommand(systemMenu, "再起動", ID_RESTART);
    systemMenu->addSeparator();

    // どこでもLOAD,SAVEメニュー
    QMenu* dokoMenu = systemMenu->addMenu("どこでも");
    systemMenu->addSeparator();
    addCommand(dokoMenu, "LOAD...", ID_DOKOLOAD);
    addCommand(dokoMenu, "SAVE...", ID_DOKOSAVE);

    // リプレイメニュー
    QMenu* replayMenu = systemMenu->addMenu("リプレイ");
    systemMenu->addSeparator();
    QAction* repleyLoad = addCommand(replayMenu, (REPLAY::GetStatus() == REP_REPLAY) ? MSMEN_REP3: MSMEN_REP2, ID_REPLAYLOAD);
    // モニタモード or ブレークポインタが設定されている
    // またはリプレイ記録中だったらリプレイ再生無効
    if( cfg->GetMonDisp() || vm->bp->ExistBreakPoint() || ( REPLAY::GetStatus() == REP_RECORD ) )
        repleyLoad->setEnabled(false);

    QAction* repleySave = addCommand(replayMenu, (REPLAY::GetStatus() == REP_RECORD) ? MSMEN_REP1 : MSMEN_REP0, ID_REPLAYSAVE);
    systemMenu->addSeparator();
    // モニタモード or ブレークポインタが設定されている
    // またはリプレイ再生中だったらリプレイ記録無効
    if( cfg->GetMonDisp() || vm->bp->ExistBreakPoint() || ( REPLAY::GetStatus() == REP_REPLAY ) )
        repleySave->setEnabled(false);

    // ビデオキャプチャ
    addCommand(systemMenu, AVI6::IsAVI() ? MSMEN_AVI1 : MSMEN_AVI0, ID_AVISAVE);
    systemMenu->addSeparator();

    addCommand(systemMenu, "打込み代行...", ID_AUTOTYPE);
    systemMenu->addSeparator();
    addCommand(systemMenu, "終了", ID_EXIT);

    // TAPEメニュー
    QMenu* tapeMenu = menu.addMenu("TAPE");
    addCommand(tapeMenu, "挿入...", ID_TAPEINSERT);
    QAction* tapeEject = addCommand(tapeMenu, "取出", ID_TAPEEJECT);
    if(!*vm->cmtl->GetFile()) tapeEject->setEnabled(false);

    // DISKメニュー
    if (vm->disk->GetDrives()){
        QMenu* diskMenu = menu.addMenu("DISK");
        for (int i = 0; i < vm->disk->GetDrives(); i++){
            QString item = QString("Drive%1").arg(i + 1);
            QMenu* driveMenu = diskMenu->addMenu(item);
            addCommand(driveMenu, "挿入...", MenuCommand(ID_DISKINSERT1 + i));
            QAction* diskEject = addCommand(driveMenu, "取出", MenuCommand(ID_DISKEJECT1 + i));
            if (!*vm->disk->GetFile(i)) diskEject->setEnabled(false);
        }
    }

    // 拡張ROMメニュー
    QMenu* extRomMenu = menu.addMenu("拡張ROM");
    addCommand(extRomMenu, "挿入...", ID_ROMINSERT);
    QAction* romEject = addCommand(extRomMenu, "取出", ID_ROMEJECT);
    if(!*vm->mem->GetFile()) romEject->setEnabled(false);

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
        if (vm->vdg->GetMode4Color() == i) color->setChecked(true);
    }

    QMenu* fpsMenu = settingsMenu->addMenu("フレームスキップ");
    settingsMenu->addSeparator();
    QActionGroup* fpsGroup = new QActionGroup(&menu);
    QStringList fpsList = (QStringList()
                           << "0 (60fps)"
                           << "1 (30fps)"
                           << "2 (20fps)"
                           << "3 (15fps)"
                           << "4 (12fps)"
                           << "5 (10fps)");
    for( int i = 0; i < fpsList.size(); i++ ){
        QAction* fps = addCommand(fpsMenu, fpsList[i], MenuCommand(ID_FSKP0 + i), true);
        if (cfg->GetFrameSkip() == i) fps->setChecked(true);
    }

    QMenu* sprMenu = settingsMenu->addMenu("サンプリングレート");
    settingsMenu->addSeparator();
    QActionGroup* sprGroup = new QActionGroup(&menu);
    QStringList sprList = (QStringList()
                           << "44100Hz"
                           << "22050Hz"
                           << "11025Hz");
    for( int i = 0; i < sprList.size(); i++ ){
        QAction* spr = addCommand(sprMenu, sprList[i], MenuCommand(ID_SPR44 + i), true);
        if (2 - ((cfg->GetSampleRate()/11025)>>1) == i) spr->setChecked(true);
    }

    QAction* noWait = addCommand(settingsMenu, "ウェイト無効", ID_NOWAIT, true);
    if (!sche->GetWaitEnable()) noWait->setChecked(true);
    QAction* turboTape = addCommand(settingsMenu, "Turbo TAPE", ID_TURBO, true);
    if (cfg->GetTurboTAPE()) turboTape->setChecked(true);
    QAction* boostUp =  addCommand(settingsMenu, "Boost Up", ID_BOOST, true);
    if (vm->cmtl->IsBoostUp()) boostUp->setChecked(true);
    addCommand(settingsMenu, "環境設定...", ID_CONFIG);

    // デバッグメニュー
    QMenu* debugMenu = menu.addMenu("デバッグ");
    menu.addSeparator();
    QAction* monitorMode =  addCommand(debugMenu, "モニタモード", ID_MONITOR, true);
    if (cfg->GetMonDisp()) monitorMode->setChecked(true);

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
            if( !TapeMount( str ) ) Error::SetError( Error::TapeMountFailed );
        break;

    case ID_TAPEEJECT:		// TAPE 排出
        TapeUnmount();
        break;

    case ID_DISKINSERT1:	// DISK1 挿入
        if( !OSD_FileExist( DiskPathTemp ) )
            strncpy( DiskPathTemp, cfg->GetDiskPath(), PATH_MAX );
        if( OSD_FileSelect( NULL, FD_Disk, str, DiskPathTemp ) )
            if( !DiskMount( 0, str ) ) Error::SetError( Error::DiskMountFailed );
        break;

    case ID_DISKEJECT1:		// DISK1 排出
        DiskUnmount( 0 );
        break;

    case ID_DISKINSERT2:	// DISK2 挿入
        if( !OSD_FileExist( DiskPathTemp ) )
            strncpy( DiskPathTemp, cfg->GetDiskPath(), PATH_MAX );
        if( OSD_FileSelect( NULL, FD_Disk, str, DiskPathTemp ) )
            if( !DiskMount( 1, str ) ) Error::SetError( Error::DiskMountFailed );
        break;

    case ID_DISKEJECT2:		// DISK2 排出
        DiskUnmount( 1 );
        break;

    case ID_ROMINSERT:		// 拡張ROM 挿入
        if( !OSD_FileExist( ExtRomPathTemp ) )
            strncpy( ExtRomPathTemp, cfg->GetExtRomPath(), PATH_MAX );
        if( OSD_FileSelect( NULL, FD_ExtRom, str, ExtRomPathTemp ) ){
            // リセットを伴うのでメッセージ表示
            OSD_Message( MSG_RESETI, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
            if( !vm->mem->MountExtRom( str ) )
                Error::SetError( Error::ExtRomMountFailed );
            else
                vm->Reset();
        }
        break;

    case ID_ROMEJECT:		// 拡張ROM 排出
        // リセットを伴うのでメッセージ表示
        OSD_Message( MSG_RESETE, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
        vm->mem->UnmountExtRom();
        vm->Reset();
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
                OSD_PushEvent( EV_RESTART );
                        break;

    case ID_RESET:			// リセット
        vm->Reset();
        break;

    case ID_RESTART:		// 再起動
        OSD_PushEvent( EV_RESTART );
                break;

    case ID_EXIT:           // 終了
        OSD_PushEvent( EV_QUIT );
                break;

    case ID_DOKOSAVE:		// どこでもSAVE
        if( OSD_FileSelect( NULL, FD_DokoSave, str, (char *)OSD_GetConfigPath() ) )
            DokoDemoSave( str );
        break;

    case ID_DOKOLOAD:		// どこでもLOAD
        if( OSD_FileSelect( NULL, FD_DokoLoad, str, (char *)OSD_GetConfigPath() ) ){
            cfg->SetModel( GetDokoModel( str ) );
            cfg->SetDokoFile( str );
            OSD_PushEvent( EV_DOKOLOAD );
        }
        break;

    case ID_REPLAYSAVE:		// リプレイ保存
        if( REPLAY::GetStatus() == REP_IDLE ){
            if( OSD_FileSelect( NULL, FD_RepSave, str, (char *)OSD_GetConfigPath() ) ){
                if( DokoDemoSave( str ) ) ReplayStartRec( str );
            }
        }else if( REPLAY::GetStatus() == REP_RECORD ){
            ReplayStopRec();
        }
        break;

    case ID_REPLAYLOAD:		// リプレイ再生
        if( REPLAY::GetStatus() == REP_IDLE ){
            if( OSD_FileSelect( NULL, FD_RepLoad, str, (char *)OSD_GetConfigPath() ) ){
                ReplayStartPlay( str );
            }
        }else if( REPLAY::GetStatus() == REP_REPLAY ){
            ReplayStopPlay();
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
        OSD_PushEvent( EV_QUIT );
                break;

    case ID_NOWAIT:			// Wait有効無効変更
        sche->SetWaitEnable( sche->GetWaitEnable() ? false : true );
        break;

    case ID_TURBO:			// Turbo TAPE
        cfg->SetTurboTAPE( cfg->GetTurboTAPE() ? false : true );
        break;

    case ID_BOOST:			// Boost Up
        cfg->SetBoostUp( cfg->GetBoostUp() ? false : true );
        vm->cmtl->SetBoost( vm->cmtl->IsBoostUp() ? false : true );
        break;

    case ID_SCANLINE:		// スキャンラインモード変更
        // ビデオキャプチャ中は無効
        if( !AVI6::IsAVI() ){
            cfg->SetScanLine( cfg->GetScanLine() ? false : true );
            graph->ResizeScreen();	// スクリーンサイズ変更
        }
        break;

    case ID_DISP43:			// 4:3表示変更
        // ビデオキャプチャ中は無効
        if( !AVI6::IsAVI() ){
            cfg->SetDispNTSC( cfg->GetDispNTSC() ? false : true );
            graph->ResizeScreen();	// スクリーンサイズ変更
        }
        break;

    case ID_STATUS:			// ステータスバー表示状態変更
        cfg->SetStatDisp( cfg->GetStatDisp() ? false : true );
        graph->ResizeScreen();	// スクリーンサイズ変更
        break;

        // MODE4カラー
    case ID_M4MONO:		// モノクロ
        cfg->SetMode4Color( 0 );
        vm->vdg->SetMode4Color( 0 );
        break;
    case ID_M4RDBL:		// 赤/青
        cfg->SetMode4Color( 1 );
        vm->vdg->SetMode4Color( 1 );
        break;
    case ID_M4BLRD:		// 青/赤
        cfg->SetMode4Color( 2 );
        vm->vdg->SetMode4Color( 2 );
        break;
    case ID_M4PKGR:		// ピンク/緑
        cfg->SetMode4Color( 3 );
        vm->vdg->SetMode4Color( 3 );
        break;
    case ID_M4GRPK:		// 緑/ピンク
        cfg->SetMode4Color( 4 );
        vm->vdg->SetMode4Color( 4 );
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

        // サンプリングレート
    case ID_SPR44:		// 44100Hz
        cfg->SetSampleRate( 44100 );
        snd->SetSampleRate( 44100 );
        break;
    case ID_SPR22:		// 22050Hz
        cfg->SetSampleRate( 22050 );
        snd->SetSampleRate( 22050 );
        break;
    case ID_SPR11:		// 11025Hz
        cfg->SetSampleRate( 11025 );
        snd->SetSampleRate( 11025 );
        break;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
    case ID_MONITOR:		// モニターモード
        ToggleMonitor();
        break;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

    case ID_VERSION:		// バージョン情報
        AboutDialog dialog(cfg);
        dialog.exec();
        break;
    }
}


///////////////////////////////////////////////////////////
// 環境設定プロパティシート表示
///////////////////////////////////////////////////////////
static int ShowConfig()
{
    // INIファイルを開く
    try{
        ecfg = new CFG6();
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

    ConfigDialog dialog(ecfg);
    dialog.exec();
    int ret = dialog.result();
    // OKボタンが押されたならINIファイル書込み
    if( ret == QDialog::Accepted) ecfg->Write();

    delete ecfg;
    ecfg = NULL;

    return ret;
}


