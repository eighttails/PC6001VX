#include "colorbutton.h"

ColorButton::ColorButton(QWidget *parent) :
    QPushButton(parent)
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
