#ifndef VIRTUALKEYBOARDSCENE_H
#define VIRTUALKEYBOARDSCENE_H

#include <QGraphicsScene>

#include <vector>
#include "typedef.h"
#include "../keydef.h"

class KeyStateWatcher;
class VirtualKeyItem;

class VirtualKeyboardScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit VirtualKeyboardScene(QObject* parent = nullptr);
	void setKeyStateWatcher(KeyStateWatcher* watcher);
protected:
	VirtualKeyItem* createVirtualKeyItem(PCKEYsym code, PCKEYmod mod,
										 QString pixNormal,
										 QString pixShift,
										 QString pixGrph,
										 QString pixKana,
										 QString pixKanaShift,
										 QString pixKKana,
										 QString pixKKanaShift,
										 bool isAlpha = false,
										 bool mouseToggle = false);

	void alignVirtualKeyItems(std::vector<VirtualKeyItem*> list, QPointF coord);

	KeyStateWatcher* Watcher;
};

#endif // VIRTUALKEYBOARDSCENE_H
