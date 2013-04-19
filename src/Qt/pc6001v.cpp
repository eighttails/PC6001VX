#include <stdlib.h>
#include <errno.h>
#include <QFrame>
#include <QTextCodec>
#include <QTimer>
#include <QDir>

#include "qtp6vxapplication.h"

///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    //X11の場合
    if (QGuiApplication::platformName() == QLatin1String("xcb")){
        QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
    }

    QtP6VXApplication app(argc, argv);

    setlocale(LC_ALL,"Japanese");

    QTimer::singleShot(100, &app, SLOT(startup()));
    app.exec();
	
	return true;
}
