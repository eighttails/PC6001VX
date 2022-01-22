#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "p6vxapp.h"

class CFG6;
class QLineEdit;
class QComboBox;

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ConfigDialog(std::shared_ptr<CFG6> cfg, QWidget *parent = 0);
	~ConfigDialog();

private:
	Ui::ConfigDialog *ui;
	std::shared_ptr<CFG6> config;

private slots:
	void readConfig();
	void writeConfig();
	void selectFile(QWidget* widget);
	void selectFolder(QWidget* widget);
	void warnFileOrFolderNotExist(QLineEdit* edit);
	void setComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled);
	void on_checkBoxCompatibleRomMode_clicked(bool checked);
};

#endif // CONFIGDIALOG_H
