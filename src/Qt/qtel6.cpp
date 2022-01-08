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

// --- メッセージ配列 ---
// 一般メッセージ
extern const char *MsgStr[];
#define	MSG_QUIT			MsgStr[0]	// "終了してよろしいですか?"
#define	MSG_QUITC			MsgStr[1]	// "終了確認"
#define	MSG_RESTART0		MsgStr[2]	// "再起動してよろしいですか?"
#define	MSG_RESTART			MsgStr[3]	// "変更は再起動後に有効となります。\n今すぐ再起動しますか?"
#define	MSG_RESTARTC		MsgStr[4]	// "再起動確認"
#define	MSG_RESETI			MsgStr[5]	// "拡張ROMを挿入してリセットします。"
#define	MSG_RESETE			MsgStr[6]	// "拡張ROMを排出してリセットします。"
#define	MSG_RESETC			MsgStr[7]	// "リセット確認"


// メニュー用メッセージ ------
extern const char *MsgMen[];
// [システム]
#define	MSMEN_AVI0			MsgMen[0]	// "ビデオキャプチャ..."
#define	MSMEN_AVI1			MsgMen[1]	// "ビデオキャプチャ停止"
#define	MSMEN_REP0			MsgMen[2]	// "記録..."  (リプレイ)
#define	MSMEN_REP1			MsgMen[3]	// "記録停止" (リプレイ)
#define	MSMEN_REP2			MsgMen[4]	// "再生..."  (リプレイ)
#define	MSMEN_REP3			MsgMen[5]	// "再生停止" (リプレイ)


// INIファイル用メッセージ ------
extern const char *MsgIni[];
// [CONFIG]
#define	MSINI_TITLE			MsgIni[0]	// "; === PC6001V 初期設定ファイル ===\n\n"
#define	MSINI_Model			MsgIni[1]	// " 機種 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR"
#define	MSINI_FDD			MsgIni[2]	// " FDD接続台数 (0-2)"
#define	MSINI_ExtRam		MsgIni[3]	// " 拡張RAM使用"
#define	MSINI_TurboTAPE		MsgIni[4]	// " Turbo TAPE Yes:有効 No:無効"
#define	MSINI_BoostUp		MsgIni[5]	// " BoostUp Yes:有効 No:無効"
#define	MSINI_MaxBoost60	MsgIni[6]	// " BoostUp 最大倍率(N60モード)
#define	MSINI_MaxBoost62	MsgIni[7]	// " BoostUp 最大倍率(N60m/N66モード)
#define	MSINI_OverClock		MsgIni[8]	// " オーバークロック率 (1-1000)%"
#define	MSINI_CheckCRC		MsgIni[9]	// " CRCチェック Yes:有効 No:無効"
#define	MSINI_RomPatch		MsgIni[10]	// " ROMパッチ Yes:あてる No:あてない"
#define	MSINI_FDDWait		MsgIni[11]	// " FDDウェイト Yes:有効 No:無効"
// [DISPLAY]
#define	MSINI_Mode4Color	MsgIni[12]	// " MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク"
#define	MSINI_ScanLine		MsgIni[13]	// " スキャンライン Yes:あり No:なし"
#define	MSINI_ScanLineBr	MsgIni[14]	// " スキャンライン輝度 (0-100)%"
#define	MSINI_DispNTSC		MsgIni[15]	// " 4:3表示 Yes:有効 No:無効"
#define	MSINI_FullScreen	MsgIni[16]	// " フルスクリーンモード Yes:有効 No:無効"
#define	MSINI_DispStatus	MsgIni[17]	// " ステータスバー Yes:表示 No:非表示"
#define	MSINI_FrameSkip		MsgIni[18]	// " フレームスキップ"
// [SOUND]
#define	MSINI_SampleRate	MsgIni[19]	// " サンプリングレート (44100/22050/11025)Hz"
#define	MSINI_SoundBuffer	MsgIni[20]	// " サウンドバッファサイズ"
#define	MSINI_MasterVolume	MsgIni[21]	// " マスター音量 (0-100)"
#define	MSINI_PsgVolume		MsgIni[22]	// " PSG音量 (0-100)"
#define	MSINI_VoiceVolume	MsgIni[23]	// " 音声合成音量 (0-100)"
#define	MSINI_TapeVolume	MsgIni[24]	// " TAPEモニタ音量 (0-100)"
#define	MSINI_PsgLPF		MsgIni[25]	// " PSG LPFカットオフ周波数(0で無効)"
// [MOVIE]
#define	MSINI_AviBpp		MsgIni[26]	// " ビデオキャプチャ色深度 (16,24,32)"
// [FILES]
#define	MSINI_ExtRom		MsgIni[27]	// " 拡張ROMファイル名"
#define	MSINI_tape			MsgIni[28]	// " TAPE(LODE)ファイル名(起動時に自動マウント)"
#define	MSINI_save			MsgIni[29]	// " TAPE(SAVE)ファイル名(SAVE時に自動マウント)"
#define	MSINI_disk1			MsgIni[30]	// " DISK1ファイル名(起動時に自動マウント)"
#define	MSINI_disk2			MsgIni[31]	// " DISK2ファイル名(起動時に自動マウント)"
#define	MSINI_printer		MsgIni[32]	// " プリンタ出力ファイル名"
#define	MSINI_fontz			MsgIni[33]	// " 全角フォントファイル名"
#define	MSINI_fonth			MsgIni[34]	// " 半角フォントファイル名"
// [PATH]
#define	MSINI_RomPath		MsgIni[35]	// " ROMイメージ格納パス"
#define	MSINI_TapePath		MsgIni[36]	// " TAPEイメージ格納パス"
#define	MSINI_DiskPath		MsgIni[37]	// " DISKイメージ格納パス"
#define	MSINI_ExtRomPath	MsgIni[38]	// " 拡張ROMイメージ格納パス"
#define	MSINI_ImgPath		MsgIni[39]	// " スナップショット格納パス"
#define	MSINI_WavePath		MsgIni[40]	// " WAVEファイル格納パス"
#define	MSINI_FontPath		MsgIni[41]	// " FONT格納パス"
#define	MSINI_DokoSavePath	MsgIni[42]	// " どこでもSAVE格納パス"
// [CHECK]
#define	MSINI_CkQuit		MsgIni[43]	// " 終了時確認 Yes:する No:しない"
#define	MSINI_SaveQuit		MsgIni[44]	// " 終了時INIファイルを保存 Yes:する No:しない"
// [KEY]
#define	MSINI_KeyRepeat		MsgIni[45]	// " キーリピートの間隔(単位:ms 0で無効)"
// [OPTION]
#define	MSINI_UseSoldier	MsgIni[46]	// " 戦士のカートリッジ Yes:有効 No:無効"


