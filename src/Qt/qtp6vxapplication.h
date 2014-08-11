#ifndef QTP6VXAPPLICATION_H
#define QTP6VXAPPLICATION_H

#include <QtSingleApplication>
#include <QImage>
#include <QMutex>
#include <QPointer>
#include <QSettings>

#include "../typedef.h"
#include "../p6vm.h"
#include "../config.h"
#include "../common.h"
#include "qtel6.h"
#include "emulationadaptor.h"

#ifdef NOSINGLEAPP
typedef QApplication ParentAppClass;
#else
typedef QtSingleApplication ParentAppClass;
#endif

class QKeyEvent;

class QtP6VXApplication : public ParentAppClass
{
    Q_OBJECT
public:
	//設定用キー
	static const QString keyGeometry;			// ウィンドウ表示位置
	static const QString keyMaximized;			// ウィンドウ最大化フラグ
	static const QString keyHwAccel;			// ハードウェアアクセラレーション(OpenGL/DirectX)有効化フラグ
	static const QString keyFiltering;			// 画面拡大縮小時のフィルタリング有効化フラグ
	static const QString keyFixMagnification;	// 表示倍率固定フラグ
	static const QString keyMagnification;		// 表示倍率

	explicit QtP6VXApplication(int &argc, char **argv);
    virtual ~QtP6VXApplication();

    virtual bool notify(QObject *receiver, QEvent *event);

	// P6VX固有の設定
	static const QVariant getSetting(const QString& key);
	static void setSetting(const QString& key, const QVariant& value);
	static void setDefaultSetting(const QString &key, const QVariant &value);

    // TILT関連
    bool isTiltEnabled();
    void enableTilt(bool enable);
    TiltDirection getTiltDirection();
    void setTiltDirection(TiltDirection dir);
	int getTiltStep();
	void setTiltStep(int step);

	// セーフモード(一時的にハードウェアアクセラレーションを無効化する)
	bool isSafeMode();
	void enableSafeMode(bool enable);

	// デバッグ用に一時的にROMパスを切り替える(主にAndroid用)
	QString getCustomRomPath();
	void setCustomRomPath(QString path);

public slots:
    //仮想マシンの起動→終了→再起動のループ
    //直接呼び出さずに、Qtのイベントループの中で実行する
    void startup();

    //ウィンドウを生成
    void createWindow(HWINDOW Wh, bool fsflag );

    //グラフィックをシーンに配置
    //QGraphicsSceneの操作はメインスレッドでしかできないため、
    //ここで実装する
    void layoutBitmap(HWINDOW Wh, int x, int y, double scaleX, double scaleY, QImage image);

    //ウィンドウイメージバイト列でを取得
    //QGraphicsSceneの操作はメインスレッドでしかできないため、
    //ここで実装する
    void getWindowImage(HWINDOW Wh, QRect pos, void** pixels);

    //グラフィックシーンをクリア
    void clearLayout(HWINDOW Wh);

    //コンテキストメニューを表示
    void showPopupMenu(int x, int y);

signals:
    //初期化終了シグナル
    void initialized();

    //仮想マシン実行前準備終了シグナル
    void vmPrepared();

    // 仮想マシンの実行終了シグナル
    void vmRestart();
    
private slots:
    //仮想マシン開始させる
    void executeEmulation();
    //仮想マシン終了後の処理
    void postExecuteEmulation();

    //仮想マシンを終了させる
    void terminateEmulation();

protected:
	// 特殊キー対策
	void handleSpecialKeys(QKeyEvent* ke, int keyCode);
private:
    QPointer<QtEL6> P6Core;		// オブジェクトポインタ
    EL6::ReturnCode Restart;	// 再起動フラグ
    CFG6 Cfg;					// 環境設定オブジェクト
    EmulationAdaptor* Adaptor;  // P6Coreにシグナル・スロットを付加するアダプタ
	QMutex PropretyMutex;       // 属性値保護のためのMutex
    QMutex MenuMutex;           // メニュー表示中にロックされるMutex

	// P6VX固有の設定
	static QMutex SettingMutex; // 設定読み書き用Mutex
	bool TiltEnabled;
    TiltDirection TiltDir;
	int TiltStep;

	bool SafeMode;

	QString CustomRomPath;
};

#endif // QTP6VXAPPLICATION_H
