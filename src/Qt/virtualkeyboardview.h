#ifndef VIRTUALKEYBOARDVIEW_H
#define VIRTUALKEYBOARDVIEW_H

#include <QGraphicsView>

class VirtualKeyboardView : public QGraphicsView
{
public:
	VirtualKeyboardView(QWidget *parent = Q_NULLPTR);
public slots:

	// QWidget interface
public:
	virtual QSize sizeHint() const override;

	// QWidget interface
protected:
	virtual void showEvent(QShowEvent *event) override;
	virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // VIRTUALKEYBOARDVIEW_H
