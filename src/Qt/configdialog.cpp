#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDir>

#include "../config.h"
#include "../osd.h"

ConfigDialog::ConfigDialog(cConfig* cfg, QWidget *parent) :
    QDialog(parent),
    config(cfg),
    sliderLabelMap(new QSignalMapper(this)),
    fileRefMap(new QSignalMapper(this)),
    folderRefMap(new QSignalMapper(this)),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    connect(ui->horizontalSliderFPS, SIGNAL(valueChanged(int)), this, SLOT(dispFPS(int)));
    // スライダーを動かしたらラベルに値を反映
    connect(sliderLabelMap, SIGNAL(mapped(QWidget*)), this, SLOT(dispValue(QWidget*)));
    // 参照ボタンを押したらファイル、ファイル選択ダイアログを出し、ラインエディットに反映
    connect(fileRefMap, SIGNAL(mapped(QWidget*)), this, SLOT(selectFile(QWidget*)));
    // 参照ボタンを押したらファイル、フォルダ選択ダイアログを出し、ラインエディットに反映
    connect(folderRefMap, SIGNAL(mapped(QWidget*)), this, SLOT(selectFolder(QWidget*)));

    // OKボタンを押したら設定に書き込む
    connect(this, SIGNAL(accepted()), this, SLOT(writeConfig()));

    readConfig();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::readConfig()
{
    char str[PATH_MAX];		// 文字列取得用

    //基本------------------------------------------------------
    //機種
    switch(config->GetModel()){
    case 60:    ui->radioButtonModel6001->setChecked(true);         break;
    case 62:    ui->radioButtonModel6001mk2->setChecked(true);      break;
    case 66:    ui->radioButtonModel6001mk2SR->setChecked(true);    break;
    case 64:    ui->radioButtonModel6601->setChecked(true);         break;
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

    // 拡張RAM使用
    ui->checkBoxExtRam->setChecked(config->GetUseExtRam());

    // 戦士のカートリッジ使用
    ui->checkBoxUseSoldier->setChecked(config->GetUseSoldier());

    // 画面------------------------------------------------------
    // カラーモード
    switch(config->GetScrBpp()){
    case  8: ui->radioButtonColor8Bit->setChecked(true);   break;
    case 16: ui->radioButtonColor16Bit->setChecked(true);  break;
    case 24: ui->radioButtonColor24Bit->setChecked(true);  break;
    default:    Q_ASSERT(false);
    }

    // MODE4カラー
    switch(config->GetMode4Color()){
    case 0: ui->radioButtonColorBW->setChecked(true);  break;
    case 1: ui->radioButtonColorRB->setChecked(true);  break;
    case 2: ui->radioButtonColorBR->setChecked(true);  break;
    case 3: ui->radioButtonColorPG->setChecked(true);  break;
    case 4: ui->radioButtonColorGP->setChecked(true);  break;
    default:    Q_ASSERT(false);
    }

    // スキャンライン
    ui->checkBoxScanline->setChecked(config->GetScanLine());

    // スキャンライン輝度
    ui->lineEditScanLineBr->setText(QString::number(config->GetScanLineBr()));

    // 4:3表示
    ui->checkBoxDispNTSC->setChecked(config->GetDispNTSC());

    // フルスクリーン
    ui->checkBoxFullscreen->setChecked(config->GetFullScreen());

    // ステータスバー表示状態
    ui->checkBoxStatDisp->setChecked(config->GetStatDisp());

    // フレームスキップ
    ui->horizontalSliderFPS->setValue(config->GetFrameSkip());
    dispFPS(config->GetFrameSkip());

    // サウンド------------------------------------------------------
    // サンプリングレート
    switch(config->GetSampleRate()){
    case 44100: ui->radioButtonSample441->setChecked(true); break;
    case 22500: ui->radioButtonSample225->setChecked(true); break;
    case 11025: ui->radioButtonSample110->setChecked(true); break;
    default:    Q_ASSERT(false);
    }

    // バッファサイズ
    ui->horizontalSliderSndBufferSize->setValue(config->GetSoundBuffer());
    ui->labelSndBufferSize->setText(QString::number(config->GetSoundBuffer()));
    connect(ui->horizontalSliderSndBufferSize, SIGNAL(valueChanged(int)), sliderLabelMap, SLOT(map()));
    sliderLabelMap->setMapping(ui->horizontalSliderSndBufferSize, ui->labelSndBufferSize);

    // PSG LPFカットオフ周波数
    ui->lineEditPSGLPF->setText(QString::number(config->GetPsgLPF()));

    // マスター音量
    ui->horizontalSliderMasterVol->setValue(config->GetMasterVol());
    ui->labelMasterVol->setText(QString::number(config->GetMasterVol()));
    connect(ui->horizontalSliderMasterVol, SIGNAL(valueChanged(int)), sliderLabelMap, SLOT(map()));
    sliderLabelMap->setMapping(ui->horizontalSliderMasterVol, ui->labelMasterVol);

    // PSG音量
    ui->horizontalSliderPSGVol->setValue(config->GetPsgVol());
    ui->labelPSGVol->setText(QString::number(config->GetPsgVol()));
    connect(ui->horizontalSliderPSGVol, SIGNAL(valueChanged(int)), sliderLabelMap, SLOT(map()));
    sliderLabelMap->setMapping(ui->horizontalSliderPSGVol, ui->labelPSGVol);

    // 音声合成音量
    ui->horizontalSliderVoiceVol->setValue(config->GetVoiceVol());
    ui->labelVoiceVol->setText(QString::number(config->GetVoiceVol()));
    connect(ui->horizontalSliderVoiceVol, SIGNAL(valueChanged(int)), sliderLabelMap, SLOT(map()));
    sliderLabelMap->setMapping(ui->horizontalSliderVoiceVol, ui->labelVoiceVol);

    // TAPEモニタ音量
    ui->horizontalSliderTapeVol->setValue(config->GetCmtVol());
    ui->labelTapeVol->setText(QString::number(config->GetCmtVol()));
    connect(ui->horizontalSliderTapeVol, SIGNAL(valueChanged(int)), sliderLabelMap, SLOT(map()));
    sliderLabelMap->setMapping(ui->horizontalSliderTapeVol, ui->labelTapeVol);

    // 入力関係----------------------------------------------------
    // キーリピート間隔
    ui->lineEditKeyRepeat->setText(QString::number(config->GetKeyRepeat()));

    // ファイル----------------------------------------------------
    // 拡張ROMファイル
    strncpy( str, config->GetExtRomFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditExtRom->setText(str);
    connect(ui->pushButtonRefExtRom, SIGNAL(clicked()), fileRefMap, SLOT(map()));
    fileRefMap->setMapping(ui->pushButtonRefExtRom, ui->lineEditExtRom);

    // TAPE(LOAD)ファイル名
    strncpy( str, config->GetTapeFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditLoadTape->setText(str);
    connect(ui->pushButtonRefLoadTape, SIGNAL(clicked()), fileRefMap, SLOT(map()));
    fileRefMap->setMapping(ui->pushButtonRefLoadTape, ui->lineEditLoadTape);

    // TAPE(SAVE)ファイル名
    strncpy( str, config->GetSaveFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditSaveTape->setText(str);
    connect(ui->pushButtonRefSaveTape, SIGNAL(clicked()), fileRefMap, SLOT(map()));
    fileRefMap->setMapping(ui->pushButtonRefSaveTape, ui->lineEditSaveTape);

    // DISKファイル名
    strncpy( str, config->GetDiskFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditDisk->setText(str);
    connect(ui->pushButtonRefDisk, SIGNAL(clicked()), fileRefMap, SLOT(map()));
    fileRefMap->setMapping(ui->pushButtonRefDisk, ui->lineEditDisk);

    // プリンタファイル名
    strncpy( str, config->GetPrinterFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditPrinter->setText(str);
    connect(ui->pushButtonRefPrinter, SIGNAL(clicked()), fileRefMap, SLOT(map()));
    fileRefMap->setMapping(ui->pushButtonRefPrinter, ui->lineEditPrinter);

    // 全角フォントファイル名
    strncpy( str, config->GetFontFileZ(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditZenFont->setText(str);
    connect(ui->pushButtonRefZenFont, SIGNAL(clicked()), fileRefMap, SLOT(map()));
    fileRefMap->setMapping(ui->pushButtonRefZenFont, ui->lineEditZenFont);

    // 半角フォントファイル名
    strncpy( str, config->GetFontFileH(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditHanFont->setText(str);
    connect(ui->pushButtonRefHanFont, SIGNAL(clicked()), fileRefMap, SLOT(map()));
    fileRefMap->setMapping(ui->pushButtonRefHanFont, ui->lineEditHanFont);

    // フォルダ--------------------------------------------------------------
    // ROMパス
    strncpy( str, config->GetRomPath(), PATH_MAX );
    DelDelimiter( str );
    UnDelimiter( str );
    ui->lineEditFolderRom->setText(str);
    connect(ui->pushButtonRefFolderRom, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefFolderRom, ui->lineEditFolderRom);

    // TAPEパス
    strncpy( str, config->GetTapePath(), PATH_MAX );
    DelDelimiter( str );
    UnDelimiter( str );
    ui->lineEditFolderTape->setText(str);
    connect(ui->pushButtonRefFolderTape, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefFolderTape, ui->lineEditFolderTape);

    // DISKパス
    strncpy( str, config->GetDiskPath(), PATH_MAX );
    DelDelimiter( str );
    UnDelimiter( str );
    ui->lineEditFolderDisk->setText(str);
    connect(ui->pushButtonRefFolderDisk, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefFolderDisk, ui->lineEditFolderDisk);

    // 拡張ROMパス
    strncpy( str, config->GetExtRomPath(), PATH_MAX );
    DelDelimiter( str );
    UnDelimiter( str );
    ui->lineEditFolderExtRom->setText(str);
    connect(ui->pushButtonRefFolderExtRom, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefFolderExtRom, ui->lineEditFolderExtRom);

    // IMGパス
    strncpy( str, config->GetImgPath(), PATH_MAX );
    DelDelimiter( str );
    UnDelimiter( str );
    ui->lineEditFolderImg->setText(str);
    connect(ui->pushButtonRefFolderImg, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefFolderImg, ui->lineEditFolderImg);

    // WAVEパス
    strncpy( str, config->GetWavePath(), PATH_MAX );
    DelDelimiter( str );
    UnDelimiter( str );
    ui->lineEditFolderWave->setText(str);
    connect(ui->pushButtonRefFolderWave, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefFolderWave, ui->lineEditFolderWave);

    // FONTパス
    strncpy( str, config->GetFontPath(), PATH_MAX );
    DelDelimiter( str );
    UnDelimiter( str );
    ui->lineEditFolderFont->setText(str);
    connect(ui->pushButtonRefFolderFont, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefFolderFont, ui->lineEditFolderFont);

    // 色--------------------------------------------------------------------------
    // 16〜64の色IDに対応させる。
    for (int id = 16; id <= 64; id++){
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

    // ROMパッチ
    ui->checkBoxRomPatch->setChecked(config->GetRomPatch());

    // Turbo TAPE
    ui->checkBoxTurboTape->setChecked(config->GetTurboTAPE());

    // Boost Up
    ui->groupBoxBoostUp->setChecked(config->GetBoostUp());

    // BoostUp 最大倍率(N60モード)
    ui->lineEditBoost60->setText(QString::number(qMin(qMax(1, config->GetMaxBoost1()), 100)));

    // BoostUp 最大倍率(N60m/N66モード)
    ui->lineEditBoost66->setText(QString::number(qMin(qMax(1, config->GetMaxBoost2()), 100)));

    // ビデオキャプチャ RLE
    ui->checkBoxAviRLE->setChecked(config->GetAviRle());

    // 終了時 確認する
    ui->checkBoxCkQuit->setChecked(config->GetCkQuit());

    // 終了時 INIファイルを保存する
    ui->checkBoxSaveQuit->setChecked(config->GetSaveQuit());
}

void ConfigDialog::writeConfig()
{
    // 一時変数
    int iVal = 0;
    QString qStr;
    bool conv = false;

    // 基本------------------------------------------------
    // 機種
    if      (ui->radioButtonModel6001->isChecked())       config->SetModel(60);
    else if (ui->radioButtonModel6001mk2->isChecked())    config->SetModel(62);
    else if (ui->radioButtonModel6601->isChecked())       config->SetModel(64);
    else if (ui->radioButtonModel6001mk2SR->isChecked())  config->SetModel(66);
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
    // カラーモード
    if      (ui->radioButtonColor8Bit)  config->SetScrBpp(8);
    else if (ui->radioButtonColor16Bit) config->SetScrBpp(16);
    else if (ui->radioButtonColor24Bit) config->SetScrBpp(24);

    // MODE4カラー
    if      (ui->radioButtonColorBW->isChecked())   config->SetMode4Color(0);   // モノクロ
    else if (ui->radioButtonColorBR->isChecked())   config->SetMode4Color(1);   // 赤/青
    else if (ui->radioButtonColorRB->isChecked())   config->SetMode4Color(2);   // 青/赤
    else if (ui->radioButtonColorPG->isChecked())   config->SetMode4Color(3);   // ピンク/緑
    else if (ui->radioButtonColorGP->isChecked())   config->SetMode4Color(4);   // 緑/ピンク

    // スキャンライン
    config->SetScanLine(ui->checkBoxScanline->isChecked());

    // スキャンライン輝度
    iVal = ui->lineEditScanLineBr->text().toInt(&conv);
    if(conv){
        config->SetScanLineBr(conv);
    }

    // 4:3表示
    config->SetDispNTSC(ui->checkBoxDispNTSC->isChecked());

    // フルスクリーン
    config->SetFullScreen(ui->checkBoxFullscreen->isChecked());

    // ステータスバー表示状態
    config->SetStatDisp(ui->checkBoxStatDisp->isChecked());

    // フレームスキップ
    config->SetFrameSkip(ui->horizontalSliderFPS->value());

    // サウンド-------------------------------------------------------------------
    // サンプリングレート
    if      (ui->radioButtonSample441->isChecked())   config->SetSampleRate(44100);
    else if (ui->radioButtonSample225->isChecked())   config->SetSampleRate(22500);
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
    config->SetMasterVol(ui->horizontalSliderMasterVol->value());

    // 音声合成音量
    config->SetMasterVol(ui->horizontalSliderMasterVol->value());

    // TAPEモニタ音量
    config->SetMasterVol(ui->horizontalSliderMasterVol->value());

    // 入力関係--------------------------------------------------------
    // キーリピート間隔
    iVal = ui->lineEditKeyRepeat->text().toInt(&conv);
    if(conv){
        config->SetKeyRepeat(iVal);
    }

    // フォルダ-------------------------------------------------------------
    // ROMパス
    qStr = ui->lineEditFolderRom->text();
    if(QDir(qStr).exists()){
        config->SetRomPath(qStr.toLocal8Bit().data());
    }

    // TAPEパス
    qStr = ui->lineEditFolderTape->text();
    if(QDir(qStr).exists()){
        config->SetTapePath(qStr.toLocal8Bit().data());
    }

    // DISKパス
    qStr = ui->lineEditFolderDisk->text();
    if(QDir(qStr).exists()){
        config->SetDiskPath(qStr.toLocal8Bit().data());
    }

    // 拡張ROMパス
    qStr = ui->lineEditFolderExtRom->text();
    if(QDir(qStr).exists()){
        config->SetExtRomPath(qStr.toLocal8Bit().data());
    }

    // IMGパス
    qStr = ui->lineEditFolderImg->text();
    if(QDir(qStr).exists()){
        config->SetImgPath(qStr.toLocal8Bit().data());
    }

    // WAVEパス
    qStr = ui->lineEditFolderWave->text();
    if(QDir(qStr).exists()){
        config->SetWavePath(qStr.toLocal8Bit().data());
    }

    // FONTパス
    qStr = ui->lineEditFolderFont->text();
    if(QDir(qStr).exists()){
        config->SetFontPath(qStr.toLocal8Bit().data());
    }

    // ファイル--------------------------------------------------------
    // 拡張ROMファイル
    qStr = ui->lineEditExtRom->text();
    if(QFile(qStr).exists()){
        config->SetExtRomFile(qStr.toLocal8Bit().data());
    }

    // TAPE(LOAD)ファイル名
    qStr = ui->lineEditLoadTape->text();
    if(QFile(qStr).exists()){
        config->SetTapeFile(qStr.toLocal8Bit().data());
    }

    // TAPE(SAVE)ファイル名
    qStr = ui->lineEditSaveTape->text();
    config->SetSaveFile(qStr.toLocal8Bit().data());

    // DISKファイル名
    qStr = ui->lineEditDisk->text();
    if(QFile(qStr).exists()){
        config->SetDiskFile(qStr.toLocal8Bit().data());
    }

    // プリンタファイル名
    qStr = ui->lineEditPrinter->text();
    config->SetPrinterFile(qStr.toLocal8Bit().data());

    // 全角フォントファイル名
    qStr = ui->lineEditZenFont->text();
    if(QFile(qStr).exists()){
        config->SetFontFileZ(qStr.toLocal8Bit().data());
    }

    // 半角フォントファイル名
    qStr = ui->lineEditHanFont->text();
    if(QFile(qStr).exists()){
        config->SetFontFileH(qStr.toLocal8Bit().data());
    }

    // 色-----------------------------------------------------------------
    // 16〜64の色IDに対応させる。
    for (int id = 16; id <= 64; id++){
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

    // ROMパッチ
    config->SetRomPatch(ui->checkBoxRomPatch->isChecked());

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

    // ビデオキャプチャ RLE
    config->SetAviRle(ui->checkBoxAviRLE->isChecked());

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

void ConfigDialog::dispValue(QWidget *widget)
{
    QSlider* slider = qobject_cast<QSlider*>(this->sliderLabelMap->mapping(widget));
    if(slider){
        QLabel* label = qobject_cast<QLabel*>(widget);
        if(label){
            label->setText(QString::number(slider->value()));
        }
    }
}

void ConfigDialog::selectFile(QWidget *widget)
{
    char folder[PATH_MAX];
    QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
    if(edit){
        char* path = NULL;
        FileDlg dlg = EndofFileDlg;

        if(edit == ui->lineEditExtRom){
            dlg = FD_ExtRom; path = config->GetExtRomPath();
        } else if(edit == ui->lineEditLoadTape){
            dlg = FD_TapeLoad; path = config->GetTapePath();
        } else if(edit == ui->lineEditSaveTape){
            dlg = FD_TapeSave; path = config->GetTapePath();
        } else if(edit == ui->lineEditDisk){
            dlg = FD_Disk; path = config->GetDiskPath();
        } else if(edit == ui->lineEditPrinter){
            dlg = FD_Printer; path = NULL;
        } else if(edit == ui->lineEditZenFont){
            dlg = FD_FontZ; path = config->GetFontPath();
        } else if(edit == ui->lineEditHanFont){
            dlg = FD_FontH; path = config->GetFontPath();
        }

        strncpy(folder, edit->text().toLocal8Bit().data(), PATH_MAX);
        Delimiter(folder);
        OSD_FileSelect(NULL, dlg, folder, path);
        UnDelimiter(folder);
        if(strlen(folder) > 0){
            edit->setText(QString::fromLocal8Bit(folder));
        }
    }

}

void ConfigDialog::selectFolder(QWidget *widget)
{
    char folder[PATH_MAX];
    QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
    if(edit){
        strncpy(folder, edit->text().toLocal8Bit().data(), PATH_MAX);
        Delimiter(folder);
        OSD_FolderDiaog(NULL, folder);
        UnDelimiter(folder);
        if(strlen(folder) > 0){
            edit->setText(QString::fromLocal8Bit(folder));
        }
    }
}

