#include <stdlib.h>
#include <errno.h>
#include <QtWidgets>

#include "p6vxapp.h"

#ifndef NOJOYSTICK
//SDL使用時にビルドを通すのに必要
#undef main
#endif

///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
#ifdef PANDORA
	//VALGRIND実行時用の環境変数
	setenv("DISPLAY", ":0.0", 1);
	//GlibのOSと開発環境のバージョン不一致に対する暫定対応
	//setenv("QT_NO_GLIB", "0", 1);
	//EGLFS対応
	setenv("EGLFS_X11_SIZE", "800x480", 1);
	setenv("EGLFS_X11_FULLSCREEN", "1", 1);
	setenv("QT_QPA_EGLFS_DEPTH", "16", 1);
	setenv("QT_QPA_EGLFS_PHYSICAL_WIDTH", "200", 1);
	setenv("QT_QPA_EGLFS_PHYSICAL_HEIGHT", "150", 1);
#endif

	//X11の場合用
	QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
#if QT_VERSION >= 0x050700
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	P6VXApp app(argc, argv);

	QCommandLineParser parser;
	QCommandLineOption safeModeOption(QStringList() << "s" << "safemode", "Safe Mode");
	parser.addOption(safeModeOption);
	parser.process(app);
	bool safeMode = parser.isSet(safeModeOption);
	app.enableSafeMode(safeMode);

#ifdef ANDROID
	// AndroidではFusion以外のスタイルでは表示が崩れるので明示的に指定する。
	app.setStyle(QStyleFactory::create("Fusion"));
	app.setCustomRomPath(CUSTOMROMPATH);

#endif

	QLocale locale;
	QString lang = locale.uiLanguages()[0];
	QTranslator myappTranslator;

	//表示言語が日本語でない場合は英語リソースを読み込む
	if(lang != "ja-JP" && lang != "ja"){
		qDebug() << "LANG = " << lang;
		myappTranslator.load(":/translation/PC6001VX_en");
		app.installTranslator(&myappTranslator);
	} else {
#ifdef ANDROID
		QFont font = QFont("MotoyaLMaru");
		if(font.exactMatch()){
			app.setFont(font);
		}
#endif
	}

	//イベントループが始まったらp6vxapp::startup()を実行
	QMetaObject::invokeMethod(&app, "startup", Qt::QueuedConnection);

	//イベントループを開始
	return app.exec();
}
