#include "virtualkeyboardview.h"
#include "../osd.h"
#include "p6vxapp.h"

#include <QEvent>

VirtualKeyboardView::VirtualKeyboardView(QWidget *parent)
	: QGraphicsView(parent)
{
	setAttribute(Qt::WA_Hover);
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
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	switch (event->type()) {
	case QEvent::Show:
	case QEvent::Expose:
	case QEvent::Resize:
		if(scene()){
			fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
		}
		break;
	case QEvent::Enter:
	case QEvent::HoverMove:
		OSD_ShowCursor(true);
		break;
	case QEvent::Leave:
		OSD_ShowCursor(true);
		break;
	default:;
	}
	return QGraphicsView::event(event);
}


