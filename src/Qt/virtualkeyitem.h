#ifndef VIRTUALKEYITEM_H
#define VIRTUALKEYITEM_H
#include <QObject>
#include <QGraphicsPixmapItem>
#include "../typedef.h"
#include "../keydef.h"
#include "../event.h"

class QGraphicsColorizeEffect;

//仮想キーボード用の各キー表示、入力を行うアイテム
class VirtualKeyItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	VirtualKeyItem(PCKEYsym code,
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
public slots:
	void changeStatus(
			bool ON_SHIFT,	// SHIFT
			bool ON_CTRL,	// CTRL
			bool ON_GRAPH,	// GRAPH
			bool ON_KANA,	// かな
			bool ON_KKANA,	// カタカナ
			bool ON_CAPS	// CAPS
			);

	// QGraphicsItem interface
protected:
	virtual bool sceneEvent(QEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void sendKeyEvent(EventType type, bool state);

	const PCKEYsym Code;			// キーコード
	const QPixmap PixNormal;		// 通常時の画像
	const QPixmap PixShift;			// SHIFT押下時の画像
	const QPixmap PixGrph;			// GRPH押下時の画像
	const QPixmap PixKana;			// かなモード時の画像
	const QPixmap PixKanaShift;		// かな+SHIFTモード時の画像
	const QPixmap PixKKana;			// カナモード時の画像
	const QPixmap PixKKanaShift;	// カナ+SHIFTモード時の画像
	const bool IsAlpha;				// アルファベットフラグ(CAPS時に大文字小文字が反転)
	const bool MouseToggle;			// マウス入力時にトグル入力にする(SHIFT,GRPH用)
	bool ToggleStatus;				// マウス入力時のトグル状態

	QGraphicsColorizeEffect* pressEffect; // ボタンを押すと色が変わるエフェクト
};

#endif // VIRTUALKEYITEM_H
