#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>
#include "../typedef.h"
class cConfig;

// 色選択用のボタン
class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget *parent = 0);

signals:

public slots:
public:
    void initialize(int id, cConfig* cfg);
    // ボタンサイズは30x20を基準とする
    virtual QSize sizeHint () const {return QSize (30, 20);}

    const COLOR24& getColor() const {return color;};

private slots:
    // 色選択ダイアログを開く
    void chooseColor();
private:
    void updateColor();
    int colorId;
    cConfig* config;
    COLOR24 color;
};

#endif // COLORBUTTON_H
