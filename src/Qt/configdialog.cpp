#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDir>
#include <QStandardItemModel>
#include <QScreen>

#include "../config.h"
#include "../osd.h"
#include "../pc6001v.h"
#include "p6vxapp.h"

// 設定ファイル内のモデルIDとコンボボックスのインデックスの対応
static const QList<int> modelIds {
	60,
	61,
	62,
	66,
	64,
	68,
};


// 設定ファイル内の拡張カートリッジIDとコンボボックスのインデックスの対応
static const QList<WORD> extCartIds {
	0,
	EXC6005,
	EXC6006,
	EXC6001,
	EXC660101,
	EXC6006SR,
	EXC6007SR,
	// EXC6053,
	// EXC60M55,
	EXCSOL1,
	EXCSOL2,
	EXCSOL3,
};

ConfigDialog::ConfigDialog(std::shared_ptr<CFG6> cfg, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ConfigDialog)
	, config(cfg)
{
	ui->setupUi(this);

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
	
#ifndef ALWAYSFULLSCREEN
	// ダイアログを設定項目が収まる最適なサイズに設定
	ui->scrollAreaBasicTab->setMinimumSize(ui->scrollAreaBasicTabWidgetContents->sizeHint());
	ui->scrollAreaScreenTab->setMinimumSize(ui->scrollAreaScreenTabWidgetContents->sizeHint());
	ui->scrollAreaSoundTab->setMinimumSize(ui->scrollAreaSoundTabWidgetContents->sizeHint());
	ui->scrollAreaFileTab->setMinimumSize(ui->scrollAreaFileTabWidgetContents->sizeHint());
	ui->scrollAreaColorTab->setMinimumSize(ui->scrollAreaColorTabWidgetContents->sizeHint());
	ui->scrollAreaMiscTab->setMinimumSize(ui->scrollAreaMiscTabWidgetContents->sizeHint());
	adjustSize();
	ui->scrollAreaBasicTab->setMinimumSize(0, 0);
	ui->scrollAreaScreenTab->setMinimumSize(0, 0);
	ui->scrollAreaSoundTab->setMinimumSize(0, 0);
	ui->scrollAreaFileTab->setMinimumSize(0, 0);
	ui->scrollAreaColorTab->setMinimumSize(0, 0);
	ui->scrollAreaMiscTab->setMinimumSize(0, 0);
	setMaximumSize(qApp->primaryScreen()->size());
#endif
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
	auto modelId = (config->GetValue(CV_Model));
	ui->comboBoxModel->setCurrentIndex(modelIds.indexOf(modelId));

	// 拡張カートリッジ
	auto extCartId = config->GetValue(CV_ExCartridge);
	ui->comboBoxExtCartridge->setCurrentIndex(extCartIds.indexOf(extCartId));

	//// FDD
	// FDD数
	ui->spinBoxNumFdd->setValue(config->GetValue(CV_FDDrive));
	// FDDウェイト
	ui->checkBoxFDDWait->setChecked(config->GetValue(CB_FDDWait));

	//// KEY
	// ローマ字入力有効
	ui->checkBoxRomajiInput->setChecked(config->GetValue(CB_Romaji));
	// ローマ字入力ウェイト
	ui->spinBoxRomajiWait->setValue(config->GetValue(CV_RomajiWait));

	//// CMT
	// Turbo TAPE
	ui->checkBoxTurboTape->setChecked(config->GetValue(CB_TurboTAPE));

	// Boost Up
	ui->groupBoxBoostUp->setChecked(config->GetValue(CB_BoostUp));

	// BoostUp 最大倍率(N60モード)
	ui->spinBoxBoost60->setValue(qMin(qMax(1, config->GetValue(CV_MaxBoost60)), 100));

	// BoostUp 最大倍率(N60m/N66モード)
	ui->spinBoxBoost66->setValue(qMin(qMax(1, config->GetValue(CV_MaxBoost62)), 100));

	// ストップビット数
	ui->spinBoxStopBit->setValue(qMin(qMax(2, config->GetValue(CV_StopBit)), 10));

	// 内蔵互換ROM使用
	const bool CompatibleRomMode = P6VPATH2QSTR(config->GetValue(CF_RomPath)).startsWith(":");
	ui->checkBoxCompatibleRomMode->setChecked(CompatibleRomMode);
	if (CompatibleRomMode){
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(60), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(61), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(62), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(66), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(64), false);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(68), false);
		ui->lineEditFolderRom->setEnabled(false);
		ui->pushButtonClearFolderRom->setEnabled(false);
		ui->pushButtonRefFolderRom->setEnabled(false);
	} else {
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(60), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(61), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(62), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(66), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(64), true);
		setComboBoxItemEnabled(ui->comboBoxModel, modelIds.indexOf(68), true);
		ui->lineEditFolderRom->setEnabled(true);
		ui->pushButtonClearFolderRom->setEnabled(true);
		ui->pushButtonRefFolderRom->setEnabled(true);
	}


	// 画面------------------------------------------------------
	// MODE4カラー
	ui->comboBoxMode4Color->setCurrentIndex(config->GetValue(CV_Mode4Color));

	// フレームスキップ
	ui->comboBoxFrameSkip->setCurrentIndex(config->GetValue(CV_FrameSkip));

	// スキャンライン
	ui->checkBoxScanline->setChecked(config->GetValue(CB_ScanLine));

	// スキャンライン輝度
	ui->spinBoxScanLineBr->setValue(config->GetValue(CV_ScanLineBr));

	// 4:3表示
	ui->checkBoxDispNTSC->setChecked(config->GetValue(CB_DispNTSC));
	
	// フィルタリング
	ui->checkBoxFiltering->setChecked(config->GetValue(CB_Filtering));

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

	// 横画面時の仮想キーボード位置
	ui->comboBoxVirtualKeyPosition->setCurrentIndex(app->getSetting(P6VXApp::keyVirtualKeyPosition).toInt());

	
	//// ビデオキャプチャ時の設定
	// フレームスキップ
	ui->comboBoxMovieFrameSkip->setCurrentIndex(config->GetValue(CV_AviFrameSkip));

	// 解像度倍率
	ui->spinBoxMovieZoom->setValue(config->GetValue(CV_AviZoom));

	// スキャンライン
	ui->checkBoxMovieScanline->setChecked(config->GetValue(CB_AviScanLine));

	// スキャンライン輝度
	ui->spinBoxMovieScanLineBr->setValue(config->GetValue(CV_AviScanLineBr));

	// 4:3表示
	ui->checkBoxMovieDispNTSC->setChecked(config->GetValue(CB_AviDispNTSC));

	// フィルタリング
	ui->checkBoxMovieFiltering->setChecked(config->GetValue(CB_AviFiltering));

