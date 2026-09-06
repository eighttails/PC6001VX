#include "virtualkeyboardscene.h"

#include "keystatewatcher.h"
#include "virtualkeyitem.h"
#include "virtualstickitem.h"

VirtualKeyboardScene::VirtualKeyboardScene(QObject *parent)
	: QObject(parent)
	, Watcher(nullptr)
{
}

void VirtualKeyboardScene::setKeyStateWatcher(KeyStateWatcher *watcher)
{
	Watcher = watcher;
	for (auto item : KeyItems) {
		// KeyStateWatcherから状態変更通知が来たら仮想キーアイテムに通知する
		connect(Watcher, SIGNAL(stateChanged(bool,bool,bool,bool,bool,bool,bool)), item,
				SLOT(changeStatus(bool,bool,bool,bool,bool,bool,bool)));
	}
}

QVariantList VirtualKeyboardScene::items() const
{
	QVariantList list;
	for (auto item : Items) {
		list.push_back(QVariant::fromValue(item));
	}
	return list;
}

qreal VirtualKeyboardScene::sceneWidth() const
{
	return SceneRect.width();
}

qreal VirtualKeyboardScene::sceneHeight() const
{
	return SceneRect.height();
}

VirtualKeyItem* VirtualKeyboardScene::createVirtualKeyItem(
		PCKEYsym code,
		PCKEYmod mod,
		QString pixNormal,
		QString pixShift,
		QString pixGrph,
		QString pixKana,
		QString pixKanaShift,
		QString pixKKana,
		QString pixKKanaShift,
		bool isAlpha,
		bool mouseToggle)
{
	auto item = new VirtualKeyItem(
				code,
				mod,
				pixNormal,
				pixShift,
				pixGrph,
				pixKana,
				pixKanaShift,
				pixKKana,
				pixKKanaShift,
				isAlpha,
				mouseToggle,
				this);

	Items.push_back(item);
	KeyItems.push_back(item);
	emit itemsChanged();
	return item;
}

void VirtualKeyboardScene::addItem(VirtualStickItem *item)
{
	item->setParent(this);
	Items.push_back(item);
	emit itemsChanged();
}

void VirtualKeyboardScene::alignVirtualKeyItems(std::vector<VirtualKeyItem *> list, QPointF coord)
{
	// アイテムを横一列に整列
	for (size_t i = 0; i < list.size(); i++){
		auto item = list[i];
		if (i == 0){
			item->setPos(coord);
		} else {
			auto prev = list[i-1];
			item->setPos(prev->pos().x() + prev->boundingRect().width(), coord.y());
		}
	}
}

QRectF VirtualKeyboardScene::itemsBoundingRect() const
{
	QRectF rect;
	for (auto item : Items) {
		if (auto key = qobject_cast<VirtualKeyItem*>(item)) {
			rect = rect.united(QRectF(key->pos(), key->boundingRect().size()));
		} else if (auto stick = qobject_cast<VirtualStickItem*>(item)) {
			rect = rect.united(QRectF(stick->pos(), stick->boundingRect().size()));
		}
	}
	return rect;
}

void VirtualKeyboardScene::setSceneRect(const QRectF &rect)
{
	if (SceneRect == rect) return;

	SceneRect = rect;
	emit sceneRectChanged();
}
