#include "simplevirtualkeyboardscene.h"
#include "virtualkeyitem.h"
#include "virtualstickitem.h"

#include <QRect>

SimpleVirtualKeyboardScene::SimpleVirtualKeyboardScene(QObject *parent) : VirtualKeyboardScene(parent)
{
	construct();
}

void SimpleVirtualKeyboardScene::construct()
{
	const int unitWidth = 100;
	const int unitHeight = 130;
	int i = 0;
	{
		createVirtualKeyItem(KVC_MUHENKAN, "save", "save", "save", "save", "save", "save", "save")->setPos(unitWidth * 0, unitHeight * i);
		createVirtualKeyItem(KVC_F9, "pause", "pause", "pause", "pause", "pause", "pause", "pause")->setPos(unitWidth * 2.75, unitHeight * i);
		createVirtualKeyItem(KVC_F12, "snapshot", "snapshot", "snapshot", "snapshot", "snapshot", "snapshot", "snapshot")->setPos(unitWidth * 5, unitHeight * i);
		createVirtualKeyItem(KVC_HENKAN, "load", "load", "load", "load", "load", "load", "load")->setPos(unitWidth * 7.75, unitHeight * i);
		i++;
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_1, "0x31", "0x21", "0x07", "0xe7", "blank", "0xc7", "blank"));
		list.push_back(createVirtualKeyItem(KVC_2, "0x32", "0x22", "0x01", "0xec", "blank", "0xcc", "blank"));
		list.push_back(createVirtualKeyItem(KVC_3, "0x33", "0x23", "0x02", "0x91", "0x87", "0xb1", "0xa7"));
		list.push_back(createVirtualKeyItem(KVC_4, "0x34", "0x24", "0x03", "0x93", "0x89", "0xb3", "0xa9"));
		list.push_back(createVirtualKeyItem(KVC_5, "0x35", "0x25", "0x04", "0x94", "0x8a", "0xb4", "0xaa"));
		list.push_back(createVirtualKeyItem(KVC_6, "0x36", "0x26", "0x05", "0x95", "0x8b", "0xb5", "0xab"));
		list.push_back(createVirtualKeyItem(KVC_7, "0x37", "0x27", "0x06", "0xf4", "0x8c", "0xd4", "0xac"));
		list.push_back(createVirtualKeyItem(KVC_8, "0x38", "0x28", "0x0d", "0xf5", "0x8d", "0xd5", "0xad"));
		list.push_back(createVirtualKeyItem(KVC_9, "0x39", "0x29", "0x0e", "0xf6", "0x8e", "0xd6", "0xae"));
		list.push_back(createVirtualKeyItem(KVC_0, "0x30", "blank", "0x0f", "0xfc", "0x86", "0xdc", "0xa6"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
		i++;
	}
	{
		// 方向キー
		auto stick = new VirtualStickItem();
		stick->setPos(unitWidth * 0, unitHeight * i);
		addItem(stick);

		createVirtualKeyItem(KVC_LSHIFT, "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", false, true)->setPos(unitWidth * 7.75, unitHeight * i);
		i++;
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_Y, "0x79", "0x59", "0x08", "0xfd", "blank", "0xdd", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_N, "0x6e", "0x4e", "blank", "0xf0", "blank", "0xd0", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_ENTER, "ret_flat", "ret_flat", "ret_flat", "ret_flat", "ret_flat", "ret_flat", "ret_flat"));
		alignVirtualKeyItems(list, QPointF(unitWidth * 4, unitHeight * i++));
	}
	{
		createVirtualKeyItem(KVC_SPACE, "space_small", "space_small", "space_small", "space_small", "space_small", "space_small", "space_small")->setPos(unitWidth * 7.75, unitHeight * i);
	}
	// 下側に少し隙間を開ける
	auto rect = itemsBoundingRect();
	rect.setHeight(rect.height() + unitHeight * 0.5);
	setSceneRect(rect);
}


