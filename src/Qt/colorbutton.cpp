#include <QColorDialog>

#include "colorbutton.h"
#include "../config.h"

ColorButton::ColorButton(QWidget *parent) :
	QPushButton(parent),
	colorId(0),
	config(NULL)
{
	// ボタンをクリックしたら色選択ダイアログを開く
	connect(this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

void ColorButton::updateColor()
{
	QString styleSheet = QString("background-color: rgb(%1, %2, %3);"
								 "border-style:solid;"
								 "border-color: rgb(0, 0, 0);"
								 "border-width: 1px;"
								 "border-radius: 1;").arg(color.r).arg(color.g).arg(color.b);
	setStyleSheet(styleSheet);
}

void ColorButton::initialize(int id, CFG6 *cfg)
{
	Q_ASSERT(cfg);
	config = cfg;
	colorId = id;

	// 設定情報から自ボタンに対応する色を取得
	config->GetColor(colorId, &color);
	// 自身の色に反映
	updateColor();
}

void ColorButton::chooseColor()
{
	QColor oldColor(color.r, color.g, color.b);
	QColor newColor = QColorDialog::getColor(oldColor);
	// キャンセルが押された場合、isValidはfalseになる
	if(newColor.isValid()){
		// 表示の更新
		int iR = 0, iG = 0, iB = 0;
		newColor.getRgb(&iR, &iG, &iB);
		color.r = iR;
		color.g = iG;
		color.b = iB;

		updateColor();
	}
}

