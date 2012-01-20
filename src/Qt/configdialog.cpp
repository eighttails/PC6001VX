#include "configdialog.h"
#include "ui_configdialog.h"

#include "../config.h"

ConfigDialog::ConfigDialog(cConfig* cfg, QWidget *parent) :
    QDialog(parent),
    config(cfg),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}
