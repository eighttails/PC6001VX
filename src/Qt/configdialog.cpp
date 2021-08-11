#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDir>

#include "../config.h"
#include "../osd.h"
#include "p6vxapp.h"

ConfigDialog::ConfigDialog(CFG6* cfg, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ConfigDialog)
	, config(cfg)
{
	ui->setupUi(this);
	connect(ui->horizontalSliderFPS, SIGNAL(valueChanged(int)), this, SLOT(dispFPS(int)));

	// 各種マッピング
	// サウンド------------------------------------------------------
	// マスター音量
	connect(ui->horizontalSliderMasterVol, &QSlider::valueChanged,
			[this](int value){ui->labelMasterVol->setNum(value);});

	// PSG音量
	connect(ui->horizontalSliderPSGVol, &QSlider::valueChanged,
			[this](int value){ui->labelPSGVol->setNum(value);});

	// 音声合成音量
	connect(ui->horizontalSliderVoiceVol, &QSlider::valueChanged,
			[this](int value){ui->labelVoiceVol->setNum(value);});

	// TAPEモニタ音量
	connect(ui->horizontalSliderTapeVol, &QSlider::valueChanged,
			[this](int value){ui->labelTapeVol->setNum(value);});

	// ファイル----------------------------------------------------
	// 拡張ROMファイル
	connect(ui->pushButtonClearExtRom, &QPushButton::clicked, ui->lineEditExtRom, &QLineEdit::clear);
	connect(ui->pushButtonRefExtRom, &QPushButton::clicked, this, [&]{ConfigDialog::selectFile(ui->lineEditExtRom);});

	// TAPE(LOAD)ファイル名
	connect(ui->pushButtonClearLoadTape, &QPushButton::clicked, ui->lineEditLoadTape, &QLineEdit::clear);
	connect(ui->pushButtonRefLoadTape, &QPushButton::clicked, this, [&]{ConfigDialog::selectFile(ui->lineEditLoadTape);});

	// TAPE(SAVE)ファイル名
	connect(ui->pushButtonClearSaveTape, &QPushButton::clicked, ui->lineEditSaveTape, &QLineEdit::clear);
	connect(ui->pushButtonRefSaveTape, &QPushButton::clicked, this, [&]{ConfigDialog::selectFile(ui->lineEditSaveTape);});

	// DISK1ファイル名
	connect(ui->pushButtonClearDisk1, &QPushButton::clicked, ui->lineEditDisk1, &QLineEdit::clear);
	connect(ui->pushButtonRefDisk1, &QPushButton::clicked, this, [&]{ConfigDialog::selectFile(ui->lineEditDisk1);});

	// DISK2ファイル名
	connect(ui->pushButtonClearDisk2, &QPushButton::clicked, ui->lineEditDisk2, &QLineEdit::clear);
	connect(ui->pushButtonRefDisk2, &QPushButton::clicked, this, [&]{ConfigDialog::selectFile(ui->lineEditDisk2);});

	// プリンタファイル名
	connect(ui->pushButtonClearPrinter, &QPushButton::clicked, ui->lineEditPrinter, &QLineEdit::clear);
	connect(ui->pushButtonRefPrinter, &QPushButton::clicked, this, [&]{ConfigDialog::selectFile(ui->lineEditPrinter);});

	// フォルダ--------------------------------------------------------------
	// ROMパス
	connect(ui->pushButtonClearFolderRom, &QPushButton::clicked, ui->lineEditFolderRom, &QLineEdit::clear);
	connect(ui->pushButtonRefFolderRom, &QPushButton::clicked, this, [&]{ConfigDialog::selectFolder(ui->lineEditFolderRom);});

	// TAPEパス
	connect(ui->pushButtonClearFolderTape, &QPushButton::clicked, ui->lineEditFolderTape, &QLineEdit::clear);
	connect(ui->pushButtonRefFolderTape, &QPushButton::clicked, this, [&]{ConfigDialog::selectFolder(ui->lineEditFolderTape);});

	// DISKパス
	connect(ui->pushButtonClearFolderDisk, &QPushButton::clicked, ui->lineEditFolderDisk, &QLineEdit::clear);
	connect(ui->pushButtonRefFolderDisk, &QPushButton::clicked, this, [&]{ConfigDialog::selectFolder(ui->lineEditFolderDisk);});

	// 拡張ROMパス
	connect(ui->pushButtonClearFolderExtRom, &QPushButton::clicked, ui->lineEditFolderExtRom, &QLineEdit::clear);
	connect(ui->pushButtonRefFolderExtRom, &QPushButton::clicked, this, [&]{ConfigDialog::selectFolder(ui->lineEditFolderExtRom);});

	// IMGパス
	connect(ui->pushButtonClearFolderImg, &QPushButton::clicked, ui->lineEditFolderImg, &QLineEdit::clear);
	connect(ui->pushButtonRefFolderImg, &QPushButton::clicked, this, [&]{ConfigDialog::selectFolder(ui->lineEditFolderImg);});

	// WAVEパス
	connect(ui->pushButtonClearFolderWave, &QPushButton::clicked, ui->lineEditFolderWave, &QLineEdit::clear);
	connect(ui->pushButtonRefFolderWave, &QPushButton::clicked, this, [&]{ConfigDialog::selectFolder(ui->lineEditFolderWave);});

	// どこでもSAVEパス
	connect(ui->pushButtonClearFolderDokoSave, &QPushButton::clicked, ui->lineEditFolderDokoSave, &QLineEdit::clear);
	connect(ui->pushButtonRefFolderDokoSave, &QPushButton::clicked, this, [&]{ConfigDialog::selectFolder(ui->lineEditFolderDokoSave);});

	// OKボタンを押したら設定に書き込む
	connect(this, SIGNAL(accepted()), this, SLOT(writeConfig()));

	readConfig();
	adjustSize();
}

