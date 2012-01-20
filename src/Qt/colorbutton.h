#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

// 色選択用のボタン
class ColorButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ColorButton(QWidget *parent = 0);

signals:

public slots:
public:
    void setColor(QColor color);
    // ボタンサイズは20x20を基準とする
    virtual QSize sizeHint () const {return QSize (20, 20);}
};

#endif // COLORBUTTON_H
