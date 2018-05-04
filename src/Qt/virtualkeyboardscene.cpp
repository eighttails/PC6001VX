#include "virtualkeyboardscene.h"
#include "virtualkeyitem.h"

VirtualKeyboardScene::VirtualKeyboardScene(KeyStateWatcher *watcher, QObject *parent)
	: QGraphicsScene(parent)
	, Watcher(watcher)
{
}

VirtualKeyItem* VirtualKeyboardScene::createVirtualKeyItem(
		PCKEYsym code,
		QString pixNormal,
		QString pixShift,
		QString pixGrph,
		QString pixKana,
		QString pixKanaShift,
		QString pixKKana,
		QString pixKKanaShift)
{
	auto item = new VirtualKeyItem(
				code,
				pixNormal,
				pixShift,
				pixGrph,
				pixKana,
				pixKanaShift,
				pixKKana,
				pixKKanaShift);
	//#TODO watcherとitem間の通知をconnect

	addItem(item);
	return item;
}

void VirtualKeyboardScene::alignVirtualKeyItems(std::vector<VirtualKeyItem *> list, QPointF coord)
{
	//アイテムを横一列に整列
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