#ifdef NOAVI
	ui->groupBoxVideoCapture->setVisible(false);
#endif
	
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
	path = config->GetValue(CF_Tape);
	OSD_DelDelimiter( path );
	ui->lineEditLoadTape->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditLoadTape);

	// TAPE(SAVE)ファイル名
	path = config->GetValue(CF_Save);
	OSD_DelDelimiter( path );
	ui->lineEditSaveTape->setText(P6VPATH2QSTR(path));

	// DISK1ファイル名
	path = config->GetValue(CF_Disk1);
	OSD_DelDelimiter( path );
	ui->lineEditDisk1->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditDisk1);

	// DISK2ファイル名
	path = config->GetValue(CF_Disk2);
	OSD_DelDelimiter( path );
	ui->lineEditDisk2->setText(P6VPATH2QSTR(path));
	warnFileOrFolderNotExist(ui->lineEditDisk2);

	// プリンタファイル名
	path = config->GetValue(CF_Printer);
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
	// AndroidではTAPE(SAVE)とプリンタファイル、
	// TAPE,DISK,EXTROM,IMG,SAVEパスを編集不可にする。
	ui->labelSaveTape->setVisible(false);
	ui->lineEditSaveTape->setVisible(false);
	ui->pushButtonClearSaveTape->setVisible(false);
	ui->pushButtonRefSaveTape->setVisible(false);

	ui->labelPrinter->setVisible(false);
	ui->lineEditPrinter->setVisible(false);
	ui->pushButtonClearPrinter->setVisible(false);
	ui->pushButtonRefPrinter->setVisible(false);

	ui->labelFolderTape->setVisible(false);
	ui->lineEditFolderTape->setVisible(false);
	ui->pushButtonClearFolderTape->setVisible(false);
	ui->pushButtonRefFolderTape->setVisible(false);

	ui->labelFolderDisk->setVisible(false);
	ui->lineEditFolderDisk->setVisible(false);
	ui->pushButtonClearFolderDisk->setVisible(false);
	ui->pushButtonRefFolderDisk->setVisible(false);

	ui->labelFolderExtRom->setVisible(false);
	ui->lineEditFolderExtRom->setVisible(false);
	ui->pushButtonClearFolderExtRom->setVisible(false);
	ui->pushButtonRefFolderExtRom->setVisible(false);

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
	for (int id = 0; id <= 64; id++){
		QString buttonName = QString("pushButtonColor%1").arg(id);
		// ダイアログから動的に部品を取得する
		ColorButton* button = this->findChild<ColorButton*>(buttonName);
		if (button) {
			button->initialize(id, config.get());
		}
	}

	// その他
	// オーバークロック率
	ui->spinBoxClockRatio->setValue(config->GetValue(CV_OverClock));

	// タイマー精度
	ui->spinBoxTimerResolution->setValue(app->getSetting(P6VXApp::keyTimerResolution).toInt());

	// CRCチェック
	ui->checkBoxRomCRC->setChecked(config->GetValue(CB_CheckCRC));

	// どこでもLOAD(SLOT)実行時に確認する
	ui->checkBoxCkDokoLoad->setChecked(config->GetValue(CB_CkDokoLoad));

	// 終了時 確認する
	ui->checkBoxCkQuit->setChecked(config->GetValue(CB_CkQuit));

	// 終了時 INIファイルを保存する
	ui->checkBoxSaveQuit->setChecked(config->GetValue(CB_SaveQuit));
}

