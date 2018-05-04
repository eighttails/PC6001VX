#ifndef VIRTUALKEYITEM_H
#define VIRTUALKEYITEM_H
#include <QObject>
#include <QGraphicsPixmapItem>
#include "../keydef.h"

//仮想キーボード用の各キー表示、入力を行うアイテム
class VirtualKeyItem : public QObject, QGraphicsPixmapItem
{
	Q_OBJECT
public:
	VirtualKeyItem(
			PCKEYsym code,
			QString pixNormal,
			QString pixShift,
			QString pixGrph,
			QString pixKana,
			QString pixKKana
			);
public slots:
	void changeStatus(
			bool ON_SHIFT,	// SHIFT
			bool ON_GRAPH,	// GRAPH
			bool ON_KANA,	// かな
			bool ON_KKANA,	// カタカナ
			bool ON_CAPS	// CAPS
			);

	// QGraphicsItem interface
protected:
	virtual bool sceneEvent(QEvent *event) override;

	const PCKEYsym Code;		//キーコード
	const QPixmap PixNormal;	//通常時の画像
	const QPixmap PixShift;		//SHIFT押下時の画像
	const QPixmap PixGrph;		//GRPH押下時の画像
	const QPixmap PixKana;		//かなモード時の画像
	const QPixmap PixKKana;		//カナモード時の画像


};

#endif // VIRTUALKEYITEM_H
