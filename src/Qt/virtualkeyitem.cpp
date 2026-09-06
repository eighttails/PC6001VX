#include "virtualkeyitem.h"

#include <QImage>

#include "../osd.h"

namespace {
constexpr int KeyRepeatInitialDelayMs = 960;
constexpr int KeyRepeatIntervalMs = 66;
}

VirtualKeyItem::VirtualKeyItem(PCKEYsym code,
							   PCKEYmod mod,
							   QString pixNormal,
							   QString pixShift,
							   QString pixGrph,
							   QString pixKana,
							   QString pixKanaShift,
							   QString pixKKana,
							   QString pixKKanaShift,
							   bool isAlpha,
							   bool mouseToggle,
							   QObject *parent)
	: QObject(parent)
	, Code(code)
	, Mod(mod)
	, PixNormal(pixmapUrl(pixNormal))
	, PixShift(pixmapUrl(pixShift))
	, PixGrph(pixmapUrl(pixGrph))
	, PixKana(pixmapUrl(pixKana))
	, PixKanaShift(pixmapUrl(pixKanaShift))
	, PixKKana(pixmapUrl(pixKKana))
	, PixKKanaShift(pixmapUrl(pixKKanaShift))
	, IsAlpha(isAlpha)
	, MouseToggle(mouseToggle)
	, ToggleStatus(false)
	, Size(QImage(QString(":/res/vkey/key_%1.png").arg(pixNormal)).size())
	, CurrentImageSource(PixNormal)
	, Pressed(false)
{
	RepeatTimer.setSingleShot(false);
	connect(&RepeatTimer, &QTimer::timeout, this, [this]() {
		sendKeyEvent(EV_KEYDOWN, true);
		RepeatTimer.setInterval(KeyRepeatIntervalMs);
	});
}

qreal VirtualKeyItem::x() const
{
	return Position.x();
}

qreal VirtualKeyItem::y() const
{
	return Position.y();
}

qreal VirtualKeyItem::width() const
{
	return Size.width();
}

qreal VirtualKeyItem::height() const
{
	return Size.height();
}

QString VirtualKeyItem::imageSource() const
{
	return CurrentImageSource;
}

bool VirtualKeyItem::pressed() const
{
	return Pressed;
}

void VirtualKeyItem::setPos(const QPointF &pos)
{
	if (Position == pos) return;

	Position = pos;
	emit geometryChanged();
}

void VirtualKeyItem::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

QPointF VirtualKeyItem::pos() const
{
	return Position;
}

QRectF VirtualKeyItem::boundingRect() const
{
	return QRectF(QPointF(0, 0), Size);
}

void VirtualKeyItem::pointerPressed(qreal x, qreal y)
{
	Q_UNUSED(x);
	Q_UNUSED(y);

	// トグルキーの場合はUP，DOWNを交互に送る
	if (MouseToggle) ToggleStatus = !ToggleStatus;
	bool state = MouseToggle ? ToggleStatus : true;
	sendKeyEvent(state ? EV_KEYDOWN : EV_KEYUP, state);
	setPressed(state);
	if (state) {
		startKeyRepeat();
	} else {
		stopKeyRepeat();
	}
}

void VirtualKeyItem::pointerMoved(qreal x, qreal y)
{
	Q_UNUSED(x);
	Q_UNUSED(y);
}

void VirtualKeyItem::pointerReleased(qreal x, qreal y)
{
	Q_UNUSED(x);
	Q_UNUSED(y);

	if (!MouseToggle){
		stopKeyRepeat();
		setPressed(false);
		sendKeyEvent(EV_KEYUP, false);
	}
}

void VirtualKeyItem::changeStatus(
		bool ON_SHIFT,
		bool ON_CTRL,
		bool ON_GRAPH,
		bool ON_KANA,
		bool ON_KKANA,
		bool ON_CAPS,
		bool ON_ROMAJI)
{
	Q_UNUSED(ON_CTRL);

	if (ON_KANA && !ON_ROMAJI) {
		if (ON_KKANA){
			if(ON_SHIFT)setImageSource(PixKKanaShift);
			else		setImageSource(PixKKana);
		} else {
			if(ON_SHIFT)setImageSource(PixKanaShift);
			else		setImageSource(PixKana);
		}
	} else if (ON_GRAPH) {
		setImageSource(PixGrph);
	} else {
		if (IsAlpha) {
			if (ON_SHIFT ^ ON_CAPS)	setImageSource(PixShift);
			else					setImageSource(PixNormal);
		} else {
			if (ON_SHIFT)	setImageSource(PixShift);
			else			setImageSource(PixNormal);
		}
	}
}

void VirtualKeyItem::sendKeyEvent(EventType type, bool state)
{
	Event ev;
	ev.type			= type;
	ev.key.state	= state;
	ev.key.sym		= Code;
	ev.key.mod		= Mod;
	ev.key.unicode	= 0;

	OSD_PushEvent(ev);
}

void VirtualKeyItem::setImageSource(const QString &source)
{
	if (CurrentImageSource == source) return;

	CurrentImageSource = source;
	emit imageSourceChanged();
}

void VirtualKeyItem::setPressed(bool pressed)
{
	if (Pressed == pressed) return;

	Pressed = pressed;
	emit pressedChanged();
}

void VirtualKeyItem::startKeyRepeat()
{
	if (!isRepeatable()) return;

	RepeatTimer.start(KeyRepeatInitialDelayMs);
}

void VirtualKeyItem::stopKeyRepeat()
{
	RepeatTimer.stop();
}

bool VirtualKeyItem::isRepeatable() const
{
	if (MouseToggle) return false;

	switch (Code) {
	case KVC_MUHENKAN:	// SAVE
	case KVC_HENKAN:	// LOAD
	case KVC_F9:		// PAUSE
	case KVC_F12:		// SNAPSHOT / ROMAJI
	case KVC_SCROLLLOCK:	// CAPS
	case KVC_HIRAGANA:	// KANA
		return false;
	default:
		return true;
	}
}

QString VirtualKeyItem::pixmapUrl(const QString &name)
{
	return QString("qrc:/res/vkey/key_%1.png").arg(name);
}
