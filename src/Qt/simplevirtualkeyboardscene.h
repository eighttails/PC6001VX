#ifndef SIMPLEVIRTUALKEYBOARDSCENE_H
#define SIMPLEVIRTUALKEYBOARDSCENE_H

#include "virtualkeyboardscene.h"

//簡易版仮想キーボード用シーングラフ
class SimpleVirtualKeyboardScene : public VirtualKeyboardScene

{
	Q_OBJECT
public:
	explicit SimpleVirtualKeyboardScene(QObject* parent = nullptr);

protected:
	void construct();
};

#endif // SIMPLEVIRTUALKEYBOARDSCENE_H
