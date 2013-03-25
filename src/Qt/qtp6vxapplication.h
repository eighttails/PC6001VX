#ifndef QTP6VXAPPLICATION_H
#define QTP6VXAPPLICATION_H

#include <QtSingleApplication>
#include <QImage>

#include "../typedef.h"
#include "../p6vm.h"
#include "../config.h"
#include "qtel6.h"
#include "emulationadaptor.h"

class QtP6VXApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit QtP6VXApplication(int &argc, char **argv);
    virtual ~QtP6VXApplication();

    virtual bool notify(QObject *receiver, QEvent *event);

public slots:
    //仮想マシンの起動→終了→再起動のループ
    //直接呼び出さずに、Qtのイベントループの中で実行する
    void startup();

    //ウィンドウを生成
    void createWindow(HWINDOW Wh, int w, int h, int bpp, bool fsflag );

    //グラフィックをシーンに配置
    //QGraphicsSceneの操作はメインスレッドでしかできないため、
    //ここで実装する
    void layoutBitmap(HWINDOW Wh, int x, int y, double aspect, QImage image);

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
    //仮想マシンを開始させる
    void executeEmulation();
    //仮想マシン終了後の処理
    void postExecuteEmulation();

    //仮想マシンを終了させる
    void terminateEmulation();

private:
    QtEL6 *P6Core;    			// オブジェクトポインタ
    EL6::ReturnCode Restart;	// 再起動フラグ
    CFG6 Cfg;					// 環境設定オブジェクト
    EmulationAdaptor* Adaptor;  // P6Coreにシグナル・スロットを付加するアダプタ
};

#endif // QTP6VXAPPLICATION_H
