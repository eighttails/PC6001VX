#include "keystatewatcher.h"

#include <QTimer>
#include "../keyboard.h"
#include "../p6vxcommon.h"

KeyStateWatcher::KeyStateWatcher(std::shared_ptr<KEY6> key, QObject *parent)
	: QObject(parent)
	, Key(key)
	, ON_SHIFT(false)
	, ON_GRAPH(false)
	, ON_KANA(false)
	, ON_KKANA(false)
	, ON_CAPS(false)
{
	auto timer = new QTimer(this);
	timer->setInterval(1000 / 60);
	connect(timer, SIGNAL(timeout()), this, SLOT(poll()));
	timer->start();
}

int KeyStateWatcher::GetModifierStatus()
{
	QMutexLocker lock(&mutex);
	int ret = 0;
	if (ON_SHIFT)	ret |= SHIFT;
	if (ON_GRAPH)	ret |= GRAPH;
	if (ON_KANA)	ret |= KANA;
	if (ON_KKANA)	ret |= KKANA;
	if (ON_CAPS)	ret |= CAPS;
	if (ON_ROMAJI)	ret |= ROMAJI;

	return ret;
}

void KeyStateWatcher::poll()
{
	QMutexLocker lock(&mutex);
	bool changed = false;
	auto keyStatus = Key->GetKeyIndicator();
	if(bool(keyStatus & KI_SHIFT) != this->ON_SHIFT)	changed = true;
	this->ON_SHIFT = bool(keyStatus & KI_SHIFT);

	if(bool(keyStatus & KI_CTRL) != this->ON_CTRL)		changed = true;
	this->ON_CTRL = bool(keyStatus & KI_CTRL);

	if(bool(keyStatus & KI_GRAPH) != this->ON_GRAPH)	changed = true;
	this->ON_GRAPH = bool(keyStatus & KI_GRAPH);

	if(bool(keyStatus & KI_KANA) != this->ON_KANA)		changed = true;
	this->ON_KANA = bool(keyStatus & KI_KANA);

	if(bool(keyStatus & KI_KKANA) != this->ON_KKANA)	changed = true;
	this->ON_KKANA = bool(keyStatus & KI_KKANA);

	if(bool(keyStatus & KI_CAPS) != this->ON_CAPS)		changed = true;
	this->ON_CAPS = bool(keyStatus & KI_CAPS);

	if(bool(keyStatus & KI_ROMAJI) != this->ON_ROMAJI)	changed = true;
	this->ON_ROMAJI = bool(keyStatus & KI_ROMAJI);

	auto joyKeyStatus = Key->GetKeyJoy();
	if (joyKeyStatus & 0b00100000){
		TiltScreen(TiltDirection::LEFT);
	} else if (joyKeyStatus & 0b00010000){
		TiltScreen(TiltDirection::RIGHT);
	} else {
		TiltScreen(TiltDirection::NEWTRAL);
	}
	UpdateTilt();

	if (changed){
		emit stateChanged(
					ON_SHIFT,
					ON_CTRL,
					ON_GRAPH,
					ON_KANA,
					ON_KKANA,
					ON_CAPS,
					ON_ROMAJI);
	}
}
