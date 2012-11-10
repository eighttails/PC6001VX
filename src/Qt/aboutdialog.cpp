#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "../config.h"
#include "../pc6001v.h"

AboutDialog::AboutDialog(CFG6* cfg, QWidget *parent) :
    QDialog(parent),
    config(cfg),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    switch( config->GetModel() ){
    case 60:    ui->labelIcon->setPixmap(QPixmap(":/res/PC-6001.ico"));      break;
    case 62:    ui->labelIcon->setPixmap(QPixmap(":/res/PC-6001mk2.ico"));   break;
    case 64:    ui->labelIcon->setPixmap(QPixmap(":/res/PC-6001mk2SR.ico")); break;
    case 66:    ui->labelIcon->setPixmap(QPixmap(":/res/PC-6601.ico"));      break;
    case 68:    ui->labelIcon->setPixmap(QPixmap(":/res/PC-6601SR.ico"));    break;
    default:;
    }
    ui->labelAppName->setText(QString::fromUtf8(APPNAME));
    ui->labelVersion->setText(QString::fromUtf8(VERSION));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
