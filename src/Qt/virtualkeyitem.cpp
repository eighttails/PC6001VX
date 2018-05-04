#include "virtualkeyitem.h"
#include <QDebug>
#include <QTouchEvent>

#include "../osd.h"

VirtualKeyItem::VirtualKeyItem(
		PCKEYsym code,
		QString pixNormal,
		QString pixShift,
		QString pixGrph,
		QString pixKana,
		QString pixKKana)
	: Code(code)
	, PixNormal(pixNormal)
	, PixShift(pixShift)
	, PixGrph(pixGrph)
	, PixKana(pixKana)
	, PixKKana(pixKKana)
{
}

void VirtualKeyItem::changeStatus(
		bool ON_SHIFT,
		bool ON_GRAPH,
		bool ON_KANA,
		bool ON_KKANA,
		bool ON_CAPS)
{
	if (ON_KKANA){
		setPixmap(PixKKana);
	} else if (ON_KANA) {
		setPixmap(PixKana);
	} else if (ON_GRAPH) {
		setPixmap(PixGrph);
	} else if (ON_SHIFT ^ ON_CAPS) {
		setPixmap(PixShift);
	} else {
		setPixmap(PixNormal);
	}
}

bool VirtualKeyItem::sceneEvent(QEvent *event)
{
	qDebug() << "event accepted:" << event->type();
	//QMessageBox::warning(nullptr,"event accepted:","event accepted:");
	switch (event->type()){
	case QEvent::TouchBegin:
	case QEvent::TouchEnd:
	{
		QTouchEvent* event = dynamic_cast<QTouchEvent*>(event);
		qDebug() << "event TouchBegin:";
		Event ev;
		ev.type			= event->type() == QEvent::TouchBegin ? EV_KEYDOWN : EV_KEYUP;
		ev.key.state	= event->type() == QEvent::TouchBegin ? true : false;
		ev.key.sym		= Code;
		ev.key.mod		= KVM_NONE; //#TODO SHIFTキーが効かなかったら直す
		ev.key.unicode	= 0;
		/*
		ev.key.mod	   = (PCKEYmod)(
					( ke->modifiers() & Qt::ShiftModifier ? KVM_SHIFT : KVM_NONE )
					| ( ke->modifiers() & Qt::ControlModifier ? KVM_CTRL : KVM_NONE )
					| ( ke->modifiers() & Qt::AltModifier ? KVM_ALT : KVM_NONE )
					| ( ke->modifiers() & Qt::MetaModifier ? KVM_META : KVM_NONE )
					| ( ke->modifiers() & Qt::KeypadModifier ? KVM_NUM : KVM_NONE )
					// CAPSLOCKは検出できない？
					//| ( ke->modifiers() & Qt::caps ? KVM_NUM : KVM_NONE )
					);
		*/
		//ev.key.unicode = ke->text().utf16()[0];
		OSD_PushEvent(ev);
		break;
	}
	default:;
	}
	return QGraphicsPixmapItem::sceneEvent(event);
}

