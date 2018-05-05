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
	ui->graphicsViewNormalKeyboard->setScene(new NormalVirtualKeyboardScene(this));
	ui->graphicsViewSimpleKeyboard->setScene(new SimpleVirtualKeyboardScene(this));
}

VirtualKeyTabWidget::~VirtualKeyTabWidget()
{
	delete ui;
}

void VirtualKeyTabWidget::setKeyStateWatcher(KeyStateWatcher* watcher)
{
	dynamic_cast<VirtualKeyboardScene*>(ui->graphicsViewNormalKeyboard->scene())->setKeyStateWatcher(watcher);
	dynamic_cast<VirtualKeyboardScene*>(ui->graphicsViewSimpleKeyboard->scene())->setKeyStateWatcher(watcher);
}


void VirtualKeyTabWidget::resizeEvent(QResizeEvent *event)
{
	QTabWidget::resizeEvent(event);
	ui->graphicsViewNormalKeyboard->fitInView(ui->graphicsViewNormalKeyboard->scene()->sceneRect(), Qt::KeepAspectRatio);
	ui->graphicsViewSimpleKeyboard->fitInView(ui->graphicsViewSimpleKeyboard->scene()->sceneRect(), Qt::KeepAspectRatio);

}


void VirtualKeyTabWidget::showEvent(QShowEvent *event)
{
	ui->graphicsViewNormalKeyboard->fitInView(ui->graphicsViewNormalKeyboard->scene()->sceneRect(), Qt::KeepAspectRatio);
	ui->graphicsViewSimpleKeyboard->fitInView(ui->graphicsViewSimpleKeyboard->scene()->sceneRect(), Qt::KeepAspectRatio);
}
