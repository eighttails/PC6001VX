#include "normalvirtualkeyboardscene.h"

NormalVirtualKeyboardScene::NormalVirtualKeyboardScene(QObject *parent) : VirtualKeyboardScene(parent)
{
	construct();
}

void NormalVirtualKeyboardScene::construct()
{
	const int unitHeight = 130;
	int i = 0;
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_END, "stop", "stop", "stop", "stop", "stop", "stop", "stop"));
		list.push_back(createVirtualKeyItem(KVC_PAGEDOWN, "mode", "mode", "mode", "mode", "mode", "mode", "mode"));
		list.push_back(createVirtualKeyItem(KVC_F1, "function01", "function06", "function01", "function01", "function06", "function01", "function06"));
		list.push_back(createVirtualKeyItem(KVC_F2, "function02", "function07", "function02", "function02", "function07", "function02", "function07"));
		list.push_back(createVirtualKeyItem(KVC_F3, "function03", "function08", "function03", "function03", "function08", "function03", "function08"));
		list.push_back(createVirtualKeyItem(KVC_F4, "function04", "function09", "function04", "function04", "function09", "function04", "function09"));
		list.push_back(createVirtualKeyItem(KVC_F5, "function05", "function10", "function05", "function05", "function10", "function05", "function10"));
		list.push_back(createVirtualKeyItem(KVC_HOME, "home", "home", "home", "home", "home", "home", "home"));
		list.push_back(createVirtualKeyItem(KVC_PAGEUP, "page", "page", "page", "page", "page", "page", "page"));
		list.push_back(createVirtualKeyItem(KVC_DELETE, "del", "del", "del", "del", "del", "del", "del"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_ESC, "escape", "escape", "escape", "escape", "escape", "escape", "escape"));
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
		list.push_back(createVirtualKeyItem(KVC_MINUS, "0x2d", "0x3d", "0x17", "0xee", "blank", "0xce", "blank"));
		list.push_back(createVirtualKeyItem(KVC_CARET, "0x5e", "blank", "blank", "0xed", "blank", "0xcd", "blank"));
		list.push_back(createVirtualKeyItem(KVC_YEN, "0x5c", "blank", "0x09", "0xb0", "blank", "0xb0", "blank"));
		list.push_back(createVirtualKeyItem(KVC_INSERT, "ins", "ins", "ins", "ins", "ins", "ins", "ins"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_TAB, "tab", "tab", "tab", "tab", "tab", "tab", "tab"));
		list.push_back(createVirtualKeyItem(KVC_Q, "0x71", "0x51", "blank", "0xe0", "blank", "0xc0", "blank"));
		list.push_back(createVirtualKeyItem(KVC_W, "0x77", "0x57", "blank", "0xe3", "blank", "0xc3", "blank"));
		list.push_back(createVirtualKeyItem(KVC_E, "0x65", "0x45", "0x18", "0x92", "0x88", "0xb2", "0xa8"));
		list.push_back(createVirtualKeyItem(KVC_R, "0x72", "0x52", "0x12", "0x9d", "blank", "0xbd", "blank"));
		list.push_back(createVirtualKeyItem(KVC_T, "0x74", "0x54", "0x19", "0x96", "blank", "0xb6", "blank"));
		list.push_back(createVirtualKeyItem(KVC_Y, "0x79", "0x59", "0x08", "0xfd", "blank", "0xdd", "blank"));
		list.push_back(createVirtualKeyItem(KVC_U, "0x75", "0x55", "blank", "0xe5", "blank", "0xc5", "blank"));
		list.push_back(createVirtualKeyItem(KVC_I, "0x69", "0x49", "0x16", "0xe6", "blank", "0xc6", "blank"));
		list.push_back(createVirtualKeyItem(KVC_O, "0x6f", "0x4f", "blank", "0xf7", "blank", "0xd7", "blank"));
		list.push_back(createVirtualKeyItem(KVC_P, "0x70", "0x50", "0x10", "0x9e", "blank", "0xbe", "blank"));
		list.push_back(createVirtualKeyItem(KVC_AT, "0x40", "blank", "blank", "0xde", "blank", "0xde", "blank"));
		list.push_back(createVirtualKeyItem(KVC_LBRACKET, "0x5b", "blank", "0x84", "0xdf", "0xa2", "0xdf", "0xa2"));
		list.push_back(createVirtualKeyItem(KVC_RBRACKET, "0x5d", "blank", "0x85", "0xf1", "0xa3", "0xd1", "0xa3"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	//	list.push_back(createVirtualKeyItem(KVC_LSHIFT, "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", true));
	//	list.push_back(createVirtualKeyItem(KVC_ENTER, "return", "return", "return", "return", "return", "return", "return"));

	setSceneRect(itemsBoundingRect());

}
