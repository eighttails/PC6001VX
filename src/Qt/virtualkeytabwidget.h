#ifndef VIRTUALKEYTABWIDGET_H
#define VIRTUALKEYTABWIDGET_H

#include <QTabWidget>

namespace Ui {
class VirtualKeyTabWidget;
}
class KeyStateWatcher;
class QGraphicsView;

class VirtualKeyTabWidget : public QTabWidget
{
	Q_OBJECT

public:
	explicit VirtualKeyTabWidget(QWidget *parent = 0);
	~VirtualKeyTabWidget();

	void setKeyStateWatcher(KeyStateWatcher *watcher);

	// QWidget interface
protected:
	virtual void showEvent(QShowEvent *event) override;
	virtual void resizeEvent(QResizeEvent *event) override;

private:
	Ui::VirtualKeyTabWidget *ui;
	std::vector<QGraphicsView*> Views;
};

#endif // VIRTUALKEYTABWIDGET_H
