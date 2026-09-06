#ifndef VIRTUALSTICKITEM_H
#define VIRTUALSTICKITEM_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QTimer>
#include <array>

#include "../typedef.h"
#include "../keydef.h"
#include "../event.h"

// 仮想キーボード用のスティック(カーソルキー)入力アイテム
class VirtualStickItem : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qreal x READ x NOTIFY geometryChanged)
	Q_PROPERTY(qreal y READ y NOTIFY geometryChanged)
	Q_PROPERTY(qreal width READ width NOTIFY geometryChanged)
	Q_PROPERTY(qreal height READ height NOTIFY geometryChanged)
	Q_PROPERTY(QString imageSource READ imageSource CONSTANT)
	Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
public:
	explicit VirtualStickItem(QObject *parent = nullptr);

	qreal x() const;
	qreal y() const;
	qreal width() const;
	qreal height() const;
	QString imageSource() const;
	bool pressed() const;

	void setPos(const QPointF &pos);
	void setPos(qreal x, qreal y);
	QPointF pos() const;
	QRectF boundingRect() const;

	Q_INVOKABLE void pointerPressed(qreal x, qreal y);
	Q_INVOKABLE void pointerMoved(qreal x, qreal y);
	Q_INVOKABLE void pointerReleased(qreal x, qreal y);

signals:
	void geometryChanged();
	void pressedChanged();

protected:
	void sendKeyEvent(EventType type, PCKEYsym code, bool state);
	std::array<bool, 4> estimateStickInput(QPointF coord);
	void setPressed(bool pressed);
	void startKeyRepeat();
	void stopKeyRepeat();
	bool hasPressedDirection() const;

	QPointF Position;
	QSizeF Size;
	std::array<bool, 4> currentKeyStatus;
	bool Pressed;
	QTimer RepeatTimer;
};

#endif // VIRTUALSTICKITEM_H
