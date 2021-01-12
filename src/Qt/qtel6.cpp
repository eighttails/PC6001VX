#include <QtWidgets>

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
#include "../joystick.h"
#include "../id_menu.h"

#include "mainwidget.h"
#include "renderview.h"
#include "keypanel.h"
#include "virtualkeytabwidget.h"
#include "systeminfodialog.h"
#include "qtel6.h"
#include "p6vxapp.h"

#define	FRAMERATE	(VSYNC_HZ/(cfg->GetFrameSkip()+1))

///////////////////////////////////////////////////////////
// ポップアップメニュー表示
///////////////////////////////////////////////////////////
void EL6::ShowPopupMenu( int x, int y )
{
	QMetaObject::invokeMethod(qApp, "showPopupMenu",
							  Q_ARG(int, x),
							  Q_ARG(int, y));
}

////////////////////////////////////////////////////////////////
// メニュー選択項目実行
//
// 引数:	id		選択したメニューID
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::ExecMenu( int id )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	char str[PATH_MAX];
	// 項目ごとの処理
	switch( id ){
	case ID_TAPEINSERT:		UI_TapeInsert();						break;	// TAPE 挿入
	case ID_TAPEEJECT:		TapeUnmount();							break;	// TAPE 排出
	case ID_TAPEEXPORT:		app->exportSavedTape();					break;	// TAPE エクスポート
	case ID_DISKINSERT1:													// DISK 挿入
	case ID_DISKINSERT2:	UI_DiskInsert( id - ID_DISKINSERT1 );	break;
	case ID_DISKEJECT1:														// DISK 排出
	case ID_DISKEJECT2:		DiskUnmount( id - ID_DISKEJECT1 );		break;
	case ID_ROMINSERT:		UI_RomInsert();							break;	// 拡張ROM 挿入
	case ID_ROMEJECT:		UI_RomEject();							break;	// 拡張ROM 排出
	case ID_JOY100:															// ジョイスティック1
	case ID_JOY101:
	case ID_JOY102:
	case ID_JOY103:
	case ID_JOY104:
	case ID_JOY105:			joy->Connect( 0, id - ID_JOY101 );		break;
	case ID_JOY200:															// ジョイスティック2
	case ID_JOY201:
	case ID_JOY202:
	case ID_JOY203:
	case ID_JOY204:
	case ID_JOY205:			joy->Connect( 1, id - ID_JOY201 );		break;
	case ID_CONFIG:			UI_Config();							break;	// 環境設定
	case ID_RESET:			UI_Reset();								break;	// リセット
	case ID_RESTART:		UI_Restart();							break;	// 再起動
	case ID_PAUSE:															// 一時停止
		sche->SetPauseEnable(!sche->GetPauseEnable());
		break;
	case ID_SPEEDRESET:		Speed = 100;							break;	// 速度調整(リセット)
	case ID_SPEED50:		Speed = 50;								break;	// 速度調整(50%)
	case ID_SPEED70:		Speed = 70;								break;	// 速度調整(70%)
	case ID_SPEED150:		Speed = 150;							break;	// 速度調整(150%)
	case ID_SPEED200:		Speed = 200;							break;	// 速度調整(200%)
	case ID_SPEED300:		Speed = 300;							break;	// 速度調整(300%)
	case ID_SPEEDMANUAL:
	{
		int speed = QInputDialog::getInt(NULL, QtEL6::tr("動作速度"), QtEL6::tr("動作速度(%)を入力してください。(10-2000)"), 100, 10, 2000);
		// 200%以下は10%刻み、それ以上は100%刻み
		if (speed <= 200) speed -= speed % 10;
		if (speed > 200) speed -= speed % 100;
		Speed = speed;
		break;
	}
	case ID_SNAPSHOT:		graph->SnapShot( cfg->GetImgPath() );	break;	// スナップショット取得
	case ID_DOKOSAVE:		UI_DokoSave();							break;	// どこでもSAVE
	case ID_DOKOSAVE1:                                                      // どこでもSAVE1
	case ID_DOKOSAVE2:                                                      // どこでもSAVE2
	case ID_DOKOSAVE3:                                                      // どこでもSAVE3
		OSD_AddPath(str, cfg->GetDokoSavePath(), QString(".%1.dds").arg(id - ID_DOKOSAVE).toUtf8().constData());
		DokoDemoSave( str );
		break;
	case ID_DOKOLOAD:		UI_DokoLoad();							break;	// どこでもLOAD
	case ID_DOKOLOAD1:                                                      // どこでもLOAD1
	case ID_DOKOLOAD2:                                                      // どこでもLOAD2
	case ID_DOKOLOAD3:                                                      // どこでもLOAD3
		OSD_AddPath(str, cfg->GetDokoSavePath(), QString(".%1.dds").arg(id - ID_DOKOLOAD).toUtf8().constData());
		if( OSD_FileExist( str ) ){
			cfg->SetModel( GetDokoModel( str ) );
			cfg->SetDokoFile( str );
			OSD_PushEvent( EV_DOKOLOAD );
		}
		break;
	case ID_REPLAYSAVE:		UI_ReplaySave();						break;	// リプレイ保存
	case ID_REPLAYRESUME:	UI_ReplayResumeSave();					break;	// リプレイ保存再開
	case ID_REPLAYDOKOLOAD:	UI_ReplayDokoLoad();					break;	// リプレイ中どこでもLOAD
	case ID_REPLAYROLLBACK:	UI_ReplayRollback();					break;	// リプレイ中どこでもLOADを巻き戻し
	case ID_REPLAYDOKOSAVE:	UI_ReplayDokoSave();					break;	// リプレイ中どこでもSAVE
	case ID_REPLAYLOAD:		UI_ReplayLoad();						break;	// リプレイ再生
	case ID_REPLAYMOVIE:													// リプレイを動画に変換
		UI_ReplayLoad();
		OSD_PushEvent(EV_REPLAYMOVIE);
		break;
	case ID_AVISAVE:		UI_AVISave();							break;	// ビデオキャプチャ
	case ID_KEYPANEL:		app->toggleKeyPanel();					break;	// キーパネル
	case ID_VIRTURLKEY:		app->toggleVirtualKeyboard();			break;	// 仮想キーボード
	case ID_AUTOTYPE:		UI_AutoType();							break;	// 打込み代行
	case ID_QUIT:			UI_Quit();								break;	// 終了
	case ID_NOWAIT:			UI_NoWait();							break;	// Wait有効無効変更
	case ID_TURBO:			UI_TurboTape();							break;	// Turbo TAPE
	case ID_BOOST:			UI_BoostUp();							break;	// Boost Up
	case ID_FULLSCREEN:
		OSD_ShowCursor(cfg->GetFullScreen() ? true : false);
		cfg->SetFullScreen( cfg->GetFullScreen() ? false : true );
		graph->ResizeScreen();	// スクリーンサイズ変更
		break;
	case ID_SCANLINE:		UI_ScanLine();							break;	// スキャンラインモード変更
	case ID_TILT:                                                           // TILTモード変更
	{
		app->enableTilt(!app->isTiltEnabled());
		graph->ResizeScreen();	// スクリーンサイズ変更
		break;
	}
	case ID_DISP43:			UI_Disp43();							break;	// 4:3表示変更
	case ID_STATUS:			UI_StatusBar();							break;	// ステータスバー表示状態変更
	case ID_M4MONO:															// MODE4カラー モノクロ
	case ID_M4RDBL:															// MODE4カラー 赤/青
	case ID_M4BLRD:															// MODE4カラー 青/赤
	case ID_M4PKGR:															// MODE4カラー ピンク/緑
	case ID_M4GRPK:			UI_Mode4Color( id - ID_M4MONO );		break;	// MODE4カラー 緑/ピンク
	case ID_FSKP0:															// フレームスキップ なし
	case ID_FSKP1:															// フレームスキップ 1
	case ID_FSKP2:															// フレームスキップ 2
	case ID_FSKP3:															// フレームスキップ 3
	case ID_FSKP4:															// フレームスキップ 4
	case ID_FSKP5:			UI_FrameSkip( id - ID_FSKP0 );			break;	// フレームスキップ 5
	case ID_ONLINEHELP:
#ifdef Q_OS_WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromLocal8Bit(OSD_GetModulePath()) + "/README.html"));
		break;
#else
		QDesktopServices::openUrl(QUrl("https://github.com/eighttails/PC6001VX/blob/master/README.adoc"));
		break;
#endif
	case ID_VERSION:		OSD_VersionDialog( graph->GetWindowHandle(), cfg->GetModel() );	break;	// バージョン情報
	case ID_ABOUTQT:		QMessageBox::aboutQt(static_cast<RenderView*>(graph->GetWindowHandle()));
		break;
	case ID_SYSINFO:														// システム情報ダイアログ
	{
		auto&& dialog = SystemInfoDialog(static_cast<RenderView*>(graph->GetWindowHandle()));
#ifdef ALWAYSFULLSCREEN
		dialog.setWindowState(Qt::WindowFullScreen);
#endif
		dialog.exec();
		break;
	}
	case ID_RESETSETTINGS:	app->resetSettings();	break;
		break;
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	case ID_MONITOR:		ToggleMonitor();						break;	// モニターモード
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	case ID_SIZE50:
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(50);
		break;
	case ID_SIZE100:
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(100);
		break;
	case ID_SIZE150:
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(150);
		break;
	case ID_SIZE200:
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(200);
		break;
	case ID_SIZE250:
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(250);
		break;
	case ID_SIZE300:
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(300);
		break;
	case ID_SIZEMANUAL:
	{
		const int magnification = QInputDialog::getInt(NULL, QtEL6::tr("表示サイズ"), QtEL6::tr("表示サイズ(%)を入力してください。(50-1000)"), 100, 50, 1000);
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(magnification);
		break;
	}
	case ID_FIXMAGNIFICATION:
		app->setSetting(P6VXApp::keyFixMagnification,
						!app->getSetting(P6VXApp::keyFixMagnification).toBool());
		break;
	case ID_HWACCEL:
		if(OSD_Message(QtEL6::tr("設定を反映するには一度終了しますがよろしいですか?").toUtf8().constData(), MSG_QUITC, OSDM_OK | OSDM_OKCANCEL) == OSDR_OK){
			app->setSetting(P6VXApp::keyHwAccel,
							!app->getSetting(P6VXApp::keyHwAccel).toBool());
			UI_Quit();
		}
		break;
	case ID_FILTERING:
		app->setSetting(P6VXApp::keyFiltering,
						!app->getSetting(P6VXApp::keyFiltering).toBool());
		graph->ResizeScreen();
		break;
	default:
		qDebug() << "Unimplemented menu ID:" << id;
	}
}

