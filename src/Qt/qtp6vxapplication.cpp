#include <QtWidgets>

#include "../event.h"
#include "../osd.h"
#include "../error.h"
#include "../pc6001v.h"
#include "../typedef.h"
#include "../console.h"
#include "../error.h"
#include "../osd.h"

#include "renderview.h"
#include "qtp6vxapplication.h"

///////////////////////////////////////////////////////////
// フォントファイルチェック(無ければ作成する)
///////////////////////////////////////////////////////////
bool CheckFont( void )
{
    char FontFile[PATH_MAX];
    bool ret = true;

    sprintf( FontFile, "%s%s/%s", OSD_GetConfigPath(), FONT_DIR, FONTH_FILE );
    ret |= ( !OSD_FileExist( FontFile ) && !OSD_CreateFont( FontFile, NULL, FSIZE ) );

    sprintf( FontFile, "%s%s/%s", OSD_GetConfigPath(), FONT_DIR, FONTZ_FILE );
    ret |= ( !OSD_FileExist( FontFile ) && !OSD_CreateFont( NULL, FontFile, FSIZE ) );

    return ret;
}

///////////////////////////////////////////////////////////
// ROMファイル存在チェック&機種変更
///////////////////////////////////////////////////////////
bool SerchRom( CFG6 *cfg )
{
    char RomSerch[PATH_MAX];

    int IniModel = cfg->GetModel();
    sprintf( RomSerch, "%s*.%2d", cfg->GetRomPath(), IniModel );
    if( OSD_FileExist( RomSerch ) ){
        Error::SetError( Error::NoError );
        return true;
    }

    int models[] = { 60, 62, 66 };
    for( int i=0; i < COUNTOF(models); i++ ){
        sprintf( RomSerch, "%s*.%2d", cfg->GetRomPath(), models[i] );
        if( OSD_FileExist( RomSerch ) ){
            cfg->SetModel( models[i] );
            cfg->Write();
            Error::SetError( Error::RomChange );
            return true;
        }
    }
    Error::SetError( Error::NoRom );
    return false;
}

QtP6VXApplication::QtP6VXApplication(int &argc, char **argv) :
    QtSingleApplication(argc, argv)
  , P6Core(NULL)
  , Restart(EL6::Quit)
  , Adaptor(new EmulationAdaptor())
{
    //HWINDOWという型名をシグナルの引数として使えるようにする
    qRegisterMetaType<HWINDOW>("HWINDOW");

    // エミュレーションコア部分用スレッドを生成
    QThread* emulationThread = new QThread(this);
    emulationThread->start();
    Adaptor->moveToThread(emulationThread);

    //ウィンドウを閉じても終了しない。
    //終了はEL6::Quitが返ってきた時に行う。
    setQuitOnLastWindowClosed (false);

    connect(this, SIGNAL(initialized()), this, SLOT(executeEmulation()));
    connect(this, SIGNAL(vmPrepared()), Adaptor, SLOT(doEventLoop()));
    connect(this, SIGNAL(vmRestart()), this, SLOT(executeEmulation()));
    connect(Adaptor, SIGNAL(finished()), this, SLOT(postExecuteEmulation()));
}

QtP6VXApplication::~QtP6VXApplication()
{
    Adaptor->thread()->exit();
    Adaptor->deleteLater();
}

void QtP6VXApplication::startup()
{
    // 二重起動禁止
    if( isRunning() ) {
        exit();
        return;
    }

    // 設定ファイルパスを作成
    if(!OSD_CreateConfigPath()){
        exit();
        return;
    }

    // OSD関連初期化
    if( !OSD_Init() ){
        Error::SetError( Error::InitFailed );
        OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
        OSD_Quit();	// 終了処理
        exit();
        return;
    }

    // フォントファイルチェック
    if( !CheckFont() ){
        Error::SetError( Error::FontCreateFailed );
        OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
        Error::SetError( Error::NoError );
    }

    // コンソール用フォント読込み
    char FontZ[PATH_MAX], FontH[PATH_MAX];
    sprintf( FontZ, ":/res/%s/%s", FONT_DIR, FONTZ_FILE );
    sprintf( FontH, ":/res/%s/%s", FONT_DIR, FONTH_FILE );
    if( !JFont::OpenFont( FontZ, FontH ) ){
        Error::SetError( Error::FontLoadFailed );
        OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
        Error::SetError( Error::NoError );
    }


    // INIファイル読込み
    if( !Cfg.Init() ){
        switch( Error::GetError() ){
        case Error::IniDefault:
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
            Error::SetError( Error::NoError );
            break;

        default:
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
            OSD_Quit();			// 終了処理
            exit();
            return;
        }
    }

    emit initialized();
}

