#ifndef KEYPANEL_H
#define KEYPANEL_H

#include <QWidget>

// 使用しているキーボードにないキーを入力するための
// 補助キーパネル
class KeyPanel : public QWidget
{
	Q_OBJECT
public:
	explicit KeyPanel(QWidget *parent = 0);
signals:

public slots:

	// QWidget interface
protected:
	virtual void moveEvent(QMoveEvent *);
	virtual void showEvent(QShowEvent *);
	virtual void closeEvent(QCloseEvent *);
};

#endif // KEYPANEL_H
