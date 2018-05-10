#include "virtualkeyboardview.h"

VirtualKeyboardView::VirtualKeyboardView(QWidget *parent)
	: QGraphicsView(parent)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
}


QSize VirtualKeyboardView::sizeHint() const
{
	if(scene()){
		auto ratio = scene()->width() / width();
		return (scene()->sceneRect().size() / ratio).toSize();
	} else {
		return size();
	}
}

void VirtualKeyboardView::showEvent(QShowEvent *event)
{
	if(scene()){
		fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
	}
}

void VirtualKeyboardView::resizeEvent(QResizeEvent *event)
{
	if(scene()){
		fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
	}
}
