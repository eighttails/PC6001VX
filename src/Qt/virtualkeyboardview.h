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
	virtual bool hasHeightForWidth() const override;
	virtual int heightForWidth(int width) const override;

	// QWidget interface
protected:
	virtual bool event(QEvent *event) override;

};

#endif // VIRTUALKEYBOARDVIEW_H
