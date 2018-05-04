#ifndef VIRTUALKEYBOARDSCENE_H
#define VIRTUALKEYBOARDSCENE_H

#include <QGraphicsScene>

#include <vector>
#include "../keydef.h"

class KeyStateWatcher;
class VirtualKeyItem;

class VirtualKeyboardScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit VirtualKeyboardScene(KeyStateWatcher* watcher, QObject* parent = nullptr);

protected:
	VirtualKeyItem* createVirtualKeyItem(
			PCKEYsym code,
			QString pixNormal,
			QString pixShift,
			QString pixGrph,
			QString pixKana,
			QString pixKKana
			);

	void alignVirtualKeyItems(std::vector<VirtualKeyItem*> list, QPointF coord);

	KeyStateWatcher* Watcher;
};

#endif // VIRTUALKEYBOARDSCENE_H