void ConfigDialog::writeConfig()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	// 一時変数
	QString qStr;

	// 基本------------------------------------------------
	// 機種
	config->SetValue(CV_Model, modelIds[ui->comboBoxModel->currentIndex()]);

	//// FDD
	// FDDドライブ数
	config->SetValue(CV_FDDrive, ui->spinBoxNumFdd->value());
	// FDDアクセスウェイト有効
	config->SetValue(CB_FDDWait, ui->checkBoxFDDWait->isChecked());

	//// KEY
	// ローマ字入力有効
	config->SetValue(CB_Romaji, ui->checkBoxRomajiInput->isChecked());
	// ローマ字入力ウェイト
	config->SetValue(CV_RomajiWait, ui->spinBoxRomajiWait->value());

	//// CMT
	// Turbo TAPE
	config->SetValue(CB_TurboTAPE, ui->checkBoxTurboTape->isChecked());

	// Boost Up
	config->SetValue(CB_BoostUp, ui->groupBoxBoostUp->isChecked());

	// BoostUp 最大倍率(N60モード)
	config->SetValue(CV_MaxBoost60, ui->spinBoxBoost60->value());

	// BoostUp 最大倍率(N60m/N66モード)
	config->SetValue(CV_MaxBoost62, ui->spinBoxBoost66->value());

	// ストップビット数
	config->SetValue(CV_StopBit, ui->spinBoxStopBit->value());

	// 拡張カートリッジ
	config->SetValue(CV_ExCartridge, int(extCartIds[ui->comboBoxExtCartridge->currentIndex()]));


	// 画面---------------------------------------------------------------------
	// MODE4カラー
	config->SetValue(CV_Mode4Color, ui->comboBoxMode4Color->currentIndex());

	// フレームスキップ
	config->SetValue(CV_FrameSkip, ui->comboBoxFrameSkip->currentIndex());

	// スキャンライン
	config->SetValue(CB_ScanLine, ui->checkBoxScanline->isChecked());

	// スキャンライン輝度
	config->SetValue(CV_ScanLineBr, ui->spinBoxScanLineBr->value());

	// 4:3表示
	config->SetValue(CB_DispNTSC, ui->checkBoxDispNTSC->isChecked());
	
	// フィルタリング
	config->SetValue(CB_Filtering, ui->checkBoxFiltering->isChecked());