ConfigDialog::~ConfigDialog()
{
	delete ui;
}

void ConfigDialog::readConfig()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	// 基本------------------------------------------------------
	// 機種
	switch(config->GetValue(CV_Model)){
	case 60:    ui->radioButtonModel6001->setChecked(true);			break;
	case 61:    ui->radioButtonModel6001A->setChecked(true);		break;
	case 62:    ui->radioButtonModel6001mk2->setChecked(true);		break;
	case 64:    ui->radioButtonModel6001mk2SR->setChecked(true);	break;
	case 66:    ui->radioButtonModel6601->setChecked(true);			break;
	case 68:    ui->radioButtonModel6601SR->setChecked(true);		break;
	default:    Q_ASSERT(false);
	}

	// FDD
	switch(config->GetValue(CV_FDDrive)){
	case 0: ui->radioButtonFDD0->setChecked(true);	break;
	case 1: ui->radioButtonFDD1->setChecked(true);	break;
	case 2: ui->radioButtonFDD2->setChecked(true);	break;
	default:    Q_ASSERT(false);
	}

	// 内蔵互換ROM使用
	const bool CompatibleRomMode = P6VPATH2QSTR(config->GetValue(CF_RomPath)).startsWith(":");
	ui->checkBoxCompatibleRomMode->setChecked(CompatibleRomMode);
	if (CompatibleRomMode){
		ui->radioButtonModel6001A->setEnabled(true);
		ui->radioButtonModel6001mk2->setEnabled(true);
		ui->radioButtonModel6001mk2SR->setEnabled(false);
		ui->radioButtonModel6601->setEnabled(true);
		ui->radioButtonModel6601SR->setEnabled(false);
		ui->lineEditFolderRom->setEnabled(false);
		ui->pushButtonClearFolderRom->setEnabled(false);
		ui->pushButtonRefFolderRom->setEnabled(false);
	} else {
		ui->radioButtonModel6001A->setEnabled(true);
		ui->radioButtonModel6001mk2->setEnabled(true);
		ui->radioButtonModel6001mk2SR->setEnabled(true);
		ui->radioButtonModel6601->setEnabled(true);
		ui->radioButtonModel6601SR->setEnabled(true);
		ui->lineEditFolderRom->setEnabled(true);
		ui->pushButtonClearFolderRom->setEnabled(true);
		ui->pushButtonRefFolderRom->setEnabled(true);
	}

	// 拡張RAM使用
	//#TODO
	//	ui->checkBoxExtRam->setChecked(config->GetValue(CB_UseExtRam));

	// 戦士のカートリッジ使用
	//#TODO
	//	ui->checkBoxUseSoldier->setChecked(config->GetValue(CB_UseSoldier));

	// 画面------------------------------------------------------
	// MODE4カラー
	switch(config->GetValue(CV_Mode4Color)){
	case 0: ui->radioButtonColorBW->setChecked(true);	break;
	case 1: ui->radioButtonColorRB->setChecked(true);	break;
	case 2: ui->radioButtonColorBR->setChecked(true);	break;
	case 3: ui->radioButtonColorPG->setChecked(true);	break;
	case 4: ui->radioButtonColorGP->setChecked(true);	break;
	default:    Q_ASSERT(false);
	}

	// スキャンライン
	ui->checkBoxScanline->setChecked(config->GetValue(CB_ScanLine));

	// スキャンライン輝度
	ui->lineEditScanLineBr->setText(QString::number(config->GetValue(CV_ScanLineBr)));

	// 4:3表示
	ui->checkBoxDispNTSC->setChecked(config->GetValue(CB_DispNTSC));

	// フルスクリーン
