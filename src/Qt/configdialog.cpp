#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDir>

#include "../config.h"
#include "../osd.h"
#include "p6vxapp.h"

ConfigDialog::ConfigDialog(CFG6* cfg, QWidget *parent)
	: QDialog(parent)
	, config(cfg)
	, ui(new Ui::ConfigDialog)
{
	ui->setupUi(this);
	connect(ui->horizontalSliderFPS, SIGNAL(valueChanged(int)), this, SLOT(dispFPS(int)));

	// 各種マッピング
	// サウンド------------------------------------------------------
	// バッファサイズ
	connect(ui->horizontalSliderSndBufferSize, &QSlider::valueChanged,
			[this](int value){ui->labelSndBufferSize->setNum(value);});

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

	// ビデオキャプチャ設定を消す
	ui->groupBoxVideoCapture->setVisible(false);

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
	char str[PATH_MAX];		// 文字列取得用
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	//基本------------------------------------------------------
	//機種
	switch(config->GetModel()){
	case 60:    ui->radioButtonModel6001->setChecked(true);         break;
	case 61:    ui->radioButtonModel6001A->setChecked(true);         break;
	case 62:    ui->radioButtonModel6001mk2->setChecked(true);      break;
	case 64:    ui->radioButtonModel6001mk2SR->setChecked(true);    break;
	case 66:    ui->radioButtonModel6601->setChecked(true);         break;
	case 68:    ui->radioButtonModel6601SR->setChecked(true);       break;
	default:    Q_ASSERT(false);
	}

	// FDD
	switch(config->GetFddNum()){
	case 0: ui->radioButtonFDD0->setChecked(true);  break;
	case 1: ui->radioButtonFDD1->setChecked(true);  break;
	case 2: ui->radioButtonFDD2->setChecked(true);  break;
	default:    Q_ASSERT(false);
	}

	// 内蔵互換ROM使用
	const bool CompatibleRomMode = QString(config->GetRomPath()).startsWith(":");
	ui->checkBoxCompatibleRomMode->setChecked(CompatibleRomMode);
	if (CompatibleRomMode){
		ui->radioButtonModel6001A->setEnabled(false);
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
	ui->checkBoxExtRam->setChecked(config->GetUseExtRam());

	// 戦士のカートリッジ使用
	ui->checkBoxUseSoldier->setChecked(config->GetUseSoldier());

	// 画面------------------------------------------------------
	// MODE4カラー
	switch(config->GetMode4Color()){
	case 0: ui->radioButtonColorBW->setChecked(true);  break;
	case 1: ui->radioButtonColorRB->setChecked(true);  break;
	case 2: ui->radioButtonColorBR->setChecked(true);  break;
	case 3: ui->radioButtonColorPG->setChecked(true);  break;
	case 4: ui->radioButtonColorGP->setChecked(true);  break;
	default:    Q_ASSERT(false);
	}

	// ビデオキャプチャ
	switch(config->GetAviBpp()){
	case 16: ui->radioButtonVCap16bit->setChecked(true);  break;
	case 24: ui->radioButtonVCap24bit->setChecked(true);  break;
	case 32: ui->radioButtonVCap32bit->setChecked(true);  break;
	default:    Q_ASSERT(false);
	}
	// スキャンライン
	ui->checkBoxScanline->setChecked(config->GetScanLine());

	// スキャンライン輝度
	ui->lineEditScanLineBr->setText(QString::number(config->GetScanLineBr()));

	// 4:3表示
	ui->checkBoxDispNTSC->setChecked(config->GetDispNTSC());

	// フルスクリーン
#ifdef ALWAYSFULLSCREEN
	ui->checkBoxFullscreen->setVisible(false);
#else
	ui->checkBoxFullscreen->setChecked(config->GetFullScreen());
#endif

	// ステータスバー表示状態
	ui->checkBoxStatDisp->setChecked(config->GetDispStat());

	// ハードウェアアクセラレーション
#ifndef NOOPENGL
	ui->checkBoxHwAccel->setChecked(app->getSetting(P6VXApp::keyHwAccel).toBool());
#else
	ui->checkBoxHwAccel->setVisible(false);
#endif
	// フィルタリング
	ui->checkBoxFiltering->setChecked(app->getSetting(P6VXApp::keyFiltering).toBool());

	// フレームスキップ
	ui->horizontalSliderFPS->setValue(config->GetFrameSkip());
	dispFPS(config->GetFrameSkip());

	// サウンド------------------------------------------------------
	// サンプリングレート
	switch(config->GetSampleRate()){
	case 44100: ui->radioButtonSample441->setChecked(true); break;
	case 22050: ui->radioButtonSample220->setChecked(true); break;
	case 11025: ui->radioButtonSample110->setChecked(true); break;
	default:    Q_ASSERT(false);
	}

	// バッファサイズ
	ui->horizontalSliderSndBufferSize->setValue(config->GetSoundBuffer());
	ui->labelSndBufferSize->setText(QString::number(config->GetSoundBuffer()));

	// PSG LPFカットオフ周波数
	ui->lineEditPSGLPF->setText(QString::number(config->GetPsgLPF()));

	// マスター音量
	ui->horizontalSliderMasterVol->setValue(config->GetMasterVol());
	ui->labelMasterVol->setText(QString::number(config->GetMasterVol()));

	// PSG音量
	ui->horizontalSliderPSGVol->setValue(config->GetPsgVol());
	ui->labelPSGVol->setText(QString::number(config->GetPsgVol()));

	// 音声合成音量
	ui->horizontalSliderVoiceVol->setValue(config->GetVoiceVol());
	ui->labelVoiceVol->setText(QString::number(config->GetVoiceVol()));

	// TAPEモニタ音量
	ui->horizontalSliderTapeVol->setValue(config->GetCmtVol());
	ui->labelTapeVol->setText(QString::number(config->GetCmtVol()));

	// 入力関係----------------------------------------------------
	// キーリピート間隔
	ui->lineEditKeyRepeat->setText(QString::number(config->GetKeyRepeat()));

	// ファイル----------------------------------------------------
	// 拡張ROMファイル
	strncpy( str, config->GetExtRomFile(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditExtRom->setText(str);

	// TAPE(LOAD)ファイル名
	strncpy( str, config->GetTapeFile(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditLoadTape->setText(str);

	// TAPE(SAVE)ファイル名
	strncpy( str, config->GetSaveFile(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditSaveTape->setText(str);

	// DISK1ファイル名
	strncpy( str, config->GetDiskFile(1), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditDisk1->setText(str);

	// DISK2ファイル名
	strncpy( str, config->GetDiskFile(2), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditDisk2->setText(str);

	// プリンタファイル名
	strncpy( str, config->GetPrinterFile(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditPrinter->setText(str);

	// フォルダ--------------------------------------------------------------
	// ROMパス
	strncpy( str, config->GetRomPath(), PATH_MAX );
	OSD_DelDelimiter( str );
	if (CompatibleRomMode){
		ui->lineEditFolderRom->setText(tr("互換ROM使用中"));
	} else {
		ui->lineEditFolderRom->setText(str);
	}

	// TAPEパス
	strncpy( str, config->GetTapePath(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditFolderTape->setText(str);

	// DISKパス
	strncpy( str, config->GetDiskPath(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditFolderDisk->setText(str);

	// 拡張ROMパス
	strncpy( str, config->GetExtRomPath(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditFolderExtRom->setText(str);

	// IMGパス
	strncpy( str, config->GetImgPath(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditFolderImg->setText(str);

	// WAVEパス
	strncpy( str, config->GetWavePath(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditFolderWave->setText(str);

	// どこでもSAVEパス
	strncpy( str, config->GetDokoSavePath(), PATH_MAX );
	OSD_DelDelimiter( str );
	ui->lineEditFolderDokoSave->setText(str);

#ifdef ANDROID
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
	ui->lineEditClockRatio->setText(QString::number(qMin(qMax(1, config->GetOverClock()), 1000)));

	// CRCチェック
	ui->checkBoxRomCRC->setChecked(config->GetCheckCRC());

	// Turbo TAPE
	ui->checkBoxTurboTape->setChecked(config->GetTurboTAPE());

	// Boost Up
	ui->groupBoxBoostUp->setChecked(config->GetBoostUp());

	// BoostUp 最大倍率(N60モード)
	ui->lineEditBoost60->setText(QString::number(qMin(qMax(1, config->GetMaxBoost1()), 100)));

	// BoostUp 最大倍率(N60m/N66モード)
	ui->lineEditBoost66->setText(QString::number(qMin(qMax(1, config->GetMaxBoost2()), 100)));

	// 終了時 確認する
	ui->checkBoxFDDWaitEnable->setChecked(config->GetFddWaitEnable());

	// 終了時 確認する
	ui->checkBoxCkQuit->setChecked(config->GetCkQuit());

	// 終了時 INIファイルを保存する
	ui->checkBoxSaveQuit->setChecked(config->GetSaveQuit());
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
	if      (ui->radioButtonModel6001->isChecked())       config->SetModel(60);
	else if (ui->radioButtonModel6001A->isChecked())      config->SetModel(61);
	else if (ui->radioButtonModel6001mk2->isChecked())    config->SetModel(62);
	else if (ui->radioButtonModel6001mk2SR->isChecked())  config->SetModel(64);
	else if (ui->radioButtonModel6601->isChecked())       config->SetModel(66);
	else if (ui->radioButtonModel6601SR->isChecked())     config->SetModel(68);

	// FDD
	if      (ui->radioButtonFDD0->isChecked())    config->SetFddNum(0);
	else if (ui->radioButtonFDD1->isChecked())    config->SetFddNum(1);
	else if (ui->radioButtonFDD2->isChecked())    config->SetFddNum(2);

	// 拡張RAM使用
	config->SetUseExtRam(ui->checkBoxExtRam->isChecked());

	// 戦士のカートリッジ使用
	config->SetUseSoldier(ui->checkBoxUseSoldier->isChecked());

	// 画面---------------------------------------------------------------------
	// MODE4カラー
	if      (ui->radioButtonColorBW->isChecked())   config->SetMode4Color(0);   // モノクロ
	else if (ui->radioButtonColorBR->isChecked())   config->SetMode4Color(1);   // 赤/青
	else if (ui->radioButtonColorRB->isChecked())   config->SetMode4Color(2);   // 青/赤
	else if (ui->radioButtonColorPG->isChecked())   config->SetMode4Color(3);   // ピンク/緑
	else if (ui->radioButtonColorGP->isChecked())   config->SetMode4Color(4);   // 緑/ピンク

	// ビデオキャプチャ
	if      (ui->radioButtonVCap16bit->isChecked())   config->SetAviBpp(16);   // 16bit
	else if (ui->radioButtonVCap24bit->isChecked())   config->SetAviBpp(24);   // 24bit
	else if (ui->radioButtonVCap32bit->isChecked())   config->SetAviBpp(32);   // 32bit

	// スキャンライン
	config->SetScanLine(ui->checkBoxScanline->isChecked());

	// スキャンライン輝度
	iVal = ui->lineEditScanLineBr->text().toInt(&conv);
	if(conv){
		config->SetScanLineBr(iVal);
	}

	// 4:3表示
	config->SetDispNTSC(ui->checkBoxDispNTSC->isChecked());

#ifndef ALWAYSFULLSCREEN
	// フルスクリーン
	config->SetFullScreen(ui->checkBoxFullscreen->isChecked());
#endif
	// ステータスバー表示状態
	config->SetDispStat(ui->checkBoxStatDisp->isChecked());

	// ハードウェアアクセラレーション
	app->setSetting(P6VXApp::keyHwAccel, ui->checkBoxHwAccel->isChecked());

	// フィルタリング
	app->setSetting(P6VXApp::keyFiltering, ui->checkBoxFiltering->isChecked());

	// フレームスキップ
	config->SetFrameSkip(ui->horizontalSliderFPS->value());


	// サウンド-------------------------------------------------------------------
	// サンプリングレート
	if      (ui->radioButtonSample441->isChecked())   config->SetSampleRate(44100);
	else if (ui->radioButtonSample220->isChecked())   config->SetSampleRate(22050);
	else if (ui->radioButtonSample110->isChecked())   config->SetSampleRate(11025);

	// バッファサイズ
	config->SetSoundBuffer(ui->horizontalSliderSndBufferSize->value());

	// PSG LPFカットオフ周波数
	iVal = ui->lineEditPSGLPF->text().toInt(&conv);
	if(conv){
		config->SetPsgLPF(iVal);
	}

	// マスター音量
	config->SetMasterVol(ui->horizontalSliderMasterVol->value());

	// PSG音量
	config->SetPsgVol(ui->horizontalSliderPSGVol->value());

	// 音声合成音量
	config->SetVoiceVol(ui->horizontalSliderVoiceVol->value());

	// TAPEモニタ音量
	config->SetCmtVol(ui->horizontalSliderTapeVol->value());

	// 入力関係--------------------------------------------------------
	// キーリピート間隔
	iVal = ui->lineEditKeyRepeat->text().toInt(&conv);
	if(conv){
		config->SetKeyRepeat(iVal);
	}

	// フォルダ-------------------------------------------------------------
	// ROMパス
	qStr = ui->lineEditFolderRom->text();
	if (!ui->checkBoxCompatibleRomMode->isChecked()){
		if(qStr == "" || QDir(qStr).exists()){
			config->SetRomPath(qStr.toUtf8().constData());
		}
	}
	// TAPEパス
	qStr = ui->lineEditFolderTape->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetTapePath(qStr.toUtf8().constData());
	}

	// DISKパス
	qStr = ui->lineEditFolderDisk->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetDiskPath(qStr.toUtf8().constData());
	}

	// 拡張ROMパス
	qStr = ui->lineEditFolderExtRom->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetExtRomPath(qStr.toUtf8().constData());
	}

	// IMGパス
	qStr = ui->lineEditFolderImg->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetImgPath(qStr.toUtf8().constData());
	}

	// WAVEパス
	qStr = ui->lineEditFolderWave->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetWavePath(qStr.toUtf8().constData());
	}

	// どこでもSAVEパス
	qStr = ui->lineEditFolderDokoSave->text();
	if(qStr == "" || QDir(qStr).exists()){
		config->SetDokoSavePath(qStr.toUtf8().constData());
	}

	// ファイル--------------------------------------------------------
	// 拡張ROMファイル
	qStr = ui->lineEditExtRom->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetExtRomFile(qStr.toUtf8().constData());
	}

	// TAPE(LOAD)ファイル名
	qStr = ui->lineEditLoadTape->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetTapeFile(qStr.toUtf8().constData());
	}

	// TAPE(SAVE)ファイル名
	qStr = ui->lineEditSaveTape->text();
	config->SetSaveFile(qStr.toUtf8().constData());

	// DISK1ファイル名
	qStr = ui->lineEditDisk1->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetDiskFile(1, qStr.toUtf8().constData());
	}

	// DISK2ファイル名
	qStr = ui->lineEditDisk2->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetDiskFile(2, qStr.toUtf8().constData());
	}

	// プリンタファイル名
	qStr = ui->lineEditPrinter->text();
	config->SetPrinterFile(qStr.toUtf8().constData());

	// 色-----------------------------------------------------------------
	// 16〜72の色IDに対応させる。
	for (int id = 16; id <= 80; id++){
		QString buttonName = QString("pushButtonColor%1").arg(id);
		// ダイアログから動的に部品を取得する
		ColorButton* button = this->findChild<ColorButton*>(buttonName);
		config->SetColor(id, &button->getColor());
	}

	// その他--------------------------------------------------------------
	// オーバークロック率
	iVal = ui->lineEditClockRatio->text().toInt(&conv);
	if(conv){
		config->SetOverClock(min(max(1, iVal), 1000));
	}

	// CRCチェック
	config->SetCheckCRC(ui->checkBoxRomCRC->isChecked());

	// Turbo TAPE
	config->SetTurboTAPE(ui->checkBoxTurboTape->isChecked());

	// Boost Up
	config->SetBoostUp(ui->groupBoxBoostUp->isChecked());

	// BoostUp 最大倍率(N60モード)
	iVal = ui->lineEditBoost60->text().toInt(&conv);
	if(conv){
		config->SetMaxBoost1(min(max(1, iVal), 100));
	}

	// BoostUp 最大倍率(N60m/N66モード)
	iVal = ui->lineEditBoost66->text().toInt(&conv);
	if(conv){
		config->SetMaxBoost2(min(max(1, iVal), 100));
	}

	// FDDアクセスウェイト有効
	config->SetFddWaitEnable(ui->checkBoxFDDWaitEnable->isChecked());

	// 終了時 確認する
	config->SetCkQuit(ui->checkBoxCkQuit->isChecked());

	// 終了時 INIファイルを保存する
	config->SetSaveQuit(ui->checkBoxSaveQuit->isChecked());
}

void ConfigDialog::dispFPS(int fps)
{
	QStringList list;
	list
			<< "0 (60fps)"
			<< "1 (30fps)"
			<< "2 (20fps)"
			<< "3 (15fps)"
			<< "4 (12fps)"
			<< "5 (10fps)";
	ui->labelFps->setText(list[fps]);
}

void ConfigDialog::selectFile(QWidget *widget)
{
	char folder[PATH_MAX];
	char path[PATH_MAX];
	QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
	if(edit){
		FileDlg dlg = EndofFileDlg;

		if(edit == ui->lineEditExtRom){
			dlg = FD_ExtRom; strcpy(path, config->GetExtRomPath());
		} else if(edit == ui->lineEditLoadTape){
			dlg = FD_TapeLoad; strcpy(path, config->GetTapePath());
		} else if(edit == ui->lineEditSaveTape){
			dlg = FD_TapeSave; strcpy(path, config->GetTapePath());
		} else if(edit == ui->lineEditDisk1){
			dlg = FD_Disk; strcpy(path, config->GetDiskPath());
		} else if(edit == ui->lineEditDisk2){
			dlg = FD_Disk; strcpy(path, config->GetDiskPath());
		} else if(edit == ui->lineEditPrinter){
			dlg = FD_Printer; strcpy(path, OSD_GetModulePath());
		}

		strncpy(folder, edit->text().toUtf8().constData(), PATH_MAX);
		OSD_AddDelimiter(folder);
		OSD_FileSelect(NULL, dlg, folder, path);
		OSD_DelDelimiter(folder);
		if(strlen(folder) > 0){
			edit->setText(QString::fromUtf8(folder));
		}
	}

}

void ConfigDialog::selectFolder(QWidget *widget)
{
	char folder[PATH_MAX];
	QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
	if(edit){
		strncpy(folder, edit->text().toUtf8().constData(), PATH_MAX);
		OSD_AddDelimiter(folder);
		OSD_FolderDiaog(this, folder);
		OSD_DelDelimiter(folder);
		if(strlen(folder) > 0){
			edit->setText(QString::fromUtf8(folder));
		}
	}
}

void ConfigDialog::on_checkBoxCompatibleRomMode_clicked(bool checked)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->enableCompatibleRomMode(config, checked);
	readConfig();
}
