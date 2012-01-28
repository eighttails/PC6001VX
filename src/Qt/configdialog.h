#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QSignalMapper>

class cConfig;

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(cConfig* cfg, QWidget *parent = 0);
    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;
    cConfig* config;
    QSignalMapper* sliderLabelMap;
    QSignalMapper* folderRefMap;

private slots:
    void assignColorButton();
    void readConfig();
    void writeConfig();
    void dispFPS(int fps);
    void dispValue(QWidget* widget);
    void selectFile(QWidget* widget);
    void selectFolder(QWidget* widget);
};

#endif // CONFIGDIALOG_H
