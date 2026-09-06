#ifndef VIRTUALKEYBOARDSCENE_H
#define VIRTUALKEYBOARDSCENE_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QVariantList>

#include <vector>
#include "typedef.h"
#include "../keydef.h"

class KeyStateWatcher;
class VirtualKeyItem;
class VirtualStickItem;

class VirtualKeyboardScene : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
	Q_PROPERTY(qreal sceneWidth READ sceneWidth NOTIFY sceneRectChanged)
	Q_PROPERTY(qreal sceneHeight READ sceneHeight NOTIFY sceneRectChanged)
public:
	explicit VirtualKeyboardScene(QObject* parent = nullptr);
	void setKeyStateWatcher(KeyStateWatcher* watcher);

	QVariantList items() const;
	qreal sceneWidth() const;
	qreal sceneHeight() const;

signals:
	void itemsChanged();
	void sceneRectChanged();

protected:
	VirtualKeyItem* createVirtualKeyItem(PCKEYsym code, PCKEYmod mod,
										 QString pixNormal,
										 QString pixShift,
										 QString pixGrph,
										 QString pixKana,
										 QString pixKanaShift,
										 QString pixKKana,
										 QString pixKKanaShift,
										 bool isAlpha = false,
										 bool mouseToggle = false);
	void addItem(VirtualStickItem *item);

	void alignVirtualKeyItems(std::vector<VirtualKeyItem*> list, QPointF coord);
	QRectF itemsBoundingRect() const;
	void setSceneRect(const QRectF &rect);

	KeyStateWatcher* Watcher;
	QList<QObject*> Items;
	QList<VirtualKeyItem*> KeyItems;
	QRectF SceneRect;
};

#endif // VIRTUALKEYBOARDSCENE_H
