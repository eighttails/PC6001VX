#ifndef EL6EX_H
#define EL6EX_H

#include <QObject>
#include <QRgb>
#include "../p6el.h"
class KEY6;

class QtEL6 : public QObject, public EL6
{
	Q_OBJECT
public:
	QtEL6(QObject* parent);
	virtual ~QtEL6();
	void ShowPopupImpl(int x, int y);
	bool GetPauseEnable();
	void SetPauseEnable(bool en);
	std::shared_ptr<KEY6> GetKeyboard();
	bool IsMonitor();
	void SetPaletteTable(QVector<QRgb>& palette, int scanbr);
	int SoundReadySize();

	// 親クラスのメソッドをpublicでオーバーライド
	void UI_DokoSave( int slot );
	void UI_DokoLoad( int slot, bool ask = false );
public slots:
	void UpdateFPS();
};

#endif // EL6EX_H