// どこでもSAVE用メッセージ ------
extern const char *MsgDok[];
#define	MSDOKO_TITLE		MsgDok[0]	// "; === PC6001V どこでもSAVEファイル ===\n\n"


// Error用メッセージ ------
extern const char *MsgErr[];
#define	MSERR_ERROR				MsgErr[0]	// "Error"
#define	MSERR_NoError			MsgErr[1]	// "エラーはありません"
#define	MSERR_Unknown			MsgErr[2]	// "原因不明のエラーが発生しました"
#define	MSERR_MemAllocFailed	MsgErr[3]	// "メモリの確保に失敗しました"
#define	MSERR_RomChange			MsgErr[4]	// "指定された機種のROMイメージが見つからないため機種を変更しました\n設定を確認してください"
#define	MSERR_NoRom				MsgErr[5]	// "ROMイメージが見つかりません\n設定とファイル名を確認してください"
#define	MSERR_RomSizeNG			MsgErr[6]	// "ROMイメージのサイズが不正です"
#define	MSERR_RomCrcNG			MsgErr[7]	// "ROMイメージのCRCが不正です"
#define	MSERR_LibInitFailed		MsgErr[8]	// "ライブラリの初期化に失敗しました"
#define	MSERR_InitFailed		MsgErr[9]	// "初期化に失敗しました\n設定を確認してください"
#define	MSERR_FontLoadFailed	MsgErr[10]	// "フォントの読込みに失敗しました"
#define	MSERR_FontCreateFailed	MsgErr[11]	// "フォントファイルの作成に失敗しました"
#define	MSERR_IniDefault		MsgErr[12]	// "INIファイルの読込みに失敗しました\nデフォルト設定で起動します"
#define	MSERR_IniReadFailed		MsgErr[13]	// "INIファイルの読込みに失敗しました"
#define	MSERR_IniWriteFailed	MsgErr[14]	// "INIファイルの保存に失敗しました"
#define	MSERR_TapeMountFailed	MsgErr[15]	// "TAPEイメージのマウントに失敗しました"
#define	MSERR_DiskMountFailed	MsgErr[16]	// "DISKイメージのマウントに失敗しました"
#define	MSERR_ExtRomMountFailed	MsgErr[17]	// "拡張ROMイメージのマウントに失敗しました"
#define	MSERR_DokoReadFailed	MsgErr[18]	// "どこでもLOADに失敗しました"
#define	MSERR_DokoWriteFailed	MsgErr[19]	// "どこでもSAVEに失敗しました"
#define	MSERR_DokoDiffVersion	MsgErr[20]	// "どこでもLOADに失敗しました\n保存時とPC6001Vのバージョンが異なります"
#define	MSERR_ReplayPlayError	MsgErr[21]	// "リプレイ再生に失敗しました"
#define	MSERR_ReplayRecError	MsgErr[22]	// "リプレイ記録に失敗しました"
#define	MSERR_NoReplayData		MsgErr[23]	// "リプレイデータがありません"



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
		//#TODO case ID_ROMINSERT:		UI_CartInsert();							break;	// 拡張ROM 挿入
		//#TODO ROM挿入系
	case ID_CARTEJECT:		UI_CartEject();							break;	// 拡張ROM 排出
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
		addCommand(replayMenu, (REPLAY::GetStatus() == ST_REPLAYPLAY) ? MSMEN_REP3: MSMEN_REP2, ID_REPLAYPLAY);
	}
	// モニタモード or ブレークポインタが設定されている
	// またはリプレイ再生中だったらリプレイ記録無効
	if(!(
			#ifndef NOMONITOR
				IsMonitor() || vm->bp->GetNum() ||
			#endif
				( REPLAY::GetStatus() == ST_REPLAYPLAY ) )){
		addCommand(replayMenu, (REPLAY::GetStatus() == ST_REPLAYREC) ? MSMEN_REP1 : MSMEN_REP0, ID_REPLAYSAVE);
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
	if(!P6VPATH2QSTR(vm->cmtl->GetFile()).isEmpty()) tapeEject->setEnabled(false);
	QAction* tapeExport = addCommand(tapeMenu, tr("TAPE(SAVE)をエクスポート..."), ID_TAPEEXPORT);

	// DISKメニュー
	if (vm->disk->GetDrives()){
		QMenu* diskMenu = menu.addMenu(tr("DISK"));
		for (int i = 0; i < vm->disk->GetDrives(); i++){
			QString item = QString("Drive%1").arg(i + 1);
			QMenu* driveMenu = diskMenu->addMenu(item);
			addCommand(driveMenu, tr("挿入..."), MenuCommand(ID_DISKINSERT1 + i));
			QAction* diskEject = addCommand(driveMenu, tr("取出"), MenuCommand(ID_DISKEJECT1 + i));
			if (!P6VPATH2QSTR(vm->disk->GetFile(i)).isEmpty()){
				diskEject->setEnabled(false);
			}
		}
	}

	// 拡張カートリッジメニュー
	QMenu* extRomMenu = menu.addMenu(tr("拡張カートリッジ"));
	QAction* romEject = addCommand(extRomMenu, tr("なし"), ID_CARTEJECT);
	if(!P6VPATH2QSTR(vm->mem->GetFile()).isEmpty()) romEject->setEnabled(false);
	extRomMenu->addSeparator();
	QAction* romC6005 = addCommand(extRomMenu, tr("PC-6005    ROMカートリッジ..."), ID_C6005);
	{
		QMenu* extRomSubMenu = extRomMenu->addMenu(tr("PC-6006    拡張ROM/RAMカートリッジ"));
		QAction* romC6006 = addCommand(extRomSubMenu, tr("ROM選択..."), ID_C6006);
		QAction* romC6006NR = addCommand(extRomSubMenu, tr("ROMなし"), ID_C6006NR);
	}
	extRomMenu->addSeparator();
	QAction* romC6001 = addCommand(extRomMenu, tr("PCS-6001R  拡張BASIC"), ID_C6001);
	QAction* romC660101 = addCommand(extRomMenu, tr("PC-6601-01 拡張漢字ROMカートリッジ"), ID_C660101);
	QAction* romC6006SR = addCommand(extRomMenu, tr("PC-6006SR  拡張64KRAMカートリッジ"), ID_C6006SR);
	QAction* romC6007SR = addCommand(extRomMenu, tr("PC-6007SR  拡張漢字ROM&&RAMカートリッジ"), ID_C6007SR);
	extRomMenu->addSeparator();
	QAction* romC6053 = addCommand(extRomMenu, tr("PC-6053    ボイスシンセサイザー"), ID_C6053);
	QAction* romC60M55 = addCommand(extRomMenu, tr("PC-60m55   FM音源カートリッジ"), ID_C60M55);
	extRomMenu->addSeparator();
	{
		QMenu* extRomSubMenu = extRomMenu->addMenu(tr("戦士のカートリッジ"));
		QAction* romCSOL1 = addCommand(extRomSubMenu, tr("ROM選択..."), ID_CSOL1);
		QAction* romCSOL1NR = addCommand(extRomSubMenu, tr("ROMなし"), ID_CSOL1NR);
	}
	{
		QMenu* extRomSubMenu = extRomMenu->addMenu(tr("戦士のカートリッジmkⅡ"));
		QAction* romCSOL2 = addCommand(extRomSubMenu, tr("ROM選択..."), ID_CSOL2);
		QAction* romCSOL2NR = addCommand(extRomSubMenu, tr("ROMなし"), ID_CSOL2NR);
	}
	{
		QMenu* extRomSubMenu = extRomMenu->addMenu(tr("戦士のカートリッジmkⅢ"));
		QAction* romCSOL3 = addCommand(extRomSubMenu, tr("ROM選択..."), ID_CSOL3);
		QAction* romCSOL3NR = addCommand(extRomSubMenu, tr("ROMなし"), ID_CSOL3NR, true);
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
						   << "0 (60fps)"
						   << "1 (30fps)"
						   << "2 (20fps)"
						   << "3 (15fps)"
						   << "4 (12fps)"
						   << "5 (10fps)");
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
#if 0	//#TODO 後で消す
	Event ev;
	ev.type = EV_FPSUPDATE;
	ev.fps.fps = UDFPSCount;
	OSD_PushEvent( ev );
	UDFPSCount = 0;
#endif
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

