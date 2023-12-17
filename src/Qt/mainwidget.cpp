#include "mainwidget.h"

#include <QBoxLayout>
#include <QResizeEvent>
#include <QApplication>

#include "p6vxapp.h"
#include "renderview.h"
#include "virtualkeytabwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	//IMEを無効化
	setAttribute(Qt::WA_InputMethodEnabled, false);

	auto layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0 );
	setLayout(layout);

	// シーングラフ生成
	QGraphicsScene* Scene = new QGraphicsScene(this);

	// メインウィジェット(エミュレータのメイン画面)
	MainView = new RenderView(Scene);
	layout->addWidget(MainView);
	connect(MainView, SIGNAL(resized(QSize)), this, SLOT(adjustSizeToChild(QSize)));

	// 仮想キーウィジェット
	VKeyWidget = new VirtualKeyTabWidget();
	layout->addWidget(VKeyWidget);
	VKeyWidget->setVisible(app->getSetting(P6VXApp::keyVirtualKeyVisible).toBool());
	VKeyWidget->setCurrentIndex(app->getSetting(P6VXApp::keyVirtualKeyTabIndex).toInt());
	connect(VKeyWidget, SIGNAL(currentChanged(int)), this, SLOT(virtualKeyTabSelected(int)));
	adjustSize();

	// ウィンドウ位置とサイズを復元
	if(app->hasSetting(P6VXApp::keyGeometry)){
		restoreGeometry(app->getSetting(P6VXApp::keyGeometry).toByteArray());
	} else {
		adjustSizeToChild(MainView->size());
		layout->update();
#ifndef ALWAYSFULLSCREEN
		// 初回起動時は画面中心に表示
		QRect availableGeometry(QApplication::primaryScreen()->availableGeometry());
		move((availableGeometry.width() - width()) / 2,
			 (availableGeometry.height() - height()) / 2);
#endif
	}

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

void MainWidget::updateLayout()
{
	layout()->update();
	ensurePolished();
}

void MainWidget::adjustSizeToChild(QSize size)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	auto state = this->windowState();
	if (!((state & Qt::WindowFullScreen) | (state & Qt::WindowMaximized))){
		if (app->getSetting(P6VXApp::keyVirtualKeyPosition).toInt() == 2){
			// 下方向に仮想キーボードを表示する場合
			setGeometry(x(), y(), size.width(), size.height() * (VKeyWidget->isVisible() ? 2 : 1));
		} else {
			// それ以外
			setGeometry(x(), y(), size.width() * (VKeyWidget->isVisible() ? 2 : 1), size.height());
		}
	}
}

void MainWidget::toggleVirtualKeyboard()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	auto size = MainView->size();
	VKeyWidget->setVisible(!VKeyWidget->isVisible());
	app->setSetting(P6VXApp::keyVirtualKeyVisible, VKeyWidget->isVisible());
	app->setSetting(P6VXApp::keyVirtualKeyTabIndex, VKeyWidget->currentIndex());
	adjustSizeToChild(size);
}

void MainWidget::virtualKeyTabSelected(int index)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->setSetting(P6VXApp::keyVirtualKeyTabIndex, index);
}

void MainWidget::closeEvent(QCloseEvent *event)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	// ウィンドウ位置とサイズを保存
	app->setSetting(P6VXApp::keyGeometry, saveGeometry());
	app->setSetting(P6VXApp::keyMaximized, isMaximized());
	app->setSetting(P6VXApp::keyVirtualKeyVisible, VKeyWidget->isVisible());
	app->setSetting(P6VXApp::keyVirtualKeyTabIndex, VKeyWidget->currentIndex());

	// アプリの終了及び実際のウィンドウのクローズはメインロジックに移譲
	OSD_PushEvent(EV_QUIT);
	event->ignore();
}


void MainWidget::resizeEvent(QResizeEvent *event)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	// 仮想キーボードのは位置設定
	auto position = app->getSetting(P6VXApp::keyVirtualKeyPosition).toInt();
	
	if (event->size().width() > event->size().height()){ 
		// 横画面の場合、設定値に応じて仮想キーボードの配置位置を変える
		switch (position) {
		case 0: // 右
			dynamic_cast<QBoxLayout*>(this->layout())->setDirection(QBoxLayout::LeftToRight);
			VKeyWidget->setMaximumWidth(event->size().width() / 2);
			VKeyWidget->setMaximumHeight(QWIDGETSIZE_MAX);
			break;
		case 1: // 左
			dynamic_cast<QBoxLayout*>(this->layout())->setDirection(QBoxLayout::RightToLeft);
			VKeyWidget->setMaximumWidth(event->size().width() / 2);
			VKeyWidget->setMaximumHeight(QWIDGETSIZE_MAX);
			break;
		case 2: // 下
			dynamic_cast<QBoxLayout*>(this->layout())->setDirection(QBoxLayout::TopToBottom);
			VKeyWidget->setMaximumWidth(QWIDGETSIZE_MAX);
			VKeyWidget->setMaximumHeight(event->size().height() / 2);
			break;
		case 3: // 非表示
			dynamic_cast<QBoxLayout*>(this->layout())->setDirection(QBoxLayout::LeftToRight);
			VKeyWidget->setMaximumWidth(0);
			VKeyWidget->setMaximumHeight(QWIDGETSIZE_MAX);
			break;
		default:;
		}
		
	} else {
		// 縦画面の場合、設定値に関わらず仮想キーボードの配置位置は固定
		dynamic_cast<QBoxLayout*>(this->layout())->setDirection(QBoxLayout::TopToBottom);
		VKeyWidget->setMaximumWidth(QWIDGETSIZE_MAX);
		VKeyWidget->setMaximumHeight(event->size().height() / 2);
	}
	ensurePolished();
}
