#ifndef SYSTEMINFODIALOG_H
#define SYSTEMINFODIALOG_H

#include <QDialog>

namespace Ui {
class SystemInfoDialog;
}

class SystemInfoDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SystemInfoDialog(QWidget *parent = nullptr);
	~SystemInfoDialog();

protected slots:
	void copySystemInfo();

private:
	void obtainSystemInfo();

	Ui::SystemInfoDialog *ui;
};

#endif // SYSTEMINFODIALOG_H
