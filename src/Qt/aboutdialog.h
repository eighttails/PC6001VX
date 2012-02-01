#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class cConfig;

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(cConfig* cfg, QWidget *parent = 0);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
    cConfig* config;
};

#endif // ABOUTDIALOG_H