#ifndef ALWAYSFULLSCREEN
	// フルスクリーン
	config->SetValue(CB_FullScreen, ui->checkBoxFullscreen->isChecked());
#endif
	// ステータスバー表示状態
	config->SetValue(CB_DispStatus, ui->checkBoxStatDisp->isChecked());

	// ハードウェアアクセラレーション
	app->setSetting(P6VXApp::keyHwAccel, ui->checkBoxHwAccel->isChecked());

	
	// 横画面時の仮想キーボード位置
	app->setSetting(P6VXApp::keyVirtualKeyPosition, ui->comboBoxVirtualKeyPosition->currentIndex());
	

	//// ビデオキャプチャ時の設定
	// フレームスキップ
	config->SetValue(CV_AviFrameSkip, ui->comboBoxMovieFrameSkip->currentIndex());

	// 解像度倍率
	config->SetValue(CV_AviZoom, ui->spinBoxMovieZoom->value());

	// スキャンライン
	config->SetValue(CB_AviScanLine, ui->checkBoxMovieScanline->isChecked());

	// スキャンライン輝度
	config->SetValue(CV_AviScanLineBr, ui->spinBoxMovieScanLineBr->value());

	// 4:3表示
	config->SetValue(CB_AviDispNTSC, ui->checkBoxMovieDispNTSC->isChecked());

	// フィルタリング
	config->SetValue(CB_AviFiltering, ui->checkBoxMovieFiltering->isChecked());


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
		config->SetValue(CF_Tape, QSTR2P6VPATH(qStr));
	}

	// TAPE(SAVE)ファイル名
	qStr = ui->lineEditSaveTape->text();
	config->SetValue(CF_Save, QSTR2P6VPATH(qStr));

	// DISK1ファイル名
	qStr = ui->lineEditDisk1->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetValue(CF_Disk1, QSTR2P6VPATH(qStr));
	}

	// DISK2ファイル名
	qStr = ui->lineEditDisk2->text();
	if(qStr == "" || QFile(qStr).exists()){
		config->SetValue(CF_Disk2, QSTR2P6VPATH(qStr));
	}

	// プリンタファイル名
	qStr = ui->lineEditPrinter->text();
	config->SetValue(CF_Printer, QSTR2P6VPATH(qStr));

	// 色-----------------------------------------------------------------
	// 16〜72の色IDに対応させる。
	for (int id = 0; id <= 64; id++){
		QString buttonName = QString("pushButtonColor%1").arg(id);
		// ダイアログから動的に部品を取得する
		ColorButton* button = this->findChild<ColorButton*>(buttonName);
		if (button) {
			config->SetColor(id, button->getColor());
		}
	}

	// その他--------------------------------------------------------------
	// オーバークロック率
	config->SetValue(CV_OverClock, ui->spinBoxClockRatio->value());

	// タイマー精度
	app->setSetting(P6VXApp::keyTimerResolution, ui->spinBoxTimerResolution->value());

	// CRCチェック
	config->SetValue(CB_CheckCRC, ui->checkBoxRomCRC->isChecked());

	// どこでもLOAD(SLOT)実行時に確認する
	config->SetValue(CB_CkDokoLoad, ui->checkBoxCkDokoLoad->isChecked());

	// 終了時 確認する
	config->SetValue(CB_CkQuit, ui->checkBoxCkQuit->isChecked());

	// 終了時 INIファイルを保存する
	config->SetValue(CB_SaveQuit, ui->checkBoxSaveQuit->isChecked());
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
		if(folder != ""){
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
		if(folder != ""){
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

void ConfigDialog::setComboBoxItemEnabled(QComboBox *comboBox, int index, bool enabled)
{
	auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
	assert(model);
	if(!model) return;

	auto * item = model->item(index);
	assert(item);
	if(!item) return;
	item->setEnabled(enabled);
}

void ConfigDialog::on_checkBoxCompatibleRomMode_clicked(bool checked)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->enableCompatibleRomMode(config, checked);
	readConfig();
}
