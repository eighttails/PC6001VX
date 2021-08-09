#ifndef EMULATIONADAPTOR_H
#define EMULATIONADAPTOR_H

#include <QObject>
#include"../p6el.h"

//エミュレーションクラスにQtのシグナル、スロット機能を
//付加するためのアダプタクラス
class EmulationAdaptor : public QObject
{
	Q_OBJECT
public:
	explicit EmulationAdaptor(QObject *parent = 0);
	virtual ~EmulationAdaptor(){}

	EL6* getEmulationObj(){return P6Core;}
	void setEmulationObj(EL6* el){
		P6Core = el;
	}

	EL6::ReturnCode getReturnCode(){return RetCode;}

signals:
	// エミュレーションイベントループの終了
	void finished();
public slots:
	// エミュレーションイベントループを開始
    void doEventLoop(EL6::ReturnCode rc);

protected:

private:
	EL6* P6Core;
	EL6::ReturnCode RetCode;
};

#endif // EMULATIONADAPTOR_H