///////////////////////////////////////////////////////////
// メニューコマンドID
///////////////////////////////////////////////////////////
Q_DECLARE_METATYPE(MenuCommand)
#define MENUIDPROPERTY "MenuID"


QAction* addCommand(QMenu* menu, QString label, MenuCommand id, bool checkable = false)
{
	QAction* action = menu->addAction(TRANS(label.toUtf8()));
	action->setProperty(MENUIDPROPERTY, QVariant::fromValue<MenuCommand>(id));
	action->setCheckable(checkable);
	return action;
}


QtEL6::QtEL6(QObject *parent)
	: QObject(parent)
{
	// FPS更新タイマ
	QTimer* fpsTimer = new QTimer(this);
	fpsTimer->start(1000);
	connect(fpsTimer, SIGNAL(timeout()), this, SLOT(UpdateFPS()));
}

QtEL6::~QtEL6()
{
}

void QtEL6::ShowPopupImpl(int x, int y)
{
	OSD_ShowCursor( true );
	QAction* selectedAction = NULL;
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	QMenu menu;

	// システムメニュー
	QMenu* systemMenu = menu.addMenu(tr("システム"));
	menu.addSeparator();
	addCommand(systemMenu, tr("リセット"), ID_RESET);
	addCommand(systemMenu, tr("再起動"), ID_RESTART);
	systemMenu->addSeparator();

	QAction* pause = addCommand(systemMenu, tr("一時停止"), ID_PAUSE, true);
	pause->setChecked(GetPauseEnable());
	QMenu* speedMenu = systemMenu->addMenu(tr("速度調節"));
	addCommand(speedMenu, tr("リセット"), ID_SPEEDRESET);
	addCommand(speedMenu, tr("50%"), ID_SPEED50);
	addCommand(speedMenu, tr("70%"), ID_SPEED70);
	addCommand(speedMenu, tr("150%"), ID_SPEED150);
	addCommand(speedMenu, tr("200%"), ID_SPEED200);
	addCommand(speedMenu, tr("300%"), ID_SPEED300);
	addCommand(speedMenu, tr("速度を指定..."), ID_SPEEDMANUAL);
	systemMenu->addSeparator();
	addCommand(systemMenu, tr("スナップショットを取得"), ID_SNAPSHOT);
	systemMenu->addSeparator();

	// どこでもLOAD,SAVEメニュー
	QMenu* dokoLoadMenu = systemMenu->addMenu(tr("どこでもLOAD"));
	QMenu* dokoSaveMenu = systemMenu->addMenu(tr("どこでもSAVE"));
	systemMenu->addSeparator();
	addCommand(dokoLoadMenu, tr("LOAD..."), ID_DOKOLOAD);
	addCommand(dokoLoadMenu, "1", ID_DOKOLOAD1);
	addCommand(dokoLoadMenu, "2", ID_DOKOLOAD2);
	addCommand(dokoLoadMenu, "3", ID_DOKOLOAD3);
	addCommand(dokoSaveMenu, tr("SAVE..."), ID_DOKOSAVE);
	addCommand(dokoSaveMenu, "1", ID_DOKOSAVE1);
	addCommand(dokoSaveMenu, "2", ID_DOKOSAVE2);
	addCommand(dokoSaveMenu, "3", ID_DOKOSAVE3);

	// リプレイメニュー
	QMenu* replayMenu = systemMenu->addMenu(tr("リプレイ"));
	systemMenu->addSeparator();
	// モニタモード or ブレークポインタが設定されている
	// またはリプレイ記録中だったらリプレイ再生無効
	if(!(
			#ifndef NOMONITOR
				cfg->GetMonDisp() || vm->bp->ExistBreakPoint() ||
			#endif
				( REPLAY::GetStatus() == REP_RECORD ) )){
		addCommand(replayMenu, (REPLAY::GetStatus() == REP_REPLAY) ? MSMEN_REP3: MSMEN_REP2, ID_REPLAYLOAD);
	}
	// モニタモード or ブレークポインタが設定されている
	// またはリプレイ再生中だったらリプレイ記録無効
	if(!(
			#ifndef NOMONITOR
				cfg->GetMonDisp() || vm->bp->ExistBreakPoint() ||
			#endif
				( REPLAY::GetStatus() == REP_REPLAY ) )){
		addCommand(replayMenu, (REPLAY::GetStatus() == REP_RECORD) ? MSMEN_REP1 : MSMEN_REP0, ID_REPLAYSAVE);
	}
	if (REPLAY::GetStatus() == REP_IDLE){
		addCommand(replayMenu, tr("記録再開..."), ID_REPLAYRESUME);
#ifndef NOAVI
		addCommand(replayMenu, tr("リプレイを動画に変換..."), ID_REPLAYMOVIE);
#endif
	}
	if (REPLAY::GetStatus() == REP_RECORD){
		addCommand(replayMenu, tr("途中保存"), ID_REPLAYDOKOSAVE);
		addCommand(replayMenu, tr("途中保存から再開"), ID_REPLAYDOKOLOAD);
		addCommand(replayMenu, tr("1つ前の途中保存から再開"), ID_REPLAYROLLBACK);
	}
	systemMenu->addSeparator();

#ifndef NOAVI
	// ビデオキャプチャ
	addCommand(systemMenu, AVI6::IsAVI() ? MSMEN_AVI1 : MSMEN_AVI0, ID_AVISAVE);
	systemMenu->addSeparator();
#endif

	QAction* keyPanel = addCommand(systemMenu, tr("キーパネル"), ID_KEYPANEL, true);
	keyPanel->setChecked(app->getKeyPanel()->isVisible());
	QAction* virtualKey = addCommand(systemMenu, tr("仮想キーボード"), ID_VIRTURLKEY, true);
	virtualKey->setChecked(app->getVirtualKeyboard()->isVisible());
	systemMenu->addSeparator();

	addCommand(systemMenu, tr("打込み代行..."), ID_AUTOTYPE);
	systemMenu->addSeparator();
	addCommand(systemMenu, tr("終了"), ID_QUIT);

	// TAPEメニュー
	QMenu* tapeMenu = menu.addMenu(tr("TAPE"));
	addCommand(tapeMenu, tr("挿入..."), ID_TAPEINSERT);
	QAction* tapeEject = addCommand(tapeMenu, tr("取出"), ID_TAPEEJECT);
	if(!*vm->cmtl->GetFile()) tapeEject->setEnabled(false);
	QAction* tapeExport = addCommand(tapeMenu, tr("TAPE(SAVE)をエクスポート..."), ID_TAPEEXPORT);

	// DISKメニュー
	if (vm->disk->GetDrives()){
		QMenu* diskMenu = menu.addMenu(tr("DISK"));
		for (int i = 0; i < vm->disk->GetDrives(); i++){
			QString item = QString("Drive%1").arg(i + 1);
			QMenu* driveMenu = diskMenu->addMenu(item);
			addCommand(driveMenu, tr("挿入..."), MenuCommand(ID_DISKINSERT1 + i));
			QAction* diskEject = addCommand(driveMenu, tr("取出"), MenuCommand(ID_DISKEJECT1 + i));
			if (!*vm->disk->GetFile(i)) diskEject->setEnabled(false);
		}
	}

	// 拡張ROMメニュー
	QMenu* extRomMenu = menu.addMenu(tr("拡張ROM"));
	addCommand(extRomMenu, tr("挿入..."), ID_ROMINSERT);
	QAction* romEject = addCommand(extRomMenu, tr("取出"), ID_ROMEJECT);
	if(!*vm->mem->GetFile()) romEject->setEnabled(false);

#ifndef NOJOYSTICK
	// ジョイスティックメニュー
	QMenu* joystickMenu = menu.addMenu(tr("ジョイスティック"));
	// ------
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
	QAction* noJoy1 = addCommand(joyMenu1, tr("なし"), ID_JOY100, true);
	QAction* noJoy2 = addCommand(joyMenu2, tr("なし"), ID_JOY200, true);
	joyGroup1->addAction(noJoy1);
	joyGroup2->addAction(noJoy2);
	if (joy->GetID(0) < 0) noJoy1->setChecked(true);
	if (joy->GetID(1) < 0) noJoy2->setChecked(true);
#endif // NOJOYSTICK

	// 設定メニュー
	QMenu* settingsMenu = menu.addMenu(tr("設定"));
	// 表示サイズメニュー
	QMenu* dispSizeMenu = settingsMenu->addMenu(tr("表示サイズ"));
	addCommand(dispSizeMenu, tr("50%"), ID_SIZE50);
	addCommand(dispSizeMenu, tr("100%"), ID_SIZE100);
	addCommand(dispSizeMenu, tr("150%"), ID_SIZE150);
	addCommand(dispSizeMenu, tr("200%"), ID_SIZE200);
	addCommand(dispSizeMenu, tr("250%"), ID_SIZE250);
	addCommand(dispSizeMenu, tr("300%"), ID_SIZE300);
	addCommand(dispSizeMenu, tr("倍率を指定..."), ID_SIZEMANUAL);
	QAction* fixMagnification = addCommand(dispSizeMenu, tr("倍率を固定"), ID_FIXMAGNIFICATION, true);
	if (app->getSetting(P6VXApp::keyFixMagnification).toBool()) fixMagnification->setChecked(true);
#ifndef ALWAYSFULLSCREEN
	QAction* fullScreen = addCommand(settingsMenu, tr("フルスクリーン"), ID_FULLSCREEN, true);
	if (cfg->GetFullScreen()) fullScreen->setChecked(true);
#endif
	QAction* statusBar = addCommand(settingsMenu, tr("ステータスバー"), ID_STATUS, true);
	if (cfg->GetDispStat()) statusBar->setChecked(true);
	QAction* disp43 = addCommand(settingsMenu, tr("4:3表示"), ID_DISP43, true);
	if (cfg->GetDispNTSC()) disp43->setChecked(true);
	QAction* scanLine = addCommand(settingsMenu, tr("スキャンライン"), ID_SCANLINE, true);
	if (cfg->GetScanLine()) scanLine->setChecked(true);
#ifndef NOOPENGL
	QAction* hwAccel = addCommand(settingsMenu, tr("ハードウェアアクセラレーション"), ID_HWACCEL, true);
	if (app->getSetting(P6VXApp::keyHwAccel).toBool()) hwAccel->setChecked(true);
#endif
	QAction* filtering = addCommand(settingsMenu, tr("フィルタリング"), ID_FILTERING, true);
	if (app->getSetting(P6VXApp::keyFiltering).toBool()) filtering->setChecked(true);

	QAction* tiltMode = addCommand(settingsMenu, tr("TILTモード"), ID_TILT, true);
	if (app->isTiltEnabled()) tiltMode->setChecked(true);

	QMenu* colorMenu = settingsMenu->addMenu(tr("MODE4 カラー"));
	QActionGroup* colorGroup = new QActionGroup(&menu);
	QStringList colorList = (QStringList()
							 << tr("モノクロ")
							 << tr("赤/青")
							 << tr("青/赤")
							 << tr("桃/緑")
							 << tr("緑/桃"));
	for( int i = 0; i < 5; i++ ){
		QAction* color = addCommand(colorMenu, colorList[i], MenuCommand(ID_M4MONO + i), true);
		if (vm->vdg->GetMode4Color() == i) color->setChecked(true);
	}

	QMenu* fpsMenu = settingsMenu->addMenu(tr("フレームスキップ"));
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

	QAction* noWait = addCommand(settingsMenu, tr("ウェイト無効"), ID_NOWAIT, true);
	if (!sche->GetWaitEnable()) noWait->setChecked(true);
	QAction* turboTape = addCommand(settingsMenu, tr("Turbo TAPE"), ID_TURBO, true);
	if (cfg->GetTurboTAPE()) turboTape->setChecked(true);
	QAction* boostUp =  addCommand(settingsMenu, tr("Boost Up"), ID_BOOST, true);
	if (vm->cmtl->IsBoostUp()) boostUp->setChecked(true);
	addCommand(settingsMenu, tr("環境設定..."), ID_CONFIG);

#ifndef NOMONITOR
	// デバッグメニュー
	QMenu* debugMenu = menu.addMenu(tr("デバッグ"));
	menu.addSeparator();
	QAction* monitorMode =  addCommand(debugMenu, tr("モニタモード"), ID_MONITOR, true);
	if (cfg->GetMonDisp()) monitorMode->setChecked(true);
#endif

	// ヘルプメニュー
	QMenu* helpMenu = menu.addMenu(tr("ヘルプ"));
	addCommand(helpMenu, tr("オンラインヘルプ"), ID_ONLINEHELP);
	addCommand(helpMenu, tr("バージョン情報..."), ID_VERSION);
	addCommand(helpMenu, tr("About Qt..."), ID_ABOUTQT);
	addCommand(helpMenu, tr("システム情報..."), ID_SYSINFO);
	addCommand(helpMenu, tr("設定初期化..."), ID_RESETSETTINGS);

	selectedAction = menu.exec(QPoint(x,y));

	if (selectedAction != NULL) {
		// 項目ごとの処理
		MenuCommand id = selectedAction->property(MENUIDPROPERTY).value<MenuCommand>();
		ExecMenu(id);
	}
}



