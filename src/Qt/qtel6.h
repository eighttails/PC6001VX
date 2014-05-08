#ifndef EL6EX_H
#define EL6EX_H

#include <QObject>
#include "../p6el.h"

class QtEL6 : public QObject, public EL6
{
    Q_OBJECT
public:
    QtEL6();
    void ShowPopupImpl(int x, int y);
    bool GetPauseEnable();

    virtual bool Start();		// 動作開始
    virtual void Stop();		// 動作停止

public slots:
    void updateFPS();
private:
    static int Speed;           // 停止時の速度退避用
};

#endif // EL6EX_H
