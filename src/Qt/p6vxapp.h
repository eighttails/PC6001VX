#ifndef p6vxapp_H
#define p6vxapp_H

#include <QImage>
#include <QMutex>
#include <QPointer>
#include <QSettings>
#include <QApplication>

#include <memory>

#include "../typedef.h"
#include "../osd.h"
#include "../p6vm.h"
#include "../config.h"
#include "../common.h"
#include "../p6vxcommon.h"
#include "qtel6.h"
#include "emulationadaptor.h"
#ifndef NOSINGLEAPP
#include "singleapplication.h"
#endif

#ifdef NOSINGLEAPP
using ParentAppClass = QApplication;
#else
using ParentAppClass = SingleApplication;
#endif

class QKeyEvent;
class RenderView;
class MainWidget;
class QGraphicsScene;
class KeyPanel;
class VirtualKeyTabWidget;
class KeyStateWatcher;
class QFileDialog;

class P6VXApp : public ParentAppClass
{
	Q_OBJECT
public:
	// 設定用キー
	static const QString keyGeometry;			// ウィンドウ表示位置
	static const QString keyMaximized;			// ウィンドウ最大化フラグ
	static const QString keyHwAccel;			// ハードウェアアクセラレーション(OpenGL/DirectX)有効化フラグ
	static const QString keyFixMagnification;	// 表示倍率固定フラグ
	static const QString keyMagnification;		// 表示倍率
	static const QString keyKeyPanelVisible;		// キーパレット表示有効化
	static const QString keyKeyPanelPosition;		// キーパレット表示位置
	static const QString keyVirtualKeyVisible;		// 仮想キーボード表示有効化
	static const QString keyVirtualKeyTabIndex;		// 仮想キーボードタブインデックス
	static const QString keyVirtualKeyPosition;		// 仮想キーボード表示位置(横画面時)0:右 1:左 2:下 3:非表示
	static const QString keyTimerResolution;		// OSタイマー精度

	explicit P6VXApp(int &argc, char **argv);
	virtual ~P6VXApp();

	// ウィンドウ関連
	RenderView* getView();
	KeyPanel* getKeyPanel();
	VirtualKeyTabWidget *getVirtualKeyboard();

	// 画面設定
	const QVector<QRgb>& getPaletteTable() const;

	// P6VX固有の設定
	bool hasSetting(const QString& key);
	const QVariant getSetting(const QString& key);
	void setSetting(const QString& key, const QVariant& value);
	void setDefaultSetting(const QString &key, const QVariant &value);

	// TILT関連
	bool isTiltEnabled();
	void enableTilt(bool enable);
	TiltDirection getTiltDirection();
	void setTiltDirection(TiltDirection dir);
	int getTiltStep();
	void setTiltStep(int step);

	// プラットフォーム判定
	bool isPlatform(const QString platform);

	// ステータス関係
	bool isAVI();

	// セーフモード(一時的にハードウェアアクセラレーションを無効化する)
	bool isSafeMode();
	void enableSafeMode(bool enable);

	// デバッグ用に一時的にROMパスを切り替える(主にAndroid用)
	QString getCustomRomPath();
	void setCustomRomPath(QString path);

	// 内蔵互換ROM使用モード(リソースファイル内のROMを使う)
	bool isCompatibleRomMode();
	void enableCompatibleRomMode(std::shared_ptr<CFG6> &config, bool enable);

	// SAVEテープをエクスポート
	void exportSavedTape();

public slots:
	// 仮想マシンの起動→終了→再起動のループ
	// 直接呼び出さずに、Qtのイベントループの中で実行する
	void startup();

	// メッセージボックスの表示
	int showMessageBox(void *hwnd, const char *mes, const char *cap, int type);

	// ファイルダイアログの表示
	bool fileDialog( void *hwnd, FileMode mode, const char *title,
							const char *filter, char *fullpath, char *path, const char *ext );
	// フォルダダイアログの表示
	bool folderDialog( void *hwnd, char *Result );

	// ウィンドウを生成
	void createWindow(HWINDOW Wh, bool fsflag );

	// アイコンを設定
	void setWindowIcon(const QIcon &icon);

	// グラフィックをシーンに配置
	// QGraphicsSceneの操作はメインスレッドでしかできないため、
	// ここで実装する
	void layoutBitmap(HWINDOW Wh, int x, int y, double scaleX, double scaleY, QImage image);

	// ウィンドウイメージバイト列でを取得
	// QGraphicsSceneの操作はメインスレッドでしかできないため、
	// ここで実装する
	void getWindowImage(HWINDOW Wh, QRect pos, void *pixels);

	// ウィンドウを前面に移動
	void raiseWidget();

	// グラフィックシーンをクリア
	void clearLayout(HWINDOW Wh);

	// コンテキストメニューを表示
	void showPopupMenu(int x, int y);

	// キーパネルを表示
	void toggleKeyPanel();

	// 仮想キーボードを表示
	void toggleVirtualKeyboard();

	// 設定初期化
	void resetSettings();

signals:
	// 初期化終了シグナル
	void initialized();

	// 仮想マシン実行前準備終了シグナル
	void vmPrepared(EL6::ReturnCode);

	// 仮想マシンの実行終了シグナル
	void vmRestart();

private slots:
	// 仮想マシン開始させる
	void executeEmulation();
	// 仮想マシン終了後の処理
	void postExecuteEmulation();

	// 仮想マシンを終了させる
	void terminateEmulation();

	// スクリーンセーバー無効化
	void inhibitScreenSaver();

	// マウスカーソル関連
	void showMouseCursor();
	void hideMouseCursor();

protected:
	virtual bool notify(QObject *receiver, QEvent *event);

	// 特殊キー対策
	void handleSpecialKeys(QKeyEvent* ke, int &keyCode, Qt::KeyboardModifiers &modifiers);
	// キーコードのモディファイヤー修正
	void finishKeyEvent(Event& ev);

	// プラットフォーム固有の設定で上書き
	void overrideSettings(std::shared_ptr<CFG6> &cfg);

	// ファイル選択ダイアログの生成
	QFileDialog* createFileDialog(void *hwnd);

private:
	QPointer<QtEL6> P6Core;					// オブジェクトポインタ
	QPointer<KeyStateWatcher> KeyWatcher;	// オブジェクトポインタ
	EL6::ReturnCode Restart;				// 再起動フラグ
	std::shared_ptr<CFG6> Cfg;				// 環境設定オブジェクト
	QVector<QRgb> PaletteTable;				// パレットテーブル
	EmulationAdaptor* Adaptor;				// P6Coreにシグナル・スロットを付加するアダプタ
	QMutex PropretyMutex;					// 属性値保護のためのMutex
	QMutex MenuMutex;						// メニュー表示中にロックされるMutex

	// ウィンドウ関連
	QPointer<MainWidget> MWidget;
	QPointer<KeyPanel> KPanel;

	// P6VX固有の設定
	QSettings Setting;
	QMutex SettingMutex; // 設定読み書き用Mutex
	bool TiltEnabled;
	TiltDirection TiltDir;
	int TiltStep;

	bool SafeMode;

	QString CustomRomPath;
};

#endif // p6vxapp_H
