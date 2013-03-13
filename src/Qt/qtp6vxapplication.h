#ifndef QTP6VXAPPLICATION_H
#define QTP6VXAPPLICATION_H

#include <QtSingleApplication>
#include "../p6el.h"
#include "../p6vm.h"
#include "../config.h"
#include "emulationadaptor.h"

class QtP6VXApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit QtP6VXApplication(int &argc, char **argv);
    
    virtual bool notify(QObject *receiver, QEvent *event);

public slots:
    //仮想マシンの起動→終了→再起動のループ
    //直接呼び出さずに、Qtのイベントループの中で実行する
    void startup();

signals:
    //前準備終了シグナル
    void prepared();

    // 仮想マシンの実行終了シグナル
    void vmRestart();
    
private slots:
    //仮想マシンを開始させる
    void executeEmulation();
    //仮想マシン終了後の処理
    void postExecuteEmulation();

private:
    EL6 *P6Core;    			// オブジェクトポインタ
    EL6::ReturnCode Restart;	// 再起動フラグ
    CFG6 Cfg;					// 環境設定オブジェクト
    EmulationAdaptor* Adaptor;  // P6Coreにシグナル・スロットを付加するアダプタ
};

#endif // QTP6VXAPPLICATION_H
