#include <QColorDialog>

#include "colorbutton.h"
#include "../config.h"

ColorButton::ColorButton(QWidget *parent) :
    QPushButton(parent),
    colorId(0),
    config(NULL)
{
}

void ColorButton::updateColor()
{
    int r = 0, g = 0, b = 0;
    color.getRgb(&r, &g, &b);
    QString styleSheet = QString("background-color: rgb(%1, %2, %3);"
                                 "border-style:solid;"
                                 "border-color: rgb(0, 0, 0);"
                                 "border-width: 1px;"
                                 "border-radius: 1;").arg(r).arg(g).arg(b);
    setStyleSheet(styleSheet);
}

void ColorButton::initialize(int id, cConfig *cfg)
{
    Q_ASSERT(cfg);
    config = cfg;
    colorId = id;

    // 設定情報から自ボタンに対応する色を取得
    COLOR24 col;
    config->GetColor(colorId, &col);
    color = QColor(col.r, col.g, col.b);
    // 自身の色に反映
    updateColor();

    // ボタンをクリックしたら色選択ダイアログを開く
    connect(this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

void ColorButton::chooseColor()
{
    QColor newColor = QColorDialog::getColor(color);
    // キャンセルが押された場合、isValidはfalseになる
    if(newColor.isValid()){
        // 表示の更新
        color = newColor;
        updateColor();
    }
}

const COLOR24& ColorButton::getColor() const
{
    COLOR24 col;
    col.r = color.red();
    col.g = color.green();
    col.b = color.blue();
    return col;
}
