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
	//X11の場合用
	QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
#if QT_VERSION >= 0x050700
#ifndef ANDROID
	// AndroidではAA_EnableHighDpiScalingを設定するとメニューの座標がおかしくなる
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
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
		QFontDatabase database;
		auto list = database.families(QFontDatabase::Japanese);
		for (auto& f : list){
			// 日本語で検索して最初に見つかったフォントを使う
			auto font = QFont(f);
			qDebug() << "using " << f;
			app.setFont(font);
			break;
		}
#endif
	}

	//イベントループが始まったらp6vxapp::startup()を実行
	QMetaObject::invokeMethod(&app, "startup", Qt::QueuedConnection);

	//イベントループを開始
	return app.exec();
}
