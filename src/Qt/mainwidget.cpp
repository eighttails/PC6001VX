#include "mainwidget.h"

#include <QVBoxLayout>
#include <QResizeEvent>

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
	VKeyWidget = new VirtualKeyTabWidget();
	layout->addWidget(VKeyWidget);
	VKeyWidget->setVisible(app->getSetting(P6VXApp::keyVirtualKeyVisible).toBool());

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

VirtualKeyTabWidget *MainWidget::getVirtualKeyboard()
{
	return VKeyWidget;
}

void MainWidget::setKeyStateWatcher(KeyStateWatcher* watcher)
{
	VKeyWidget->setKeyStateWatcher(watcher);
}

void MainWidget::adjustSizeToChild(QSize size)
{
#ifndef ALWAYSFULLSCREEN
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	setGeometry(x(), y(), size.width(), size.height() * (VKeyWidget->isVisible() ? 2 : 1));
#endif
}

void MainWidget::toggleVirtualKeyboard()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	auto size = MainView->size();
	VKeyWidget->setVisible(!VKeyWidget->isVisible());
	app->setSetting(P6VXApp::keyVirtualKeyVisible, VKeyWidget->isVisible());
	adjustSizeToChild(size);
}

void MainWidget::closeEvent(QCloseEvent *event)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	// ウィンドウ位置とサイズを保存
	app->setSetting(P6VXApp::keyGeometry, saveGeometry());
	app->setSetting(P6VXApp::keyMaximized, isMaximized());
	app->setSetting(P6VXApp::keyVirtualKeyVisible, VKeyWidget->isVisible());
	QWidget::closeEvent(event);
}


void MainWidget::resizeEvent(QResizeEvent *event)
{
	VKeyWidget->setMaximumHeight(event->size().height() / 2);
	ensurePolished();
}
