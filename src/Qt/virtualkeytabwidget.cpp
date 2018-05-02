#include "virtualkeytabwidget.h"
#include "ui_virtualkeytabwidget.h"

VirtualKeyTabWidget::VirtualKeyTabWidget(QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::VirtualKeyTabWidget)
{
	ui->setupUi(this);
}

VirtualKeyTabWidget::~VirtualKeyTabWidget()
{
	delete ui;
}
