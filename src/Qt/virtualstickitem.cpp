#include "virtualstickitem.h"

#include <QImage>

#include "../osd.h"

namespace {
constexpr int KeyRepeatInitialDelayMs = 960;
constexpr int KeyRepeatIntervalMs = 66;
}

// 上下左右に対応するキーコード
static const PCKEYsym keySims[] = {
	KVC_UP,
	KVC_DOWN,
	KVC_LEFT,
	KVC_RIGHT,
};

VirtualStickItem::VirtualStickItem(QObject *parent)
	: QObject(parent)
	, Size(QImage(":/res/vkey/key_stick.png").size())
	, currentKeyStatus{false,false,false,false}
	, Pressed(false)
{
	RepeatTimer.setSingleShot(false);
	connect(&RepeatTimer, &QTimer::timeout, this, [this]() {
		for(size_t i = 0; i < currentKeyStatus.size(); i++){
			if(currentKeyStatus[i]){
				sendKeyEvent(EV_KEYDOWN, keySims[i], true);
			}
		}
		RepeatTimer.setInterval(KeyRepeatIntervalMs);
	});
}

qreal VirtualStickItem::x() const
{
	return Position.x();
}

qreal VirtualStickItem::y() const
{
	return Position.y();
}

qreal VirtualStickItem::width() const
{
	return Size.width();
}

qreal VirtualStickItem::height() const
{
	return Size.height();
}

QString VirtualStickItem::imageSource() const
{
	return QStringLiteral("qrc:/res/vkey/key_stick.png");
}

bool VirtualStickItem::pressed() const
{
	return Pressed;
}

void VirtualStickItem::setPos(const QPointF &pos)
{
	if (Position == pos) return;

	Position = pos;
	emit geometryChanged();
}

void VirtualStickItem::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

QPointF VirtualStickItem::pos() const
{
	return Position;
}

QRectF VirtualStickItem::boundingRect() const
{
	return QRectF(QPointF(0, 0), Size);
}

void VirtualStickItem::pointerPressed(qreal x, qreal y)
{
	setPressed(true);
	auto keyStatus = estimateStickInput(QPointF(x, y));
	for(size_t i = 0; i < keyStatus.size(); i++){
		if(keyStatus[i]){
			sendKeyEvent(EV_KEYDOWN, keySims[i], true);
		}
	}
	currentKeyStatus = keyStatus;
	startKeyRepeat();
}

void VirtualStickItem::pointerMoved(qreal x, qreal y)
{
	auto keyStatus = estimateStickInput(QPointF(x, y));
	for(size_t i = 0; i < keyStatus.size(); i++){
		// 前回と状態が変わったキーのイベントを送信
		if (keyStatus[i] && !currentKeyStatus[i]){
			sendKeyEvent(EV_KEYDOWN, keySims[i], true);
		}
		else if (!keyStatus[i] && currentKeyStatus[i]){
			sendKeyEvent(EV_KEYUP, keySims[i], false);
		}
	}
	currentKeyStatus = keyStatus;
	if (hasPressedDirection()) {
		startKeyRepeat();
	} else {
		stopKeyRepeat();
	}
}

void VirtualStickItem::pointerReleased(qreal x, qreal y)
{
	Q_UNUSED(x);
	Q_UNUSED(y);

	setPressed(false);
	stopKeyRepeat();
	// それまで押されていたキーをリリースする
	for(size_t i = 0; i < currentKeyStatus.size(); i++){
		sendKeyEvent(EV_KEYUP, keySims[i], false);
	}
	// キー押下状態をクリアする
	currentKeyStatus = {false,false,false,false};
}

void VirtualStickItem::sendKeyEvent(EventType type, PCKEYsym code, bool state)
{
	Event ev;
	ev.type			= type;
	ev.key.state	= state;
	ev.key.sym		= code;
	ev.key.mod		= KVM_NONE;
	ev.key.unicode	= 0;

	OSD_PushEvent(ev);
}

// タップした座標に対して、上下左右のカーソルキー押下状態の配列を返す
std::array<bool, 4> VirtualStickItem::estimateStickInput(QPointF coord)
{
	auto val = std::array<bool, 4>();

	// タップ座標が上下左右1/3の範囲にあったら
	// その方向のキーが押されていると判断する

	// ↑
	val[0] = (Size.height() / 3) > coord.y();
	// ↓
	val[1] = (Size.height() / 3) * 2 < coord.y();
	// ←
	val[2] = (Size.width() / 3) > coord.x();
	// →
	val[3] = (Size.width() / 3) * 2 < coord.x();

	return val;
}

void VirtualStickItem::setPressed(bool pressed)
{
	if (Pressed == pressed) return;

	Pressed = pressed;
	emit pressedChanged();
}

void VirtualStickItem::startKeyRepeat()
{
	if (!hasPressedDirection()) return;

	if (!RepeatTimer.isActive()) {
		RepeatTimer.start(KeyRepeatInitialDelayMs);
	}
}

void VirtualStickItem::stopKeyRepeat()
{
	RepeatTimer.stop();
}

bool VirtualStickItem::hasPressedDirection() const
{
	for (bool status : currentKeyStatus) {
		if (status) return true;
	}
	return false;
}
