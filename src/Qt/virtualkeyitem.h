#ifndef VIRTUALKEYITEM_H
#define VIRTUALKEYITEM_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QTimer>

#include "../typedef.h"
#include "../keydef.h"
#include "../event.h"

// 仮想キーボード用の各キー表示、入力を行うアイテム
class VirtualKeyItem : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qreal x READ x NOTIFY geometryChanged)
	Q_PROPERTY(qreal y READ y NOTIFY geometryChanged)
	Q_PROPERTY(qreal width READ width NOTIFY geometryChanged)
	Q_PROPERTY(qreal height READ height NOTIFY geometryChanged)
	Q_PROPERTY(QString imageSource READ imageSource NOTIFY imageSourceChanged)
	Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
public:
	VirtualKeyItem(PCKEYsym code,
				   PCKEYmod mod,
				   QString pixNormal,
				   QString pixShift,
				   QString pixGrph,
				   QString pixKana,
				   QString pixKanaShift,
				   QString pixKKana,
				   QString pixKKanaShift,
				   bool isAlpha = false,
				   bool mouseToggle = false,
				   QObject *parent = nullptr);

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

	Q_INVOKABLE void pointerPressed(qreal x, qreal y, bool isTouch = false);
	Q_INVOKABLE void pointerMoved(qreal x, qreal y);
	Q_INVOKABLE void pointerReleased(qreal x, qreal y, bool isTouch = false);

public slots:
	void changeStatus(
			bool ON_SHIFT,	// SHIFT
			bool ON_CTRL,	// CTRL
			bool ON_GRAPH,	// GRAPH
			bool ON_KANA,	// かな
			bool ON_KKANA,	// カタカナ
			bool ON_CAPS,	// CAPS
			bool ON_ROMAJI	// ローマ字入力
			);

signals:
	void geometryChanged();
	void imageSourceChanged();
	void pressedChanged();

protected:
	void sendKeyEvent(EventType type, bool state);
	void setImageSource(const QString &source);
	void setPressed(bool pressed);
	void startKeyRepeat();
	void stopKeyRepeat();
	bool isRepeatable() const;
	static QString pixmapUrl(const QString &name);

	const PCKEYsym Code;			// キーコード
	const PCKEYmod Mod;				// キーモディファイヤ
	const QString PixNormal;		// 通常時の画像
	const QString PixShift;			// SHIFT押下時の画像
	const QString PixGrph;			// GRPH押下時の画像
	const QString PixKana;			// かなモード時の画像
	const QString PixKanaShift;		// かな+SHIFTモード時の画像
	const QString PixKKana;			// カナモード時の画像
	const QString PixKKanaShift;	// カナ+SHIFTモード時の画像
	const bool IsAlpha;				// アルファベットフラグ(CAPS時に大文字小文字が反転)
	const bool MouseToggle;			// マウス入力時にトグル入力にする(SHIFT,GRPH用)
	bool ToggleStatus;				// マウス入力時のトグル状態

	QPointF Position;
	QSizeF Size;
	QString CurrentImageSource;
	bool Pressed;
	QTimer RepeatTimer;
};

#endif // VIRTUALKEYITEM_H
