#include "virtualkeytabwidget.h"
#include "ui_virtualkeytabwidget.h"
#include "simplevirtualkeyboardscene.h"

VirtualKeyTabWidget::VirtualKeyTabWidget(QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::VirtualKeyTabWidget)
{
	ui->setupUi(this);
	ui->graphicsViewSimpleKeyboard->setScene(new SimpleVirtualKeyboardScene(nullptr, this));
}

VirtualKeyTabWidget::~VirtualKeyTabWidget()
{
	delete ui;
}


void VirtualKeyTabWidget::resizeEvent(QResizeEvent *event)
{
	QTabWidget::resizeEvent(event);
	ui->graphicsViewSimpleKeyboard->fitInView(ui->graphicsViewSimpleKeyboard->scene()->sceneRect(), Qt::KeepAspectRatio);

}


void VirtualKeyTabWidget::showEvent(QShowEvent *event)
{
	ui->graphicsViewSimpleKeyboard->fitInView(ui->graphicsViewSimpleKeyboard->scene()->sceneRect(), Qt::KeepAspectRatio);
}
