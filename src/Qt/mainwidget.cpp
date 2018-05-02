#include "mainwidget.h"

#include <QVBoxLayout>

#include "p6vxapp.h"
#include "renderview.h"
#include "virtualkeytabwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
	setLayout(layout);

	//シーングラフ生成
	QGraphicsScene* Scene = new QGraphicsScene();
	//アプリケーション終了前にインスタンスを削除(単なる親子関係にすると終了時にクラッシュする)
	QObject::connect(qApp, SIGNAL(aboutToQuit()), Scene, SLOT(deleteLater()));

	//メインウィジェット(エミュレータのメイン画面)
	MainView = new RenderView(Scene);
	layout->addWidget(MainView);
	connect(MainView, SIGNAL(resized(QSize)), this, SLOT(adjustSizeToChild(QSize)));

	//仮想キーウィジェット
	VirtualKeyTabWidget* VKeyWidget = new VirtualKeyTabWidget();
	layout->addWidget(VKeyWidget);

	//TODO 設定に応じて表示、非表示を切り替え


	// ウィンドウ位置とサイズを復元
	restoreGeometry(app->getSetting(P6VXApp::keyGeometry).toByteArray());
	if(app->getSetting(P6VXApp::keyMaximized).toBool()){
		showMaximized();
	}
}

RenderView *MainWidget::getMainView()
{
	return MainView;
}

void MainWidget::adjustSizeToChild(QSize size)
{
	setGeometry(x(), y(), size.width(), height());
}

void MainWidget::closeEvent(QCloseEvent *event)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	// ウィンドウ位置とサイズを保存
	app->setSetting(P6VXApp::keyGeometry, saveGeometry());
	app->setSetting(P6VXApp::keyMaximized, isMaximized());
	QWidget::closeEvent(event);
}
