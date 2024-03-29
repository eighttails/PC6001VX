#include "virtualstickitem.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTouchEvent>
#include <QMessageBox>
#include <QGraphicsColorizeEffect>

#include "../osd.h"

// 上下左右に対応するキーコード
static const PCKEYsym keySims[] = {
	KVC_UP,
	KVC_DOWN,
	KVC_LEFT,
	KVC_RIGHT,
};

VirtualStickItem::VirtualStickItem(QObject *parent)
	: QObject(parent)
	, currentKeyStatus{false,false,false,false}
	, pressEffect(new QGraphicsColorizeEffect(this))
{
	setPixmap(QPixmap(":/res/vkey/key_stick.png"));
	setTransformationMode(Qt::SmoothTransformation);
	setFlags(QGraphicsItem::ItemClipsToShape);

	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptTouchEvents(true);

	// ボタンを押すと色が変わるエフェクト
	pressEffect->setColor(Qt::blue);
	pressEffect->setEnabled(false);
	setGraphicsEffect(pressEffect);
}

bool VirtualStickItem::sceneEvent(QEvent *event)
{
	auto type = event->type();
	switch (type){
	case QEvent::TouchBegin:
	case QEvent::TouchEnd:
	case QEvent::TouchUpdate:
	case QEvent::TouchCancel:
	{
		auto touchEvent = dynamic_cast<QTouchEvent*>(event);
		for(auto p : touchEvent->points()){
			auto touchState = p.state();
			switch(touchState){
			case QEventPoint::Pressed:
			{
				pressEffect->setEnabled(true);
				// タッチした座標に対応するキーを押下する(同時押しによる斜め入力あり)
				auto keyStatus = estimateStickInput(p.position());
				for(size_t i = 0; i < keyStatus.size(); i++){
					if(keyStatus[i]){
						sendKeyEvent(EV_KEYDOWN, keySims[i], true);
					}
				}
				currentKeyStatus = keyStatus;
				break;
			}
			case QEventPoint::Released:
			{
				pressEffect->setEnabled(false);
				// それまで押されていたキーをリリースする
				for(size_t i = 0; i < currentKeyStatus.size(); i++){
					sendKeyEvent(EV_KEYUP, keySims[i], false);
				}
				// キー押下状態をクリアする
				currentKeyStatus = {false,false,false,false};
				break;
			}
			case QEventPoint::Updated:
			case QEventPoint::Stationary:
			{
				auto keyStatus = estimateStickInput(touchEvent->points()[0].position());
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
				break;
			}
			default:;
			}
		}
		return true;
	}
	default:;
		return QGraphicsPixmapItem::sceneEvent(event);
	}
}

void VirtualStickItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	pressEffect->setEnabled(true);
	// タッチした座標に対応するキーを押下する(同時押しによる斜め入力あり)
	auto keyStatus = estimateStickInput(event->pos());
	for(size_t i = 0; i < keyStatus.size(); i++){
		if(keyStatus[i]){
			sendKeyEvent(EV_KEYDOWN, keySims[i], true);
		}
	}
	currentKeyStatus = keyStatus;
	event->accept();
}

void VirtualStickItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	auto keyStatus = estimateStickInput(event->pos());
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
	event->accept();
}

void VirtualStickItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	pressEffect->setEnabled(false);
	// それまで押されていたキーをリリースする
	for(size_t i = 0; i < currentKeyStatus.size(); i++){
		sendKeyEvent(EV_KEYUP, keySims[i], false);
	}
	// キー押下状態をクリアする
	currentKeyStatus = {false,false,false,false};
	event->accept();
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
	auto size = this->sceneBoundingRect().size();

	// タップ座標が上下左右1/3の範囲にあったら
	// その方向のキーが押されていると判断する

	// ↑
	val[0] = (size.height() / 3) > coord.y();
	// ↓
	val[1] = (size.height() / 3) * 2 < coord.y();
	// ←
	val[2] = (size.width() / 3) > coord.x();
	// →
	val[3] = (size.width() / 3) * 2 < coord.x();

	return val;
}
