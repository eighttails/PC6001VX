#include <stdlib.h>
#include <errno.h>
#include <QtWidgets>

#include "qtp6vxapplication.h"

#ifndef NOJOYSTICK
//SDL使用時にビルドを通すのに必要
#undef main
#endif

///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
	//X11の場合用
	QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

	QtP6VXApplication app(argc, argv);

	QCommandLineParser parser;
	QCommandLineOption forceOption(QStringList() << "s" << "safemode", "Safe Mode");
	parser.addOption(forceOption);
	parser.process(app);
	bool safeMode = parser.isSet(forceOption);
	app.enableSafeMode(safeMode);

	QLocale locale;
    QString lang = locale.uiLanguages()[0];
    QTranslator myappTranslator;

    //表示言語が日本語でない場合は英語リソースを読み込む
    if(lang != "ja-JP"){
        myappTranslator.load(":/translation/PC6001VX_en");
        app.installTranslator(&myappTranslator);
    }
    //setlocale(LC_ALL,"Japanese");

    //イベントループが始まったらQtP6VXApplication::startup()を実行
    QMetaObject::invokeMethod(&app, "startup", Qt::QueuedConnection);

    //イベントループを開始
    app.exec();
	
	return true;
}
