#ifndef VIRTUALSTICKITEM_H
#define VIRTUALSTICKITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <array>
#include "../keydef.h"
#include "../event.h"

class QGraphicsColorizeEffect;

//仮想キーボード用のスティック(カーソルキー)入力アイテム
class VirtualStickItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	explicit VirtualStickItem(QObject *parent = nullptr);

signals:

public slots:

protected:
	// QGraphicsItem interface
	virtual bool sceneEvent(QEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void sendKeyEvent(EventType type, PCKEYsym code, bool state);
	std::array<bool, 4> estimateStickInput(QPointF coord);

	std::array<bool, 4> currentKeyStatus;
	QGraphicsColorizeEffect* pressEffect; //ボタンを押すと色が変わるエフェクト

};

#endif // VIRTUALSTICKITEM_H
