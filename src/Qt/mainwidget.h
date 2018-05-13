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
	VirtualKeyTabWidget* getVirtualKeyboard();
	void setKeyStateWatcher(KeyStateWatcher *watcher);
signals:

public slots:
	void adjustSizeToChild(QSize size);
	void toggleVirtualKeyboard();

	// QWidget interface
protected:
	virtual void closeEvent(QCloseEvent* event);
	virtual void resizeEvent(QResizeEvent *event) override;

	RenderView* MainView;
	VirtualKeyTabWidget* VKeyWidget;

};

#endif // MAINWIDGET_H