#ifdef ALWAYSFULLSCREEN
	ui->checkBoxFullscreen->setVisible(false);
#else
	ui->checkBoxFullscreen->setChecked(config->GetValue(CB_FullScreen));
#endif

	// ステータスバー表示状態
	ui->checkBoxStatDisp->setChecked(config->GetValue(CB_DispStatus));

	// ハードウェアアクセラレーション
#ifndef NOOPENGL
	ui->checkBoxHwAccel->setChecked(app->getSetting(P6VXApp::keyHwAccel).toBool());
#else
	ui->checkBoxHwAccel->setVisible(false);
#endif
	// フィルタリング
	ui->checkBoxFiltering->setChecked(app->getSetting(P6VXApp::keyFiltering).toBool());

	// フレームスキップ
	ui->horizontalSliderFPS->setValue(config->GetValue(CV_FrameSkip));
	dispFPS(config->GetValue(CV_FrameSkip));

	// サウンド------------------------------------------------------
	// マスター音量
	ui->horizontalSliderMasterVol->setValue(config->GetValue(CV_MasterVol));
	ui->labelMasterVol->setText(QString::number(config->GetValue(CV_MasterVol)));

	// PSG音量
	ui->horizontalSliderPSGVol->setValue(config->GetValue(CV_PsgVolume));
	ui->labelPSGVol->setText(QString::number(config->GetValue(CV_PsgVolume)));

	// 音声合成音量
	ui->horizontalSliderVoiceVol->setValue(config->GetValue(CV_VoiceVolume));
	ui->labelVoiceVol->setText(QString::number(config->GetValue(CV_VoiceVolume)));

	// TAPEモニタ音量
	ui->horizontalSliderTapeVol->setValue(config->GetValue(CV_TapeVolume));
	ui->labelTapeVol->setText(QString::number(config->GetValue(CV_TapeVolume)));

	// ファイル----------------------------------------------------
	// 拡張ROMファイル
	P6VPATH path;
	path = config->GetValue(CF_ExtRom);
	OSD_DelDelimiter( path );
	ui->lineEditExtRom->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditExtRom);

	// TAPE(LOAD)ファイル名
	path = config->GetValue(CF_tape);
	OSD_DelDelimiter( path );
	ui->lineEditLoadTape->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditLoadTape);

	// TAPE(SAVE)ファイル名
	path = config->GetValue(CF_save);
	OSD_DelDelimiter( path );
	ui->lineEditSaveTape->setText(P6VPATH2QSTR(path));

	// DISK1ファイル名
	path = config->GetValue(CF_disk1);
	OSD_DelDelimiter( path );
	ui->lineEditDisk1->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditDisk1);

	// DISK2ファイル名
	path = config->GetValue(CF_disk2);
	OSD_DelDelimiter( path );
	ui->lineEditDisk2->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditDisk2);

	// プリンタファイル名
	path = config->GetValue(CF_printer);
	OSD_DelDelimiter( path );
	ui->lineEditPrinter->setText(P6VPATH2QSTR(path));

	// フォルダ--------------------------------------------------------------
	// ROMパス
	path = config->GetValue(CF_RomPath);
	OSD_DelDelimiter( path );
	if (CompatibleRomMode){
		ui->lineEditFolderRom->setText(tr("互換ROM使用中"));
	} else {
		ui->lineEditFolderRom->setText(P6VPATH2QSTR(path));
	}
	warnFileOrFolderNotExist(ui->lineEditFolderRom);

	// TAPEパス
	path = config->GetValue(CF_TapePath);
	OSD_DelDelimiter( path );
	ui->lineEditFolderTape->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditFolderTape);

	// DISKパス
	path = config->GetValue(CF_DiskPath);
	OSD_DelDelimiter( path );
	ui->lineEditFolderDisk->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditFolderDisk);

	// 拡張ROMパス
	path = config->GetValue(CF_ExtRomPath);
	OSD_DelDelimiter( path );
	ui->lineEditFolderExtRom->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditFolderExtRom);

	// IMGパス
	path = config->GetValue(CF_ImgPath);
	OSD_DelDelimiter( path );
	ui->lineEditFolderImg->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditFolderImg);

	// WAVEパス
	path = config->GetValue(CF_WavePath);
	OSD_DelDelimiter( path );
	ui->lineEditFolderWave->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditFolderWave);

	// どこでもSAVEパス
	path = config->GetValue(CF_DokoPath);
	OSD_DelDelimiter( path );
	ui->lineEditFolderDokoSave->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditFolderDokoSave);

