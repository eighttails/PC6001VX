#include "configdialog.h"
#include "ui_configdialog.h"

#include "../config.h"
#include "../osd.h"

ConfigDialog::ConfigDialog(cConfig* cfg, QWidget *parent) :
    QDialog(parent),
    config(cfg),
    sliderLabelMap(new QSignalMapper(this)),
    folderRefMap(new QSignalMapper(this)),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    connect(ui->horizontalSliderFPS, SIGNAL(valueChanged(int)), this, SLOT(dispFPS(int)));
    // スライダーを動かしたらラベルに値を反映
    connect(sliderLabelMap, SIGNAL(mapped(QWidget*)), this, SLOT(dispValue(QWidget*)));
    // 参照ボタンを押したらファイル、フォルダ選択ダイアログを出し、ラインエディットに反映
    connect(folderRefMap, SIGNAL(mapped(QWidget*)), this, SLOT(selectFile(QWidget*)));

    assignColorButton();
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
    connect(ui->pushButtonRefExtRom, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefExtRom, ui->lineEditExtRom);

    // TAPE(LOAD)ファイル名
    strncpy( str, config->GetTapeFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditLoadTape->setText(str);
    connect(ui->pushButtonRefLoadTape, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefLoadTape, ui->lineEditLoadTape);

    // TAPE(SAVE)ファイル名
    strncpy( str, config->GetSaveFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditSaveTape->setText(str);
    connect(ui->pushButtonRefSaveTape, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefSaveTape, ui->lineEditSaveTape);

    // DISKファイル名
    strncpy( str, config->GetDiskFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditDisk->setText(str);
    connect(ui->pushButtonRefDisk, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefDisk, ui->lineEditDisk);

    // プリンタファイル名
    strncpy( str, config->GetPrinterFile(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditPrinter->setText(str);
    connect(ui->pushButtonRefPrinter, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefPrinter, ui->lineEditPrinter);

    // 全角フォントファイル名
    strncpy( str, config->GetFontFileZ(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditZenFont->setText(str);
    connect(ui->pushButtonRefZenFont, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefZenFont, ui->lineEditZenFont);

    // 半角フォントファイル名
    strncpy( str, config->GetFontFileH(), PATH_MAX );
    UnDelimiter( str );
    ui->lineEditHanFont->setText(str);
    connect(ui->pushButtonRefHanFont, SIGNAL(clicked()), folderRefMap, SLOT(map()));
    folderRefMap->setMapping(ui->pushButtonRefHanFont, ui->lineEditHanFont);

}

void ConfigDialog::writeConfig()
{
}

// 色設定ボタンと設定項目を対応させる
void ConfigDialog::assignColorButton()
{
    // 16〜64の色IDに対応させる。
    for (int id = 16; id <= 64; id++){
        QString buttonName = QString("pushButtonColor%1").arg(id);
        // ダイアログから動的に部品を取得する
        ColorButton* button = this->findChild<ColorButton*>(buttonName);
        button->initialize(id, config);
    }
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
    char str[PATH_MAX];
    QLineEdit* edit = qobject_cast<QLineEdit*>(widget);
    if(edit){
        strncpy(str, edit->text().toLocal8Bit().data(), PATH_MAX);
        Delimiter(str);
        OSD_FolderDiaog(NULL, str);
        UnDelimiter(str);
        edit->setText(QString::fromLocal8Bit(str));
    }
}

