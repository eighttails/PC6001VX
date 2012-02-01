#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "../config.h"
#include "../pc6001v.h"

AboutDialog::AboutDialog(cConfig* cfg, QWidget *parent) :
    QDialog(parent),
    config(cfg),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    switch( config->GetModel() ){
    case 60:    ui->labelIcon->setPixmap(QPixmap(":/res/PC6001.ico"));      break;
    case 62:    ui->labelIcon->setPixmap(QPixmap(":/res/PC60mk2.ico"));   break;
    case 64:    ui->labelIcon->setPixmap(QPixmap(":/res/PC60mk2SR.ico")); break;
    case 66:    ui->labelIcon->setPixmap(QPixmap(":/res/PC6601.ico"));      break;
    case 68:    ui->labelIcon->setPixmap(QPixmap(":/res/PC66SR_B.ico"));    break;
    default:;
    }
    ui->labelAppName->setText(QString::fromLocal8Bit(APPNAME));
    ui->labelVersion->setText(QString::fromLocal8Bit(VERSION));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