#ifdef Q_OS_ANDROID
	// AndroidではTAPE(SAVE)とプリンタファイル、IMGパスとどこでもSAVEパスを編集不可にする。
	ui->labelSaveTape->setVisible(false);
	ui->lineEditSaveTape->setVisible(false);
	ui->pushButtonClearSaveTape->setVisible(false);
	ui->pushButtonRefSaveTape->setVisible(false);

	ui->labelPrinter->setVisible(false);
	ui->lineEditPrinter->setVisible(false);
	ui->pushButtonClearPrinter->setVisible(false);
	ui->pushButtonRefPrinter->setVisible(false);

	ui->labelFolderImg->setVisible(false);
	ui->lineEditFolderImg->setVisible(false);
	ui->pushButtonClearFolderImg->setVisible(false);
	ui->pushButtonRefFolderImg->setVisible(false);

	ui->labelFolderDokoSave->setVisible(false);
	ui->lineEditFolderDokoSave->setVisible(false);
	ui->pushButtonClearFolderDokoSave->setVisible(false);
	ui->pushButtonRefFolderDokoSave->setVisible(false);
#endif

	// 色--------------------------------------------------------------------------
	// 16〜72の色IDに対応させる。
	for (int id = 16; id <= 80; id++){
		QString buttonName = QString("pushButtonColor%1").arg(id);
		// ダイアログから動的に部品を取得する
		ColorButton* button = this->findChild<ColorButton*>(buttonName);
		button->initialize(id, config);
	}

	// その他
	// オーバークロック率
	ui->lineEditClockRatio->setText(QString::number(qMin(qMax(1, config->GetValue(CV_OverClock)), 1000)));

	// CRCチェック
	ui->checkBoxRomCRC->setChecked(config->GetValue(CB_CheckCRC));

	// Turbo TAPE
	ui->checkBoxTurboTape->setChecked(config->GetValue(CB_TurboTAPE));

	// Boost Up
	ui->groupBoxBoostUp->setChecked(config->GetValue(CB_BoostUp));

	// BoostUp 最大倍率(N60モード)
	ui->lineEditBoost60->setText(QString::number(qMin(qMax(1, config->GetValue(CV_MaxBoost60)), 100)));

	// BoostUp 最大倍率(N60m/N66モード)
	ui->lineEditBoost66->setText(QString::number(qMin(qMax(1, config->GetValue(CV_MaxBoost62)), 100)));

	// 終了時 確認する
	ui->checkBoxFDDWaitEnable->setChecked(config->GetValue(CB_FDDWait));

	// 終了時 確認する
	ui->checkBoxCkQuit->setChecked(config->GetValue(CB_CkQuit));

	// 終了時 INIファイルを保存する
	ui->checkBoxSaveQuit->setChecked(config->GetValue(CB_SaveQuit));
}

