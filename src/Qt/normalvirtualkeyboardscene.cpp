#include "normalvirtualkeyboardscene.h"
#include "virtualkeyitem.h"

NormalVirtualKeyboardScene::NormalVirtualKeyboardScene(QObject *parent) : VirtualKeyboardScene(parent)
{
	construct();
}

void NormalVirtualKeyboardScene::construct()
{
	const int unitWidth = 100;
	const int unitHeight = 130;
	int i = 0;
	{
		createVirtualKeyItem(KVC_MUHENKAN, KVM_NONE, "save", "save", "save", "save", "save", "save", "save")->setPos(unitWidth * 0, unitHeight * i);
		createVirtualKeyItem(KVC_F9, KVM_NONE, "pause", "pause", "pause", "pause", "pause", "pause", "pause")->setPos(unitWidth * 2.25, unitHeight * i);
		createVirtualKeyItem(KVC_F12, KVM_LALT, "roma", "roma", "roma", "roma", "roma", "roma", "roma")->setPos(unitWidth * 8.75, unitHeight * i);
		createVirtualKeyItem(KVC_F12, KVM_NONE, "snapshot", "snapshot", "snapshot", "snapshot", "snapshot", "snapshot", "snapshot")->setPos(unitWidth * 11, unitHeight * i);
		createVirtualKeyItem(KVC_HENKAN, KVM_NONE, "load", "load", "load", "load", "load", "load", "load")->setPos(unitWidth * 13.25, unitHeight * i);
		i++;
		i++;
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_END, KVM_NONE, "stop", "stop", "stop", "stop", "stop", "stop", "stop"));
		list.push_back(createVirtualKeyItem(KVC_PAGEDOWN, KVM_NONE, "mode", "mode", "mode", "mode", "mode", "mode", "mode"));
		list.push_back(createVirtualKeyItem(KVC_F1, KVM_NONE, "function01", "function06", "function01", "function01", "function06", "function01", "function06"));
		list.push_back(createVirtualKeyItem(KVC_F2, KVM_NONE, "function02", "function07", "function02", "function02", "function07", "function02", "function07"));
		list.push_back(createVirtualKeyItem(KVC_F3, KVM_NONE, "function03", "function08", "function03", "function03", "function08", "function03", "function08"));
		list.push_back(createVirtualKeyItem(KVC_F4, KVM_NONE, "function04", "function09", "function04", "function04", "function09", "function04", "function09"));
		list.push_back(createVirtualKeyItem(KVC_F5, KVM_NONE, "function05", "function10", "function05", "function05", "function10", "function05", "function10"));
		list.push_back(createVirtualKeyItem(KVC_HOME, KVM_NONE, "home", "home", "home", "home", "home", "home", "home"));
		list.push_back(createVirtualKeyItem(KVC_PAGEUP, KVM_NONE, "page", "page", "page", "page", "page", "page", "page"));
		list.push_back(createVirtualKeyItem(KVC_DELETE, KVM_NONE, "del", "del", "del", "del", "del", "del", "del"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_ESC, KVM_NONE, "escape", "escape", "escape", "escape", "escape", "escape", "escape"));
		list.push_back(createVirtualKeyItem(KVC_1, KVM_NONE, "0x31", "0x21", "0x07", "0xe7", "blank", "0xc7", "blank"));
		list.push_back(createVirtualKeyItem(KVC_2, KVM_NONE, "0x32", "0x22", "0x01", "0xec", "blank", "0xcc", "blank"));
		list.push_back(createVirtualKeyItem(KVC_3, KVM_NONE, "0x33", "0x23", "0x02", "0x91", "0x87", "0xb1", "0xa7"));
		list.push_back(createVirtualKeyItem(KVC_4, KVM_NONE, "0x34", "0x24", "0x03", "0x93", "0x89", "0xb3", "0xa9"));
		list.push_back(createVirtualKeyItem(KVC_5, KVM_NONE, "0x35", "0x25", "0x04", "0x94", "0x8a", "0xb4", "0xaa"));
		list.push_back(createVirtualKeyItem(KVC_6, KVM_NONE, "0x36", "0x26", "0x05", "0x95", "0x8b", "0xb5", "0xab"));
		list.push_back(createVirtualKeyItem(KVC_7, KVM_NONE, "0x37", "0x27", "0x06", "0xf4", "0x8c", "0xd4", "0xac"));
		list.push_back(createVirtualKeyItem(KVC_8, KVM_NONE, "0x38", "0x28", "0x0d", "0xf5", "0x8d", "0xd5", "0xad"));
		list.push_back(createVirtualKeyItem(KVC_9, KVM_NONE, "0x39", "0x29", "0x0e", "0xf6", "0x8e", "0xd6", "0xae"));
		list.push_back(createVirtualKeyItem(KVC_0, KVM_NONE, "0x30", "blank", "0x0f", "0xfc", "0x86", "0xdc", "0xa6"));
		list.push_back(createVirtualKeyItem(KVC_MINUS, KVM_NONE, "0x2d", "0x3d", "0x17", "0xee", "blank", "0xce", "blank"));
		list.push_back(createVirtualKeyItem(KVC_CARET, KVM_NONE, "0x5e", "blank", "blank", "0xed", "blank", "0xcd", "blank"));
		list.push_back(createVirtualKeyItem(KVC_YEN, KVM_NONE, "0x5c", "blank", "0x09", "0xb0", "blank", "0xb0", "blank"));
		list.push_back(createVirtualKeyItem(KVC_INSERT, KVM_NONE, "ins", "ins", "ins", "ins", "ins", "ins", "ins"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	auto returnKey = createVirtualKeyItem(KVC_ENTER, KVM_NONE, "return", "return", "return", "return", "return", "return", "return");
	returnKey->setPos(unitWidth * 13.75, unitHeight * i);
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_TAB, KVM_NONE, "tab", "tab", "tab", "tab", "tab", "tab", "tab"));
		list.push_back(createVirtualKeyItem(KVC_Q, KVM_NONE, "0x71", "0x51", "blank", "0xe0", "blank", "0xc0", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_W, KVM_NONE, "0x77", "0x57", "blank", "0xe3", "blank", "0xc3", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_E, KVM_NONE, "0x65", "0x45", "0x18", "0x92", "0x88", "0xb2", "0xa8", true));
		list.push_back(createVirtualKeyItem(KVC_R, KVM_NONE, "0x72", "0x52", "0x12", "0x9d", "blank", "0xbd", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_T, KVM_NONE, "0x74", "0x54", "0x19", "0x96", "blank", "0xb6", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_Y, KVM_NONE, "0x79", "0x59", "0x08", "0xfd", "blank", "0xdd", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_U, KVM_NONE, "0x75", "0x55", "blank", "0xe5", "blank", "0xc5", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_I, KVM_NONE, "0x69", "0x49", "0x16", "0xe6", "blank", "0xc6", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_O, KVM_NONE, "0x6f", "0x4f", "blank", "0xf7", "blank", "0xd7", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_P, KVM_NONE, "0x70", "0x50", "0x10", "0x9e", "blank", "0xbe", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_AT, KVM_NONE, "0x40", "blank", "blank", "0xde", "blank", "0xde", "blank"));
		list.push_back(createVirtualKeyItem(KVC_LBRACKET, KVM_NONE, "0x5b", "blank", "0x84", "0xdf", "0xa2", "0xdf", "0xa2"));
		list.push_back(createVirtualKeyItem(KVC_RBRACKET, KVM_NONE, "0x5d", "blank", "0x85", "0xf1", "0xa3", "0xd1", "0xa3"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_LCTRL, KVM_NONE, "ctrl", "ctrl", "ctrl", "ctrl", "ctrl", "ctrl", "ctrl", false, true));
		list.push_back(createVirtualKeyItem(KVC_A, KVM_NONE, "0x61", "0x41", "blank", "0xe1", "blank", "0xc1", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_S, KVM_NONE, "0x73", "0x53", "0x0c", "0xe4", "blank", "0xc4", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_D, KVM_NONE, "0x64", "0x44", "0x14", "0x9c", "blank", "0xbc", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_F, KVM_NONE, "0x66", "0x46", "0x15", "0xea", "blank", "0xca", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_G, KVM_NONE, "0x67", "0x47", "0x13", "0x97", "blank", "0xb7", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_H, KVM_NONE, "0x68", "0x48", "0x0a", "0x98", "blank", "0xb8", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_J, KVM_NONE, "0x6a", "0x4a", "blank", "0xef", "blank", "0xcf", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_K, KVM_NONE, "0x6b", "0x4b", "blank", "0xe9", "blank", "0xc9", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_L, KVM_NONE, "0x6c", "0x4c", "0x1e", "0xf8", "blank", "0xd8", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_SEMICOLON, KVM_NONE, "0x3b", "0x2b", "0x82", "0xfa", "blank", "0xda", "blank"));
		list.push_back(createVirtualKeyItem(KVC_COLON, KVM_NONE, "0x3a", "0x2a", "0x81", "0x99", "blank", "0xb9", "blank"));
		list.push_back(createVirtualKeyItem(KVC_UNDERSCORE, KVM_NONE, "blank", "0x5f", "0x83", "0xfb", "blank", "0xdb", "blank"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_LSHIFT, KVM_NONE, "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", "shift_l", false, true));
		list.push_back(createVirtualKeyItem(KVC_Z, KVM_NONE, "0x7a", "0x5a", "blank", "0xe2", "0x8f", "0xc2", "0xaf", true));
		list.push_back(createVirtualKeyItem(KVC_X, KVM_NONE, "0x78", "0x58", "0x1c", "0x9b", "blank", "0xbb", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_C, KVM_NONE, "0x63", "0x43", "0x1a", "0x9f", "blank", "0xbf", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_V, KVM_NONE, "0x76", "0x56", "0x11", "0xeb", "blank", "0xcb", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_B, KVM_NONE, "0x62", "0x42", "0x1b", "0x9a", "blank", "0xba", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_N, KVM_NONE, "0x6e", "0x4e", "blank", "0xf0", "blank", "0xd0", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_M, KVM_NONE, "0x6d", "0x4d", "0x0b", "0xf3", "blank", "0xd3", "blank", true));
		list.push_back(createVirtualKeyItem(KVC_COMMA, KVM_NONE, "0x2c", "0x3c", "0x1f", "0xe8", "0xa4", "0xc8", "0xa4"));
		list.push_back(createVirtualKeyItem(KVC_PERIOD, KVM_NONE, "0x2e", "0x3e", "0x1d", "0xf9", "0xa1", "0xd9", "0xa1"));
		list.push_back(createVirtualKeyItem(KVC_SLASH, KVM_NONE, "0x2f", "0x3f", "0x80", "0xf2", "0xa5", "0xd2", "0xa5"));
		list.push_back(createVirtualKeyItem(KVC_RSHIFT, KVM_NONE, "shift_r", "shift_r", "shift_r", "shift_r", "shift_r", "shift_r", "shift_r", false, true));
		list.push_back(createVirtualKeyItem(KVC_UP, KVM_NONE, "up", "up", "up", "up", "up", "up", "up"));
		list.push_back(createVirtualKeyItem(KVC_HIRAGANA, KVM_NONE, "kana", "kana", "kana", "kana", "kana", "kana", "kana"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	{
		std::vector<VirtualKeyItem*> list;
		list.push_back(createVirtualKeyItem(KVC_SCROLLLOCK, KVM_NONE, "caps", "caps", "caps", "caps", "caps", "caps", "caps"));
		list.push_back(createVirtualKeyItem(KVC_LALT, KVM_NONE, "grph", "grph", "grph", "grph", "grph", "grph", "grph", false, true));
		list.push_back(createVirtualKeyItem(KVC_SPACE, KVM_NONE, "space", "space", "space", "space", "space", "space", "space"));
		list.push_back(createVirtualKeyItem(KVC_LEFT, KVM_NONE, "left", "left", "left", "left", "left", "left", "left"));
		list.push_back(createVirtualKeyItem(KVC_DOWN, KVM_NONE, "down", "down", "down", "down", "down", "down", "down"));
		list.push_back(createVirtualKeyItem(KVC_RIGHT, KVM_NONE, "right", "right", "right", "right", "right", "right", "right"));
		alignVirtualKeyItems(list, QPointF(0, unitHeight * i++));
	}
	// 下側に少し隙間を開ける
	auto rect = itemsBoundingRect();
	rect.setHeight(rect.height() + unitHeight * 0.5);
	setSceneRect(rect);
}
