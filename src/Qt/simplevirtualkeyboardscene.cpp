#include "simplevirtualkeyboardscene.h"
#include "virtualkeyitem.h"


SimpleVirtualKeyboardScene::SimpleVirtualKeyboardScene(KeyStateWatcher *watcher, QObject *parent) : VirtualKeyboardScene(watcher, parent)
{
	construct();
}

void SimpleVirtualKeyboardScene::construct()
{
	std::vector<VirtualKeyItem*> list;
	list.push_back(createVirtualKeyItem(KVC_LSHIFT, "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", true));
	list.push_back(createVirtualKeyItem(KVC_1, "0x31", "0x21", "0x07", "0xe7", "blank", "0xc7", "blank"));
	list.push_back(createVirtualKeyItem(KVC_2, "0x32", "0x22", "0x01", "0xec", "blank", "0xcc", "blank"));
	list.push_back(createVirtualKeyItem(KVC_ENTER, "ret_flat", "ret_flat", "ret_flat", "ret_flat", "ret_flat", "ret_flat", "ret_flat"));
	alignVirtualKeyItems(list, QPointF(0, 0));


	setSceneRect(itemsBoundingRect());
}


