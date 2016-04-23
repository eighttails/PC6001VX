#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>
#include "../vsurface.h"
class CFG6;

// 色選択用のボタン
class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget *parent = 0);

signals:

public slots:
public:
    void initialize(int id, CFG6* cfg);

    const COLOR24& getColor() const {return color;};

private slots:
    // 色選択ダイアログを開く
    void chooseColor();
private:
    void updateColor();
    int colorId;
    CFG6* config;
    COLOR24 color;
};

#endif // COLORBUTTON_H
