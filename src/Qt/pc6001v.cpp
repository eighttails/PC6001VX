#include <stdlib.h>
#include <errno.h>
#include <QFrame>
#include <QTextCodec>
#include <QTimer>
#include <QDir>

#include "qtp6vxapplication.h"

#ifndef NOJOYSTICK
//リンクエラー解消のために入れておく必要がある
#include <SDL2/SDL.h>
#undef main
#endif

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
