#ifndef KEYPANELBUTTON_H
#define KEYPANELBUTTON_H

#include <QPushButton>
#include "../keydef.h"

// 補助キーパネル用ボタン
class KeyPanelButton : public QPushButton
{
	Q_OBJECT
public:
	explicit KeyPanelButton(QWidget *parent, const QString & text, PCKEYsym key);

signals:

public slots:
private slots:
	void emitKey();
private:
	PCKEYsym KeySym;
};

#endif // KEYPANELBUTTON_H
