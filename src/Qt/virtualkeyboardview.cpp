#include "virtualkeyboardview.h"

#include <QEvent>

VirtualKeyboardView::VirtualKeyboardView(QWidget *parent)
	: QGraphicsView(parent)
{
}


bool VirtualKeyboardView::hasHeightForWidth() const
{
	return true;
}

int VirtualKeyboardView::heightForWidth(int width) const
{
	if(scene()){
		auto sceneAspectRatio = scene()->height() / scene()->width();
		return int(sceneAspectRatio * width);
	} else {
		return width;
	}
}


bool VirtualKeyboardView::event(QEvent *event)
{
	switch (event->type()) {
	case QEvent::Show:
	case QEvent::Expose:
	case QEvent::Resize:
		if(scene()){
			fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
		}
		break;
	default:;
	}
	return QGraphicsView::event(event);
}


