#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QSignalMapper>

class CFG6;

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(CFG6* cfg, QWidget *parent = 0);
    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;
    CFG6* config;
    QSignalMapper* sliderLabelMap;
    QSignalMapper* fileRefMap;
    QSignalMapper* folderRefMap;

private slots:
    void readConfig();
    void writeConfig();
    void dispFPS(int fps);
    void dispValue(QWidget* widget);
    void selectFile(QWidget* widget);
    void selectFolder(QWidget* widget);
};

#endif // CONFIGDIALOG_H
