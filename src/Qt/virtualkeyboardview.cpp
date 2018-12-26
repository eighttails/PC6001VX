#include "virtualkeyboardview.h"
#include "../osd.h"
#include "p6vxapp.h"

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
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	switch (event->type()) {
	case QEvent::Show:
	case QEvent::Expose:
	case QEvent::Resize:
		if(scene()){
			fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
		}
		break;
	case QEvent::HoverEnter:
		OSD_ShowCursor(app->getSetting(P6VXApp::keyVirtualKeyVisible).toBool());
		break;
	case QEvent::HoverLeave:
		OSD_RestoreCursor();
		break;
	default:;
	}
	return QGraphicsView::event(event);
}


