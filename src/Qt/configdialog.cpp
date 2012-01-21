#include "configdialog.h"
#include "ui_configdialog.h"

#include "../config.h"

ConfigDialog::ConfigDialog(cConfig* cfg, QWidget *parent) :
    QDialog(parent),
    config(cfg),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    assignColorButton();
    readConfig();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::readConfig()
{
}

void ConfigDialog::writeConfig()
{
}

// 色設定ボタンと設定項目を対応させる
void ConfigDialog::assignColorButton()
{
    // 16〜64の色IDに対応させる。
    for (int id = 16; id <= 64; id++){
        QString buttonName = QString("pushButtonColor%1").arg(id);
        // ダイアログから動的に部品を取得する
        ColorButton* button = this->findChild<ColorButton*>(buttonName);
        button->initialize(id, config);
    }
}

