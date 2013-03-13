#ifndef EMULATIONADAPTOR_H
#define EMULATIONADAPTOR_H

#include <QThread>
#include"../p6el.h"

//エミュレーションクラスにQtのシグナル、スロット機能を
//付加するためのアダプタクラス
class EmulationAdaptor : public QThread
{
    Q_OBJECT
public:
    explicit EmulationAdaptor(QObject *parent = 0);
    
    void setEmulationObj(EL6* el){
        P6Core = el;
    }
    EL6::ReturnCode getReturnCode(){return RetCode;}

signals:
public slots:

protected:
    virtual void run();

private:
    EL6* P6Core;
    EL6::ReturnCode RetCode;
};

#endif // EMULATIONADAPTOR_H
