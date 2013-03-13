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

//SDLMainとのコンフリクト解消
#undef main

#ifdef SDL_VIDEO_DRIVER_X11
#include <X11/Xlib.h>
#endif




///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
#ifdef SDL_VIDEO_DRIVER_X11
    //X11環境でSDLスレッドを使用するために必要。
    XInitThreads();
#endif

    QtP6VXApplication app(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


	// 環境変数設定(テスト用)
//	putenv( "SDL_VIDEODRIVER=windib" );
//	putenv( "SDL_VIDEODRIVER=directx" );
//	putenv( "SDL_AUDIODRIVER=waveout" );
//	putenv( "SDL_AUDIODRIVER=dsound" );
	

    setlocale(LC_ALL,"Japanese");

#ifdef WIN32
    // Windowsではアプリに同梱のqjpcodecs4.dllプラグインを読み込むため、そのパスを指定
    QApplication::addLibraryPath(qApp->applicationDirPath() + QDir::separator() + "plugins");
#endif
	

    QTimer::singleShot(500, &app, SLOT(startup()));
    app.exec();
	
	
	return true;
}
