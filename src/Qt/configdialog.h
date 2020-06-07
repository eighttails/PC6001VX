#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "p6vxapp.h"

class CFG6;
class QLineEdit;

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

private slots:
	void readConfig();
	void writeConfig();
	void dispFPS(int fps);
	void selectFile(QWidget* widget);
	void selectFolder(QWidget* widget);
	void warnFileOrFolderNotExist(QLineEdit* edit);
	void on_checkBoxCompatibleRomMode_clicked(bool checked);
};

#endif // CONFIGDIALOG_H