void QtEL6::UpdateFPS()
{
	Event ev;
	ev.type = EV_FPSUPDATE;
	ev.fps.fps = UDFPSCount;
	OSD_PushEvent( ev );
	UDFPSCount = 0;
}


///////////////////////////////////////////////////////////
// 環境設定プロパティシート表示
///////////////////////////////////////////////////////////
//static int ShowConfig()
//{
//    // INIファイルを開く
//    try{
//        ecfg = new CFG6();
//        if( !ecfg->Init() ) throw Error::IniReadFailed;
//    }
//    // new に失敗した場合
//    catch( std::bad_alloc ){
//        return -1;
//    }
//    // 例外発生
//    catch( Error::Errno i ){
//        delete ecfg;
//        ecfg = NULL;
//        return -1;
//    }

//    ConfigDialog dialog(ecfg);
//    dialog.exec();
//    int ret = dialog.result();
//    // OKボタンが押されたならINIファイル書込み
//    if( ret == QDialog::Accepted) ecfg->Write();

//    delete ecfg;
//    ecfg = NULL;

//    return ret;
//}

bool QtEL6::GetPauseEnable()
{
	if(sche){
		return sche->GetPauseEnable();
	}
}

void QtEL6::SetPauseEnable(bool en)
{
	if(sche){
		sche->SetPauseEnable(en);
	}
}

KEY6 *QtEL6::GetKeyboard()
{
	return vm->key;
}