void QtP6VXApplication::createWindow(HWINDOW Wh, int bpp, bool fsflag)
{
    RenderView* view = static_cast<RenderView*>(Wh);
    Q_ASSERT(view);
    QGraphicsScene* scene = view->scene();

    OSD_ClearWindow(Wh);
    if(fsflag){
        view->showFullScreen();
    } else {
        view->showNormal();
        //qDebug("scene:%dx%d view:%dx%d", scene->width(), scene->height(), view->width(), view->height());
        //Windowsではウィンドウ表示が完了しきらないうちにresize()を呼ぶと
        //タイトルバーが画面からはみ出るので、適当に左上にマージンを取る。
        view->setGeometry(100, 100, scene->width(), scene->height());
    }
    view->fitContent();
}

void QtP6VXApplication::layoutBitmap(HWINDOW Wh, int x, int y, double aspect, QImage image)
{
    //QtではSceneRectの幅を返す
    QGraphicsView* view = static_cast<QGraphicsView*>(Wh);
    Q_ASSERT(view);
    QGraphicsScene* scene = view->scene();

    // 指定座標に生成済みのQPixmapItemが存在するかチェック
    // (同一座標にビットマップが重なることはないという前提)
    QGraphicsItem* item = NULL;
    QGraphicsPixmapItem* pItem = NULL;
    foreach(item, scene->items()){
        if(item->scenePos() == QPointF(x, y)){
            pItem = dynamic_cast<QGraphicsPixmapItem*>(item);
            break;
        }
    }

    if(pItem == NULL){
        // 既存のQPixmapItemが存在しない場合は生成
        pItem = new QGraphicsPixmapItem(NULL);
        scene->addItem(pItem);
        pItem->setTransformationMode(Qt::SmoothTransformation);
        QTransform trans;
        trans.scale(1, aspect);
        trans.translate(x, y);
        pItem->setTransform(trans);
    }
    pItem->setPixmap(QPixmap::fromImage(image));

}

void QtP6VXApplication::clearLayout(HWINDOW Wh)
{
    QGraphicsView* view = static_cast<QGraphicsView*>(Wh);
    Q_ASSERT(view);
    Q_ASSERT(view->scene());
    view->scene()->clear();
}

void QtP6VXApplication::showPopupMenu(int x, int y)
{
    P6Core->Stop();
    P6Core->ShowPopupImpl(x, y);
    P6Core->Start();
}

//仮想マシンを開始させる
void QtP6VXApplication::executeEmulation()
{
    // ROMファイル存在チェック&機種変更
    if( SerchRom( &Cfg ) ){
        if( Error::GetError() != Error::NoError ){
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDM_OK | OSDM_ICONWARNING );
            Error::SetError( Error::NoError );
        }
    }else{
        if(OSD_Message( QString("ROMファイルが見つかりません。\n"
                                "ROMフォルダ(%1)にROMファイルをコピーするか、"
                                "別のROMフォルダを指定してください。\n"
                                "別のROMフォルダを指定しますか?").arg(Cfg.GetRomPath()).toUtf8().data(), MSERR_ERROR, OSDM_YESNO | OSDM_ICONERROR ) == OSDR_YES){
            //ROMフォルダ再設定
            char folder[PATH_MAX];
            strncpy(folder, Cfg.GetRomPath(), PATH_MAX);
            Delimiter(folder);
            OSD_FolderDiaog(NULL, folder);
            UnDelimiter(folder);

            if(strlen(folder) > 0){
                Cfg.SetRomPath(folder);
                Cfg.Write();
                Restart = EL6::Restart;
            } else {
                exit();
                return;
            }
        } else {
            exit();
            return;
        }
        emit vmRestart();
        return;
    }

    // 機種別P6オブジェクト確保
    P6Core = new QtEL6;
    if( !P6Core ){
        exit();
        return;
    }

    // VM初期化
    if( !P6Core->Init( &Cfg ) ){
        if(Error::GetError() == Error::RomCrcNG){
            // CRCが合わない場合
            int ret = OSD_Message( "ROMイメージのCRCが不正です。\n"
                                   "CRCが一致しないROMを使用すると、予期せぬ不具合を引き起こす可能性があります。\n"
                                   "それでも起動しますか?",
                                   MSERR_ERROR, OSDM_YESNO | OSDM_ICONWARNING );
            if(ret == OSDR_YES) {
                Cfg.SetCheckCRC(false);
                Cfg.Write();
                Restart = EL6::Restart;
            }
        } else {
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
            exit();
        }
    }

    switch( Restart ){
    case EL6::Dokoload:	// どこでもLOAD?
        P6Core->DokoDemoLoad( Cfg.GetDokoFile() );
        break;

    case EL6::Replay:	// リプレイ再生?
    {
        char temp[PATH_MAX];
        strncpy( temp, Cfg.GetDokoFile(), PATH_MAX );
        P6Core->DokoDemoLoad( temp );
        P6Core->REPLAY::StartReplay( temp );
    }
        break;

    default:
        break;
    }

    // 以降、ウィンドウが閉じたらアプリを終了する
    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(terminateEmulation()));

    // VM実行
    Adaptor->setEmulationObj(P6Core);
    emit vmPrepared();
    P6Core->Start();
}

