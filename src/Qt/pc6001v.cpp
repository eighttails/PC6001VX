#include <stdlib.h>
#include <errno.h>
#include <QFrame>
#include <QTextCodec>
#include <QTimer>
#include <QDir>

#ifdef WIN32
//Windowsではプラグインを静的リンクする。
#include <QtPlugin>
Q_IMPORT_PLUGIN(qjpcodecs)
Q_IMPORT_PLUGIN(qico)
#endif

#include "qtp6vxapplication.h"

///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

    QtP6VXApplication app(argc, argv);

    setlocale(LC_ALL,"Japanese");

#ifdef WIN32
    // Windowsではアプリに同梱のqjpcodecs4.dllプラグインを読み込むため、そのパスを指定
    QApplication::addLibraryPath(qApp->applicationDirPath() + QDir::separator() + "plugins");
#endif

    QTimer::singleShot(100, &app, SLOT(startup()));
    app.exec();
	
	return true;
}
