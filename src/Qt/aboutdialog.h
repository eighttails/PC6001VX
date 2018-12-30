#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class CFG6;

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AboutDialog(int mdl, QWidget *parent = 0);
	~AboutDialog();

private:
	Ui::AboutDialog *ui;
	CFG6* config;
};

#endif // ABOUTDIALOG_H
