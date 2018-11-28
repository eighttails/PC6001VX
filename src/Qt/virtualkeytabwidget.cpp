#include "virtualkeytabwidget.h"
#include "ui_virtualkeytabwidget.h"
#include "keystatewatcher.h"
#include "normalvirtualkeyboardscene.h"
#include "simplevirtualkeyboardscene.h"

VirtualKeyTabWidget::VirtualKeyTabWidget(QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::VirtualKeyTabWidget)
{
	ui->setupUi(this);
	Views.push_back(ui->graphicsViewNormalKeyboard);
	Views.push_back(ui->graphicsViewSimpleKeyboard);
	ui->graphicsViewNormalKeyboard->setScene(new NormalVirtualKeyboardScene(this));
	ui->graphicsViewSimpleKeyboard->setScene(new SimpleVirtualKeyboardScene(this));
}

VirtualKeyTabWidget::~VirtualKeyTabWidget()
{
	delete ui;
}

void VirtualKeyTabWidget::setKeyStateWatcher(KeyStateWatcher* watcher)
{
	for (auto view : Views){
		dynamic_cast<VirtualKeyboardScene*>(view->scene())->setKeyStateWatcher(watcher);
	}
}

void VirtualKeyTabWidget::showEvent(QShowEvent *event)
{
	QTabWidget::showEvent(event);
	for (auto view : Views){
		view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);
	}
}


void VirtualKeyTabWidget::resizeEvent(QResizeEvent *event)
{
	QTabWidget::resizeEvent(event);
	for (auto view : Views){
		view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);
	}
}


