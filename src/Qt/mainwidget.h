#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
class RenderView;

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget *parent = nullptr);

	RenderView* getMainView();
signals:

public slots:
	void adjustSizeToChild(QSize size);

protected:
	virtual void closeEvent(QCloseEvent* event);

	RenderView* MainView;

};

#endif // MAINWIDGET_H
