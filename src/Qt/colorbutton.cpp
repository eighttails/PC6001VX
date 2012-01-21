#include "colorbutton.h"
#include "../config.h"
ColorButton::ColorButton(QWidget *parent) :
    QPushButton(parent),
    colorId(0),
    config(NULL)
{
    setColor(QColor(0, 0, 0));
}

void ColorButton::setColor(QColor color)
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
    QColor qcol(col.r, col.g, col.b);
    // 自身の色に反映
    setColor(qcol);
}
