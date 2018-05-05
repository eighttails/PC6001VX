#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
class RenderView;
class VirtualKeyTabWidget;
class KeyStateWatcher;

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget *parent = nullptr);

	RenderView* getMainView();
	void setKeyStateWatcher(KeyStateWatcher *watcher);
signals:

public slots:
	void adjustSizeToChild(QSize size);

protected:
	virtual void closeEvent(QCloseEvent* event);

	RenderView* MainView;
	VirtualKeyTabWidget* VKeyWidget;
};

#endif // MAINWIDGET_H
