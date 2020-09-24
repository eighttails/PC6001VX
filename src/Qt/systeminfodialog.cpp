#include "systeminfodialog.h"
#include "ui_systeminfodialog.h"
#include <QDialogButtonBox>
#include <QToolButton>
#include <QAction>
#include <QClipboard>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>
#include <QMetaEnum>
#include <QProcessEnvironment>


SystemInfoDialog::SystemInfoDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SystemInfoDialog)
{
	ui->setupUi(this);
	auto copyAction = new QAction("Copy");
	auto copyButton = new QToolButton();
	copyButton->setDefaultAction(copyAction);
	connect(copyAction, &QAction::triggered, this, &SystemInfoDialog::copySystemInfo);
	ui->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);

	obtainSystemInfo();
}

SystemInfoDialog::~SystemInfoDialog()
{
	delete ui;
}

void SystemInfoDialog::copySystemInfo()
{
	QGuiApplication::clipboard()->setText(ui->plainTextEdit->toPlainText());
}

void SystemInfoDialog::obtainSystemInfo()
{
	ui->plainTextEdit->clear();
	QString str;
	QTextStream s(&str);

	QSysInfo si;

	s << "[System]\n";
	s << "productName=" << si.prettyProductName() << "\n";
	s << "productType=" << si.productType() << "\n";
	s << "productVersion=" <<  si.productVersion() << "\n";
	s << "currentCpuArchitecture=" <<  si.currentCpuArchitecture() << "\n";
	s << "\n";

	s << "[Environment]\n";
	foreach(auto e, QProcessEnvironment::systemEnvironment().toStringList()){
	s << e << "\n";
	}
	s << "\n";

	QMetaEnum metaEnum = QMetaEnum::fromType<QStandardPaths::StandardLocation>();
	QStandardPaths::StandardLocation locations[] = {
		QStandardPaths::HomeLocation,
		QStandardPaths::DocumentsLocation,
		QStandardPaths::AppDataLocation,
		QStandardPaths::GenericDataLocation,
	};
	for(auto location: locations){
		s << "[" << metaEnum.valueToKey(location) << "]\n";
		{
			const auto paths = QStandardPaths::standardLocations(location);
			foreach (auto path, paths){
				s << path << "\n";
			}
		}
		s << "\n";
	}

	s << "[Fonts]\n";
	QFontDatabase database;
	// 日本語に対応したフォントを検索
	auto list = database.families(QFontDatabase::Japanese);
	for (auto& f : list){
		s << f << "\n";
	}
	s << "\n";

	ui->plainTextEdit->setPlainText(str);


}