void ConfigDialog::writeConfig()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	// 一時変数
	int iVal = 0;
	QString qStr;
	bool conv = false;

	// 基本------------------------------------------------
	// 機種
	if      (ui->radioButtonModel6001->isChecked())			config->SetValue(CV_Model, 60);
	else if (ui->radioButtonModel6001A->isChecked())		config->SetValue(CV_Model, 61);
	else if (ui->radioButtonModel6001mk2->isChecked())		config->SetValue(CV_Model, 62);
	else if (ui->radioButtonModel6001mk2SR->isChecked())	config->SetValue(CV_Model, 64);
	else if (ui->radioButtonModel6601->isChecked())			config->SetValue(CV_Model, 66);
	else if (ui->radioButtonModel6601SR->isChecked())		config->SetValue(CV_Model, 68);

	// FDD
	if      (ui->radioButtonFDD0->isChecked())	config->SetValue(CV_FDDrive, 0);
	else if (ui->radioButtonFDD1->isChecked())	config->SetValue(CV_FDDrive, 1);
	else if (ui->radioButtonFDD2->isChecked())	config->SetValue(CV_FDDrive, 2);

	// 拡張RAM使用
	// #TODO
	//	config->SetValue(CB_UseExtRam, ui->checkBoxExtRam->isChecked());

	// 戦士のカートリッジ使用
	// #TODO
	//	config->SetValue(CB_UseSoldier, ui->checkBoxUseSoldier->isChecked());

	// 画面---------------------------------------------------------------------
	// MODE4カラー
	if      (ui->radioButtonColorBW->isChecked())	config->SetValue(CV_Mode4Color, 0);	// モノクロ
	else if (ui->radioButtonColorBR->isChecked())	config->SetValue(CV_Mode4Color, 1);	// 赤/青
	else if (ui->radioButtonColorRB->isChecked())	config->SetValue(CV_Mode4Color, 2);	// 青/赤
	else if (ui->radioButtonColorPG->isChecked())	config->SetValue(CV_Mode4Color, 3);	// ピンク/緑
	else if (ui->radioButtonColorGP->isChecked())	config->SetValue(CV_Mode4Color, 4);	// 緑/ピンク

	// スキャンライン
	config->SetValue(CB_ScanLine, ui->checkBoxScanline->isChecked());

	// スキャンライン輝度
	iVal = ui->lineEditScanLineBr->text().toInt(&conv);
	if(conv){
		config->SetValue(CV_ScanLineBr, iVal);
	}

	// 4:3表示
	config->SetValue(CB_DispNTSC, ui->checkBoxDispNTSC->isChecked());

#ifndef ALWAYSFULLSCREEN
	// フルスクリーン
	config->SetValue(CB_FullScreen, ui->checkBoxFullscreen->isChecked());