//仮想マシン終了後の処理
void QtP6VXApplication::postExecuteEmulation()
{
    Restart = Adaptor->getReturnCode();
    Adaptor->setEmulationObj(NULL);
    P6Core->Stop();
    delete P6Core;	// P6オブジェクトを開放
    P6Core = NULL;

    // 再起動ならばINIファイル再読込み
    if( Restart == EL6::Restart ){
        if( !Cfg.Init() ){
            Error::SetError( Error::IniDefault );
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
            Error::SetError( Error::NoError );
        }
    }

    if( Restart == EL6::Quit ){
        // 終了処理
        OSD_Quit();
        exit();
        return;
    } else {
        emit vmRestart();
    }
}

void QtP6VXApplication::terminateEmulation()
{
    OSD_PushEvent( EV_QUIT );
}

bool QtP6VXApplication::notify ( QObject * receiver, QEvent * event )
{
    Event ev;
    ev.type = EV_NOEVENT;

    switch(event->type()){
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
        QKeyEvent* ke = dynamic_cast<QKeyEvent*>(event);
        Q_ASSERT(ke);
        int keyCode = ke->key();

        // 「ろ」が入力できない対策
        quint32 nativeKey = ke->nativeScanCode();
        //X11の場合
        if (QGuiApplication::platformName() == QLatin1String("xcb")){
            if(keyCode == Qt::Key_Backslash){
                keyCode = nativeKey == 97 ? Qt::Key_Underscore : Qt::Key_Backslash;
            }
        }
        //Windowsの場合
        else if (QGuiApplication::platformName() == QLatin1String("windows")){
            if(keyCode == Qt::Key_Backslash){
                keyCode = nativeKey == 115 ? Qt::Key_Underscore : Qt::Key_Backslash;
            }
        }

        ev.type        = event->type() == QEvent::KeyPress ? EV_KEYDOWN : EV_KEYUP;
        ev.key.state   = event->type() == QEvent::KeyPress ? true : false;
        ev.key.sym     = OSD_ConvertKeyCode( keyCode );
        ev.key.mod	   = (PCKEYmod)(
                    ( ke->modifiers() & Qt::ShiftModifier ? KVM_SHIFT : KVM_NONE )
                    | ( ke->modifiers() & Qt::ControlModifier ? KVM_CTRL : KVM_NONE )
                    | ( ke->modifiers() & Qt::AltModifier ? KVM_ALT : KVM_NONE )
                    | ( ke->modifiers() & Qt::MetaModifier ? KVM_META : KVM_NONE )
                    | ( ke->modifiers() & Qt::KeypadModifier ? KVM_NUM : KVM_NONE )
                    //#PENDING CAPSLOCKは検出できない？
                    //| ( ke->modifiers() & Qt::caps ? KVM_NUM : KVM_NONE )
                    );
        ev.key.unicode = ke->text().utf16()[0];
        OSD_PushEvent(ev);
        break;
    }
    case QEvent::ContextMenu:
    {
        ev.type        = EV_CONTEXTMENU;
        OSD_PushEvent(ev.type);
        break;
    }
    default:;
    }

    if(ev.type == EV_NOEVENT){
        return QtSingleApplication::notify(receiver, event);
    } else {
        return true;
    }
}
