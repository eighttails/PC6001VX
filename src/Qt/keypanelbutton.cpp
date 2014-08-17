#include <QApplication>
#include "keypanelbutton.h"
#include "../osd.h"


KeyPanelButton::KeyPanelButton(QWidget* parent, const QString &text, PCKEYsym key)
	: QPushButton(text, parent)
	, KeySym(key)
{
	connect(this, SIGNAL(clicked(bool)), this, SLOT(emitKey()));
}

void KeyPanelButton::emitKey()
{
	Event ev;
	ev.key.state = true;
	ev.type = EV_KEYDOWN;
	ev.key.sym = KeySym;
	// モディファイヤキーは物理キーボードから拾う
	Qt::KeyboardModifiers mod = QApplication::keyboardModifiers();
	ev.key.mod = PCKEYmod((mod & Qt::ShiftModifier ? KVM_SHIFT : KVM_NONE)
			& (mod & Qt::AltModifier ? KVM_ALT : KVM_NONE)
			& (mod & Qt::ControlModifier ? KVM_CTRL : KVM_NONE));

	// キー押下イベントを送信
	OSD_PushEvent(ev);
}