#endif
	// ステータスバー表示状態
	config->SetValue(CB_DispStatus, ui->checkBoxStatDisp->isChecked());

	// ハードウェアアクセラレーション
	app->setSetting(P6VXApp::keyHwAccel, ui->checkBoxHwAccel->isChecked());

	// フィルタリング
	app->setSetting(P6VXApp::keyFiltering, ui->checkBoxFiltering->isChecked());

	// フレームスキップ
	config->SetValue(CV_FrameSkip, ui->horizontalSliderFPS->value());


	// サウンド-------------------------------------------------------------------
	// マスター音量
	config->SetValue(CV_MasterVol, ui->horizontalSliderMasterVol->value());

	// PSG音量
	config->SetValue(CV_PsgVolume, ui->horizontalSliderPSGVol->value());

	// 音声合成音量
	config->SetValue(CV_VoiceVolume, ui->horizontalSliderVoiceVol->value());

	// TAPEモニタ音量
	config->SetValue(CV_TapeVolume, ui->horizontalSliderTapeVol->value());

	// フォルダ-------------------------------------------------------------
	// ROMパス
	qStr = ui->lineEditFolderRom->text();
	if (!ui->checkBoxCompatibleRomMode->isChecked()){
		if(qStr == "" || QDir(qStr).exists()){
			config->SetValue(CF_RomPath, QSTR2P6VPATH(qStr));
		}
	}
	// TAPEパス
	qStr = ui->lineEditFolderTape->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetValue(CF_TapePath, QSTR2P6VPATH(qStr));
	}

	// DISKパス
	qStr = ui->lineEditFolderDisk->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetValue(CF_DiskPath, QSTR2P6VPATH(qStr));
	}

	// 拡張ROMパス
	qStr = ui->lineEditFolderExtRom->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetValue(CF_ExtRomPath, QSTR2P6VPATH(qStr));
	}

	// IMGパス
	qStr = ui->lineEditFolderImg->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetValue(CF_ImgPath, QSTR2P6VPATH(qStr));
	}

	// WAVEパス
	qStr = ui->lineEditFolderWave->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetValue(CF_WavePath, QSTR2P6VPATH(qStr));
	}

	// どこでもSAVEパス
	qStr = ui->lineEditFolderDokoSave->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetValue(CF_DokoPath, QSTR2P6VPATH(qStr));
	}

	// ファイル--------------------------------------------------------
	// 拡張ROMファイル
	qStr = ui->lineEditExtRom->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetValue(CF_ExtRom, QSTR2P6VPATH(qStr));
	}

	// TAPE(LOAD)ファイル名
	qStr = ui->lineEditLoadTape->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetValue(CF_tape, QSTR2P6VPATH(qStr));
	}

	// TAPE(SAVE)ファイル名
	qStr = ui->lineEditSaveTape->text();
	config->SetValue(CF_save, QSTR2P6VPATH(qStr));

	// DISK1ファイル名
	qStr = ui->lineEditDisk1->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetValue(CF_disk1, QSTR2P6VPATH(qStr));
	}

	// DISK2ファイル名
	qStr = ui->lineEditDisk2->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetValue(CF_disk2, QSTR2P6VPATH(qStr));
	}

	// プリンタファイル名
	qStr = ui->lineEditPrinter->text();
	config->SetValue(CF_printer, QSTR2P6VPATH(qStr));

	// 色-----------------------------------------------------------------
	// 16〜72の色IDに対応させる。
	for (int id = 16; id <= 80; id++){
		QString buttonName = QString("pushButtonColor%1").arg(id);
		// ダイアログから動的に部品を取得する
		ColorButton* button = this->findChild<ColorButton*>(buttonName);
		config->SetColor(id, button->getColor());
	}

	// その他--------------------------------------------------------------
	// オーバークロック率
	iVal = ui->lineEditClockRatio->text().toInt(&conv);
	if(conv){
		config->SetValue(CV_OverClock, min(max(1, iVal), 1000));
	}

	// CRCチェック
	config->SetValue(CB_CheckCRC, ui->checkBoxRomCRC->isChecked());

	// Turbo TAPE
	config->SetValue(CB_TurboTAPE, ui->checkBoxTurboTape->isChecked());

	// Boost Up
	config->SetValue(CB_BoostUp, ui->groupBoxBoostUp->isChecked());

	// BoostUp 最大倍率(N60モード)
	iVal = ui->lineEditBoost60->text().toInt(&conv);
	if(conv){
		config->SetValue(CV_MaxBoost60, min(max(1, iVal), 100));
	}

	// BoostUp 最大倍率(N60m/N66モード)
	iVal = ui->lineEditBoost66->text().toInt(&conv);
	if(conv){
		config->SetValue(CV_MaxBoost62, min(max(1, iVal), 100));
	}

	// FDDアクセスウェイト有効
	config->SetValue(CB_FDDWait, ui->checkBoxFDDWaitEnable->isChecked());

	// 終了時 確認する
	config->SetValue(CB_CkQuit, ui->checkBoxCkQuit->isChecked());

	// 終了時 INIファイルを保存する
	config->SetValue(CB_SaveQuit, ui->checkBoxSaveQuit->isChecked());
}

