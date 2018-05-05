#ifndef SIMPLEVIRTUALKEYBOARDSCENE_H
#define SIMPLEVIRTUALKEYBOARDSCENE_H

#include "virtualkeyboardscene.h"

class SimpleVirtualKeyboardScene : public VirtualKeyboardScene

{
	Q_OBJECT
public:
	explicit SimpleVirtualKeyboardScene(QObject* parent = nullptr);

signals:

public slots:

protected:
	void construct();
};

#endif // SIMPLEVIRTUALKEYBOARDSCENE_H
