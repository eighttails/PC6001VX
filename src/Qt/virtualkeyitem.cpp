#include "virtualkeyitem.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTouchEvent>
#include <QMessageBox>
#include <QGraphicsColorizeEffect>

#include "../osd.h"

VirtualKeyItem::VirtualKeyItem(PCKEYsym code,
							   QString pixNormal,
							   QString pixShift,
							   QString pixGrph,
							   QString pixKana,
							   QString pixKanaShift,
							   QString pixKKana,
							   QString pixKKanaShift,
							   bool mouseToggle)
	: Code(code)
	, PixNormal(QString(":/res/vkey/key_%1.png").arg(pixNormal))
	, PixShift(QString(":/res/vkey/key_%1.png").arg(pixShift))
	, PixGrph(QString(":/res/vkey/key_%1.png").arg(pixGrph))
	, PixKana(QString(":/res/vkey/key_%1.png").arg(pixKana))
	, PixKanaShift(QString(":/res/vkey/key_%1.png").arg(pixKanaShift))
	, PixKKana(QString(":/res/vkey/key_%1.png").arg(pixKKana))
	, PixKKanaShift(QString(":/res/vkey/key_%1.png").arg(pixKKanaShift))
	, MouseToggle(mouseToggle)
	, ToggleStatus(false)
	, pressEffect(new QGraphicsColorizeEffect(this))
{
	setPixmap(PixNormal);
	setTransformationMode(Qt::SmoothTransformation);
	setFlags(QGraphicsItem::ItemClipsToShape);

	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptTouchEvents(true);

	//ボタンを押すと色が変わるエフェクト
	pressEffect->setColor(Qt::blue);
	pressEffect->setEnabled(false);
	setGraphicsEffect(pressEffect);

}

void VirtualKeyItem::changeStatus(
		bool ON_SHIFT,
		bool ON_GRAPH,
		bool ON_KANA,
		bool ON_KKANA,
		bool ON_CAPS)
{
	if (ON_KKANA){
		if(ON_SHIFT)setPixmap(PixKKanaShift);
		else		setPixmap(PixKKana);
	} else if (ON_KANA) {
		if(ON_SHIFT)setPixmap(PixKanaShift);
		else		setPixmap(PixKKana);
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
	qDebug() << "sceneEvent accepted:" << event->type();
	auto type = event->type();
	switch (type){
	case QEvent::TouchBegin:
	case QEvent::TouchEnd:
	{
		sendKeyEvent(type == QEvent::TouchBegin ? EV_KEYDOWN : EV_KEYUP,
					 type == QEvent::TouchBegin ? true : false);
		if(type == QEvent::TouchBegin){
			pressEffect->setEnabled(true);
		} else {
			pressEffect->setEnabled(false);
		}
		return true;
	}
	default:;
	}
	return QGraphicsPixmapItem::sceneEvent(event);
}

void VirtualKeyItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug() << "mousePressEvent accepted:" << event->type();
	// トグルキーの場合はUP，DOWNを交互に送る
	auto toggle = MouseToggle && ToggleStatus;
	if (MouseToggle) ToggleStatus = !ToggleStatus;
	sendKeyEvent(toggle ? EV_KEYUP : EV_KEYDOWN, MouseToggle ? ToggleStatus : true);
	if (MouseToggle && ToggleStatus){
		pressEffect->setEnabled(true);
	} else {
		pressEffect->setEnabled(false);
	}
	QGraphicsPixmapItem::mousePressEvent(event);
}

void VirtualKeyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	sendKeyEvent(EV_KEYUP, false);
	QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void VirtualKeyItem::sendKeyEvent(EventType type, bool state)
{
	Event ev;
	ev.type			= type;
	ev.key.state	= state;
	ev.key.sym		= Code;
	ev.key.mod		= KVM_NONE;
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
}

