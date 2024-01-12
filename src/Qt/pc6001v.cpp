#include <cstdlib>
#include <errno.h>

#include <QtWidgets>

#include "p6vxapp.h"

#ifndef NOJOYSTICK
//SDL使用時にビルドを通すのに必要
#undef main
#endif // NOJOYSTICK

///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
	P6VXApp app(argc, argv);

	QCommandLineParser parser;
	QCommandLineOption safeModeOption(QStringList() << "s" << "safemode", "Safe Mode");
	parser.addOption(safeModeOption);
	parser.process(app);
	bool safeMode = parser.isSet(safeModeOption);
	app.enableSafeMode(safeMode);

	QLocale locale;
	QTranslator myappTranslator;

	// 表示言語が日本語でない場合はまずシステムの言語リソースを読み込み、
	// 見つからない場合は英語リソースを読み込む
	if (locale.language() != QLocale::Japanese) {
		auto lang = QLocale::languageToCode(QLocale::system().language());
		qDebug() << "Lang Name = " << lang;
		if (myappTranslator.load(QString(":/translation/PC6001VX_" + lang))) {
			app.installTranslator(&myappTranslator);
		} else if (myappTranslator.load(":/translation/PC6001VX_en")) {
			app.installTranslator(&myappTranslator);
		}
	} else {
#ifdef Q_OS_ANDROID
		// カスタムROMパスを設定する場合(現在は埋め込み互換ROMがあるのでほぼ使わない)
		app.setCustomRomPath(CUSTOMROMPATH);

		// 日本語に対応したフォントを検索
		auto list = QFontDatabase::families(QFontDatabase::Japanese);
		QString family;
		if(!list.isEmpty()){
			for (auto& f : list){
				qDebug() << "listing " << f;

				// 一般に知られているAndroidのデフォルト日本語フォント
				QStringList jpFontList = QStringList()
						<< "Sony Mobile UD Gothic Regular"
						<< "Noto Sans CJK JP"
						<< "MotoyaLMaru";
				for (auto& c : jpFontList){
					if (f == c){
						family = f;
						break;
					}
				}
			}
			if (family.isEmpty()){
				// 既知のフォントが見つからなかった場合は
				// 最初に見つかったフォントを使う
				family = list.first();
			}
			auto font = QApplication::font();
			qDebug() << "using " << family;
			font.setFamily(family);
			app.setFont(font);
		}
#endif
	}

	// イベントループが始まったらp6vxapp::startup()を実行
	QMetaObject::invokeMethod(&app, "startup", Qt::QueuedConnection);

	// 終了処理を予約
	atexit(OSD_Quit);
	HandleQuitSignal();
	
	// イベントループを開始
	return app.exec();


}
