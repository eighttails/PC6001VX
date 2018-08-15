#ifndef NORMALVIRTUALKEYBOARDSCENE_H
#define NORMALVIRTUALKEYBOARDSCENE_H

#include "virtualkeyboardscene.h"

//通常版仮想キーボード用シーングラフ
class NormalVirtualKeyboardScene : public VirtualKeyboardScene
{
	Q_OBJECT
public:
	NormalVirtualKeyboardScene(QObject* parent = nullptr);

protected:
	void construct();
};

#endif // NORMALVIRTUALKEYBOARDSCENE_H
