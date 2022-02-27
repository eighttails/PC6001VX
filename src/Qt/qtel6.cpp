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


///////////////////////////////////////////////////////////
// ポップアップメニュー表示
///////////////////////////////////////////////////////////
void EL6::ShowPopupMenu( int x, int y )
{
	// 呼び元スレッドによってコネクションタイプを変える(メニューが閉じるまで待つように)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "showPopupMenu",
							  cType,
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
	std::string str;
	// 項目ごとの処理
	switch( id ){
	case ID_TAPEINSERT:		UI_TapeInsert();						break;	// TAPE 挿入
	case ID_TAPEEJECT:		TapeUnmount();							break;	// TAPE 排出
	case ID_TAPEEXPORT:		app->exportSavedTape();					break;	// TAPE エクスポート
	case ID_DISKINSERT1:													// DISK 挿入
	case ID_DISKINSERT2:	UI_DiskInsert( id - ID_DISKINSERT1 );	break;
	case ID_DISKEJECT1:														// DISK 排出
	case ID_DISKEJECT2:		DiskUnmount( id - ID_DISKEJECT1 );		break;

	case ID_C6005:			UI_CartInsert     ( EXC6005   );		break;	// 拡張カートリッジ 挿入			(PC-6005	ROMカートリッジ)
	case ID_C6006:			UI_CartInsert     ( EXC6006   );		break;	// 拡張カートリッジ 挿入			(PC-6006	拡張ROM/RAMカートリッジ)
	case ID_C6006NR:		UI_CartInsertNoRom( EXC6006   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(PC-6006	拡張ROM/RAMカートリッジ)
	case ID_C6001:			UI_CartInsert     ( EXC6001   );		break;	// 拡張カートリッジ 挿入			(PCS-6001R	拡張BASIC)
	case ID_C660101:		UI_CartInsert     ( EXC660101 );		break;	// 拡張カートリッジ 挿入			(PC-6601-01	拡張漢字ROMカートリッジ)
	case ID_C6006SR:		UI_CartInsert     ( EXC6006SR );		break;	// 拡張カートリッジ 挿入			(PC-6006SR	拡張64KRAMカートリッジ)
	case ID_C6007SR:		UI_CartInsert     ( EXC6007SR );		break;	// 拡張カートリッジ 挿入			(PC-6007SR	拡張漢字ROM&RAMカートリッジ)
	case ID_C6053:			UI_CartInsert     ( EXC6053   );		break;	// 拡張カートリッジ 挿入			(PC-6053    ボイスシンセサイザー)
	case ID_C60M55:			UI_CartInsert     ( EXC60M55  );		break;	// 拡張カートリッジ 挿入			(PC-60m55   FM音源カートリッジ)
	case ID_CSOL1:			UI_CartInsert     ( EXCSOL1   );		break;	// 拡張カートリッジ 挿入			(戦士のカートリッジ)
	case ID_CSOL1NR:		UI_CartInsertNoRom( EXCSOL1   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(戦士のカートリッジ)
	case ID_CSOL2:			UI_CartInsert     ( EXCSOL2   );		break;	// 拡張カートリッジ 挿入			(戦士のカートリッジmkⅡ)
	case ID_CSOL2NR:		UI_CartInsertNoRom( EXCSOL2   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(戦士のカートリッジmkⅡ)
	case ID_CSOL3:			UI_CartInsert     ( EXCSOL3   );		break;	// 拡張カートリッジ 挿入			(戦士のカートリッジmkⅢ)
	case ID_CSOL3NR:		UI_CartInsertNoRom( EXCSOL3   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(戦士のカートリッジmkⅢ)
	case ID_CARTEJECT:		UI_CartEject();							break;	// 拡張カートリッジ 排出

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
		int speed = QInputDialog::getInt(nullptr, QtEL6::tr("動作速度"), QtEL6::tr("動作速度(%)を入力してください。(10-2000)"), 100, 10, 2000);
		// 200%以下は10%刻み、それ以上は100%刻み
		if (speed <= 200) speed -= speed % 10;
		if (speed > 200) speed -= speed % 100;
		Speed = speed;
		break;
	}
	case ID_SNAPSHOT:		graph->SnapShot( cfg->GetValue(CF_ImgPath) );	break;	// スナップショット取得
	case ID_DOKOSAVE:		UI_DokoSave();							break;	// どこでもSAVE
	case ID_DOKOSAVE1:                                                      // どこでもSAVE1
	case ID_DOKOSAVE2:                                                      // どこでもSAVE2
	case ID_DOKOSAVE3:                                                      // どこでもSAVE3
		UI_DokoSave( id - ID_DOKOSAVE );
		break;
	case ID_DOKOLOAD:		UI_DokoLoad();							break;	// どこでもLOAD
	case ID_DOKOLOAD1:                                                      // どこでもLOAD1
	case ID_DOKOLOAD2:                                                      // どこでもLOAD2
	case ID_DOKOLOAD3:                                                      // どこでもLOAD3
		UI_DokoLoad( id - ID_DOKOLOAD );
		break;
	case ID_REPLAYSAVE:		UI_ReplaySave();						break;	// リプレイ保存
	case ID_REPLAYRESUME:	UI_ReplayResumeSave();					break;	// リプレイ保存再開
	case ID_REPLAYDOKOLOAD:	UI_ReplayDokoLoad();					break;	// リプレイ中どこでもLOAD
	case ID_REPLAYROLLBACK:	UI_ReplayRollback();					break;	// リプレイ中どこでもLOADを巻き戻し
	case ID_REPLAYDOKOSAVE:	UI_ReplayDokoSave();					break;	// リプレイ中どこでもSAVE
	case ID_REPLAYPLAY:		UI_ReplayPlay();						break;	// リプレイ再生
	case ID_REPLAYMOVIE:													// リプレイを動画に変換
		UI_ReplayMovie();
		OSD_PushEvent(EV_REPLAYMOVIE);
		break;
	case ID_AVISAVE:		AVI6::IsAVI() ? UI_AVISaveStop()
										  : UI_AVISaveStart();		break;	// ビデオキャプチャ
	case ID_KEYPANEL:		app->toggleKeyPanel();					break;	// キーパネル
	case ID_VIRTURLKEY:		app->toggleVirtualKeyboard();			break;	// 仮想キーボード
	case ID_AUTOTYPE:		UI_AutoType();							break;	// 打込み代行
	case ID_QUIT:			UI_Quit();								break;	// 終了
	case ID_NOWAIT:			UI_NoWait();							break;	// Wait有効無効変更
	case ID_TURBO:			UI_TurboTape();							break;	// Turbo TAPE
	case ID_BOOST:			UI_BoostUp();							break;	// Boost Up
	case ID_FULLSCRN:
		OSD_ShowCursor(cfg->GetValue(CB_FullScreen) ? true : false);
		cfg->SetValue(CB_FullScreen,  cfg->GetValue(CB_FullScreen) ? false : true );
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
		QDesktopServices::openUrl(QUrl::fromLocalFile(P6VPATH2QSTR(OSD_GetConfigPath()) + "/README.html"));
		break;
#else
		QDesktopServices::openUrl(QUrl("https://github.com/eighttails/PC6001VX/blob/master/README.adoc"));
		break;
#endif
	case ID_VERSION:		OSD_VersionDialog( graph->GetWindowHandle(), cfg->GetValue(CV_Model) );	break;	// バージョン情報
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
	case ID_MONITOR:		UI_Monitor();						break;	// モニターモード
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
		const int magnification = QInputDialog::getInt(nullptr, QtEL6::tr("表示サイズ"), QtEL6::tr("表示サイズ(%)を入力してください。(50-1000)"), 100, 50, 1000);
		static_cast<RenderView*>(graph->GetWindowHandle())->resizeWindowByRatio(magnification);
		break;
	}
	case ID_FIXMAGNIFICATION:
		app->setSetting(P6VXApp::keyFixMagnification,
						!app->getSetting(P6VXApp::keyFixMagnification).toBool());
		break;
	case ID_HWACCEL:
		if(OSD_Message(GetWindowHandle(), QtEL6::tr("設定を反映するには一度終了しますがよろしいですか?").toStdString(), GetText(T_QUITC), OSDM_OK | OSDM_OKCANCEL) == OSDR_OK){
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
	QAction* selectedAction = nullptr;
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
				IsMonitor() || vm->bp->GetNum() ||
			#endif
				( REPLAY::GetStatus() == ST_REPLAYREC ) )){
		addCommand(replayMenu, (REPLAY::GetStatus() == ST_REPLAYPLAY) ? tr("再生停止"): tr("再生..."), ID_REPLAYPLAY);
	}
	// モニタモード or ブレークポインタが設定されている
	// またはリプレイ再生中だったらリプレイ記録無効
	if(!(
			#ifndef NOMONITOR
				IsMonitor() || vm->bp->GetNum() ||
			#endif
				( REPLAY::GetStatus() == ST_REPLAYPLAY ) )){
		addCommand(replayMenu, (REPLAY::GetStatus() == ST_REPLAYREC) ? tr("記録停止") : tr("記録..."), ID_REPLAYSAVE);
	}
	if (REPLAY::GetStatus() == ST_IDLE){
		addCommand(replayMenu, tr("記録再開..."), ID_REPLAYRESUME);
#ifndef NOAVI
		addCommand(replayMenu, tr("リプレイを動画に変換..."), ID_REPLAYMOVIE);
#endif
	}
	if (REPLAY::GetStatus() == ST_REPLAYREC){
		addCommand(replayMenu, tr("途中保存"), ID_REPLAYDOKOSAVE);
		addCommand(replayMenu, tr("途中保存から再開"), ID_REPLAYDOKOLOAD);
		addCommand(replayMenu, tr("1つ前の途中保存から再開"), ID_REPLAYROLLBACK);
	}
	systemMenu->addSeparator();

#ifndef NOAVI
	// ビデオキャプチャ
	addCommand(systemMenu, AVI6::IsAVI() ?
				   tr("ビデオキャプチャ停止") : tr("ビデオキャプチャ..."), ID_AVISAVE);
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
	if(P6VPATH2QSTR(vm->cmtl->GetFile()).isEmpty()) tapeEject->setEnabled(false);
	QAction* tapeExport = addCommand(tapeMenu, tr("TAPE(SAVE)をエクスポート..."), ID_TAPEEXPORT);

	// DISKメニュー
	if (vm->disk->GetDrives()){
		QMenu* diskMenu = menu.addMenu(tr("DISK"));
		for (int i = 0; i < vm->disk->GetDrives(); i++){
			QString item = QString("Drive%1").arg(i + 1);
			QMenu* driveMenu = diskMenu->addMenu(item);
			addCommand(driveMenu, tr("挿入..."), MenuCommand(ID_DISKINSERT1 + i));
			QAction* diskEject = addCommand(driveMenu, tr("取出"), MenuCommand(ID_DISKEJECT1 + i));
			if (P6VPATH2QSTR(vm->disk->GetFile(i)).isEmpty()){
				diskEject->setEnabled(false);
			}
		}
	}

	// 拡張カートリッジメニュー
	QMenu* extRomMenu = menu.addMenu(tr("拡張カートリッジ"));
	QAction* romEject = addCommand(extRomMenu, tr("なし"), ID_CARTEJECT, true);
	extRomMenu->addSeparator();
	QAction* romC6005 = addCommand(extRomMenu, tr("PC-6005    ROMカートリッジ..."), ID_C6005, true);
	QMenu* extRomSubMenu = extRomMenu->addMenu(tr("PC-6006    拡張ROM/RAMカートリッジ"));
	QAction* romC6006 = addCommand(extRomSubMenu, tr("ROM選択..."), ID_C6006, true);
	QAction* romC6006NR = addCommand(extRomSubMenu, tr("ROMなし"), ID_C6006NR, true);
	extRomMenu->addSeparator();
	QAction* romC6001 = addCommand(extRomMenu, tr("PCS-6001R  拡張BASIC"), ID_C6001, true);
	QAction* romC660101 = addCommand(extRomMenu, tr("PC-6601-01 拡張漢字ROMカートリッジ"), ID_C660101, true);
	QAction* romC6006SR = addCommand(extRomMenu, tr("PC-6006SR  拡張64KRAMカートリッジ"), ID_C6006SR, true);
	QAction* romC6007SR = addCommand(extRomMenu, tr("PC-6007SR  拡張漢字ROM&&RAMカートリッジ"), ID_C6007SR, true);
	extRomMenu->addSeparator();
	// ボイスシンセサイザ,FM音源カートリッジは当面封印
	// QAction* romC6053 = addCommand(extRomMenu, tr("PC-6053    ボイスシンセサイザー"), ID_C6053, true);
	// QAction* romC60M55 = addCommand(extRomMenu, tr("PC-60m55   FM音源カートリッジ"), ID_C60M55, true);
	// extRomMenu->addSeparator();
	QMenu* extRomWarrior1SubMenu = extRomMenu->addMenu(tr("戦士のカートリッジ"));
	QAction* romCSOL1 = addCommand(extRomWarrior1SubMenu, tr("ROM選択..."), ID_CSOL1, true);
	QAction* romCSOL1NR = addCommand(extRomWarrior1SubMenu, tr("ROMなし"), ID_CSOL1NR, true);
	QMenu* extRomWarrior2SubMenu = extRomMenu->addMenu(tr("戦士のカートリッジmkⅡ"));
	QAction* romCSOL2 = addCommand(extRomWarrior2SubMenu, tr("ROM選択..."), ID_CSOL2, true);
	QAction* romCSOL2NR = addCommand(extRomWarrior2SubMenu, tr("ROMなし"), ID_CSOL2NR, true);
	QMenu* extRomWarrior3SubMenu = extRomMenu->addMenu(tr("戦士のカートリッジmkⅢ"));
	QAction* romCSOL3 = addCommand(extRomWarrior3SubMenu, tr("ROM選択..."), ID_CSOL3, true);
	QAction* romCSOL3NR = addCommand(extRomWarrior3SubMenu, tr("ROMなし"), ID_CSOL3NR, true);

	switch( vm->mem->GetCartridge() ){
	case EXC6001:	romC6001->setChecked(true);														break;
	case EXC6005:	romC6005->setChecked(true);														break;
	case EXC6006:	(cfg->GetValue( CF_ExtRom ).empty() ? romC6006NR : romC6006)->setChecked(true);	break;
	case EXC660101:	romC660101->setChecked(true);													break;
	case EXC6006SR:	romC6006SR->setChecked(true);													break;
	case EXC6007SR:	romC6007SR->setChecked(true);													break;
	// case EXC6053:	romC660101->setChecked(true);													break;
	// case EXC60M55:	romC660101->setChecked(true);													break;
	case EXCSOL1:	(cfg->GetValue( CF_ExtRom ).empty() ? romCSOL1NR : romCSOL1)->setChecked(true);	break;
	case EXCSOL2:	(cfg->GetValue( CF_ExtRom ).empty() ? romCSOL2NR : romCSOL2)->setChecked(true);	break;
	case EXCSOL3:	(cfg->GetValue( CF_ExtRom ).empty() ? romCSOL3NR : romCSOL3)->setChecked(true);	break;
	default:		romEject->setChecked(true);
	}

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
			QAction* joyAction1 = addCommand(joyMenu1, QString::fromStdString(OSD_GetJoyName( i )), MenuCommand(ID_JOY101 + i), true);
			QAction* joyAction2 = addCommand(joyMenu2, QString::fromStdString(OSD_GetJoyName( i )), MenuCommand(ID_JOY201 + i), true);
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
	QAction* fullScreen = addCommand(settingsMenu, tr("フルスクリーン"), ID_FULLSCRN, true);
	if (cfg->GetValue(CB_FullScreen)) fullScreen->setChecked(true);
#endif
	QAction* statusBar = addCommand(settingsMenu, tr("ステータスバー"), ID_STATUS, true);
	if (cfg->GetValue(CB_DispStatus)) statusBar->setChecked(true);
	QAction* disp43 = addCommand(settingsMenu, tr("4:3表示"), ID_DISP43, true);
	if (cfg->GetValue(CB_DispNTSC)) disp43->setChecked(true);
	QAction* scanLine = addCommand(settingsMenu, tr("スキャンライン"), ID_SCANLINE, true);
	if (cfg->GetValue(CB_ScanLine)) scanLine->setChecked(true);
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
						   << "0 (59.92fps)"
						   << "1 (29.96fps)"
						   << "2 (19.97fps)"
						   << "3 (14.98fps)"
						   << "4 (11.98fps)"
						   << "5 (9.99fps)");
	for( int i = 0; i < fpsList.size(); i++ ){
		QAction* fps = addCommand(fpsMenu, fpsList[i], MenuCommand(ID_FSKP0 + i), true);
		if (cfg->GetValue(CV_FrameSkip) == i) fps->setChecked(true);
	}

	QAction* noWait = addCommand(settingsMenu, tr("ウェイト無効"), ID_NOWAIT, true);
	if (!sche->GetWaitEnable()) noWait->setChecked(true);
	QAction* turboTape = addCommand(settingsMenu, tr("Turbo TAPE"), ID_TURBO, true);
	if (cfg->GetValue(CB_TurboTAPE)) turboTape->setChecked(true);
	QAction* boostUp =  addCommand(settingsMenu, tr("Boost Up"), ID_BOOST, true);
	if (vm->cmtl->IsBoostUp()) boostUp->setChecked(true);
	addCommand(settingsMenu, tr("環境設定..."), ID_CONFIG);

#ifndef NOMONITOR
	// デバッグメニュー
	QMenu* debugMenu = menu.addMenu(tr("デバッグ"));
	menu.addSeparator();
	QAction* monitorMode =  addCommand(debugMenu, tr("モニタモード"), ID_MONITOR, true);
	if ( vm->IsMonitor() ) monitorMode->setChecked(true);
#endif

	// ヘルプメニュー
	QMenu* helpMenu = menu.addMenu(tr("ヘルプ"));
	addCommand(helpMenu, tr("オンラインヘルプ"), ID_ONLINEHELP);
	addCommand(helpMenu, tr("バージョン情報..."), ID_VERSION);
	addCommand(helpMenu, tr("About Qt..."), ID_ABOUTQT);
	addCommand(helpMenu, tr("システム情報..."), ID_SYSINFO);
	addCommand(helpMenu, tr("設定初期化..."), ID_RESETSETTINGS);

	selectedAction = menu.exec(QPoint(x,y));

	if (selectedAction != nullptr) {
		// 項目ごとの処理
		MenuCommand id = selectedAction->property(MENUIDPROPERTY).value<MenuCommand>();
		ExecMenu(id);
	}
}



void QtEL6::UpdateFPS()
{
	Event ev;
	ev.type = EV_FPSUPDATE;
	OSD_PushEvent( ev );
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
//        ecfg = nullptr;
//        return -1;
//    }

//    ConfigDialog dialog(ecfg);
//    dialog.exec();
//    int ret = dialog.result();
//    // OKボタンが押されたならINIファイル書込み
//    if( ret == QDialog::Accepted) ecfg->Write();

//    delete ecfg;
//    ecfg = nullptr;

//    return ret;
//}

bool QtEL6::GetPauseEnable()
{
	if(sche){
		return sche->GetPauseEnable();
	}
	return false;
}

void QtEL6::SetPauseEnable(bool en)
{
	if(sche){
		sche->SetPauseEnable(en);
	}
}

std::shared_ptr<KEY6> QtEL6::GetKeyboard()
{
	return vm->key;
}

bool QtEL6::IsMonitor()
{
	return vm->IsMonitor();
}

void QtEL6::SetPaletteTable(QVector<QRgb> &palette, int scanbr)
{
	// パレットテーブルを設定
	palette.clear();
	palette.resize(256);
	float brRate = float(scanbr) / 100;
	for (int i=0; i < 256; i++){
		DWORD DWCOL = GetBackBuffer()->GetColor(i);
		COLOR24 col = {BYTE(DWCOL >> RSHIFT32),
					   BYTE(DWCOL >> GSHIFT32),
					   BYTE(DWCOL >> BSHIFT32),
					   BYTE(DWCOL >> ASHIFT32)};
		palette[i] = qRgb(col.r, col.g, col.b);
		// スキャンライン用のパレットは元の色+64のインデックスに格納する。
		if (i>=64 && i<128) {
			DWORD DWCOL = GetBackBuffer()->GetColor(i-64);
			COLOR24 col = {BYTE(DWCOL >> RSHIFT32),
						   BYTE(DWCOL >> GSHIFT32),
						   BYTE(DWCOL >> BSHIFT32),
						   BYTE(DWCOL >> ASHIFT32)};
			palette[i] = qRgb(brRate * col.r,
							  brRate * col.g,
							  brRate * col.b);
		}
	}
}

