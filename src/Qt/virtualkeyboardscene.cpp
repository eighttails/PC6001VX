#include "virtualkeyboardscene.h"
#include "virtualkeyitem.h"
#include "keystatewatcher.h"

VirtualKeyboardScene::VirtualKeyboardScene(QObject *parent)
	: QGraphicsScene(parent)
	, Watcher(nullptr)
{
}

void VirtualKeyboardScene::setKeyStateWatcher(KeyStateWatcher *watcher)
{
	Watcher = watcher;
	foreach (auto item, this->items()) {
		auto obj = dynamic_cast<VirtualKeyItem*>(item);
		if(!obj) continue;
		// KeyStateWatcherから状態変更通知が来たら仮想キーアイテムに通知する
		connect(Watcher, SIGNAL(stateChanged(bool,bool,bool,bool,bool,bool,bool)), obj,
				SLOT(changeStatus(bool,bool,bool,bool,bool,bool,bool)));
	}
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
				mouseToggle);

	addItem(item);
	return item;
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
