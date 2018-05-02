#ifndef VIRTUALKEYTABWIDGET_H
#define VIRTUALKEYTABWIDGET_H

#include <QTabWidget>

namespace Ui {
class VirtualKeyTabWidget;
}

class VirtualKeyTabWidget : public QTabWidget
{
	Q_OBJECT

public:
	explicit VirtualKeyTabWidget(QWidget *parent = 0);
	~VirtualKeyTabWidget();

private:
	Ui::VirtualKeyTabWidget *ui;
};

#endif // VIRTUALKEYTABWIDGET_H
