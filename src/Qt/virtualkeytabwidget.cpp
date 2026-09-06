#include "virtualkeytabwidget.h"
#include "ui_virtualkeytabwidget.h"
#include "keystatewatcher.h"
#include "normalvirtualkeyboardscene.h"
#include "simplevirtualkeyboardscene.h"
#include "virtualkeyboardview.h"

VirtualKeyTabWidget::VirtualKeyTabWidget(QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::VirtualKeyTabWidget)
{
	ui->setupUi(this);
	Views.push_back(ui->graphicsViewNormalKeyboard);
	Views.push_back(ui->graphicsViewSimpleKeyboard);
	ui->graphicsViewNormalKeyboard->setKeyboardScene(new NormalVirtualKeyboardScene(this));
	ui->graphicsViewSimpleKeyboard->setKeyboardScene(new SimpleVirtualKeyboardScene(this));
}

VirtualKeyTabWidget::~VirtualKeyTabWidget()
{
	delete ui;
}

void VirtualKeyTabWidget::setKeyStateWatcher(KeyStateWatcher* watcher)
{
	for (auto view : Views){
		view->setKeyStateWatcher(watcher);
	}
}

void VirtualKeyTabWidget::showEvent(QShowEvent *event)
{
	QTabWidget::showEvent(event);
}


void VirtualKeyTabWidget::resizeEvent(QResizeEvent *event)
{
	QTabWidget::resizeEvent(event);
}