void ConfigDialog::dispFPS(int fps)
{
	QStringList list;
	list	<< "0 (60fps)"
			<< "1 (30fps)"
			<< "2 (20fps)"
			<< "3 (15fps)"
			<< "4 (12fps)"
			<< "5 (10fps)";
	ui->labelFps->setText(list[fps]);
}

void ConfigDialog::selectFile(QWidget *widget)
{
	P6VPATH folder;
	P6VPATH path;
	QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
	if(edit){
		FileDlg dlg = FD_LoadAll;

		if(edit == ui->lineEditExtRom){
			dlg = FD_ExtRom; path = config->GetValue(CF_ExtRomPath);
		} else if(edit == ui->lineEditLoadTape){
			dlg = FD_TapeLoad; path = config->GetValue(CF_TapePath);
		} else if(edit == ui->lineEditSaveTape){
			dlg = FD_TapeSave; path = config->GetValue(CF_TapePath);
		} else if(edit == ui->lineEditDisk1){
			dlg = FD_Disk; path = config->GetValue(CF_DiskPath);
		} else if(edit == ui->lineEditDisk2){
			dlg = FD_Disk; path = config->GetValue(CF_DiskPath);
		} else if(edit == ui->lineEditPrinter){
			dlg = FD_Printer; path = OSD_GetConfigPath();
		}

		folder = QSTR2P6VPATH(edit->text());
		OSD_FileSelect(nullptr, dlg, folder, path);
		if(folder.size() > 0){
			edit->setText(P6VPATH2QSTR(folder));
			warnFileOrFolderNotExist(edit);
		}
	}

}

void ConfigDialog::selectFolder(QWidget *widget)
{
	P6VPATH folder;
	QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
	if(edit){
		folder = QSTR2P6VPATH(edit->text());
		OSD_AddDelimiter(folder);
		OSD_FolderDiaog(this, folder);
		OSD_DelDelimiter(folder);
		if(folder.size() > 0){
			edit->setText(P6VPATH2QSTR(folder));
			warnFileOrFolderNotExist(edit);
		}
	}
}

void ConfigDialog::warnFileOrFolderNotExist(QLineEdit *edit)
{
	// QLineEditが指しているファイルが存在しない場合文字を赤くする
	Q_ASSERT(edit);
	QFileInfo info(edit->text());
	QPalette *palette = new QPalette();
	if (!info.exists()){
		// ファイルが存在しないときだけ赤くする(存在する場合はテーマのデフォルトに戻る)
		palette->setColor(QPalette::Text, Qt::red);
	}
	edit->setPalette(*palette);
}

void ConfigDialog::on_checkBoxCompatibleRomMode_clicked(bool checked)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->enableCompatibleRomMode(config, checked);
	readConfig();
}
