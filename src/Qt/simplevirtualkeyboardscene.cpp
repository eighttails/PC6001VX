#include "simplevirtualkeyboardscene.h"
#include "virtualkeyitem.h"


SimpleVirtualKeyboardScene::SimpleVirtualKeyboardScene(KeyStateWatcher *watcher, QObject *parent) : VirtualKeyboardScene(watcher, parent)
{
	construct();
}

void SimpleVirtualKeyboardScene::construct()
{
	std::vector<VirtualKeyItem*> list;
	list.push_back(createVirtualKeyItem(KVC_1, "0x31", "0x21", "0x07", "0xe7", "0xc7"));
	list.push_back(createVirtualKeyItem(KVC_2, "0x32", "0x22", "0x01", "0xec", "0xcc"));

	alignVirtualKeyItems(list, QPointF(0, 0));

	createVirtualKeyItem(KVC_ENTER, "return", "return", "return", "return", "return")->setPos(300,0);
	setSceneRect(itemsBoundingRect());
}


