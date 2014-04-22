#include <QtWidgets>

#include "../event.h"
#include "../osd.h"
#include "../error.h"
#include "../pc6001v.h"
#include "../typedef.h"
#include "../console.h"
#include "../error.h"
#include "../osd.h"
#include "../common.h"

#include "renderview.h"
#include "qtp6vxapplication.h"


///////////////////////////////////////////////////////////
// フォントファイルチェック(無ければ作成する)
///////////////////////////////////////////////////////////
bool CheckFont( void )
{
    char FontFile[PATH_MAX];
    bool ret = true;

    sprintf( FontFile, "%s%s/%s", OSD_GetModulePath(), FONT_DIR, FONTH_FILE );
    ret |= ( !OSD_FileExist( FontFile ) && !OSD_CreateFont( FontFile, NULL, FSIZE ) );

    sprintf( FontFile, "%s%s/%s", OSD_GetModulePath(), FONT_DIR, FONTZ_FILE );
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

    int models[] = { 60, 61, 62, 66, 64, 68 };
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
  , TiltEnabled(false)
  , TiltDir(NEWTRAL)
  , TiltAngle(0.0)
{
    //アプリで定義した型名をシグナルの引数として使えるようにする
    qRegisterMetaType<HWINDOW>("HWINDOW");
    qRegisterMetaType<TiltDirection>("TiltDirection");

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
    if(!OSD_CreateModulePath()){
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

void QtP6VXApplication::createWindow(HWINDOW Wh, bool fsflag)
{
    RenderView* view = static_cast<RenderView*>(Wh);
    Q_ASSERT(view);
    QGraphicsScene* scene = view->scene();

    if(fsflag){
        view->setWindowState(view->windowState() | Qt::WindowFullScreen);
        view->showFullScreen();
    } else {
        view->setWindowState(view->windowState() & ~Qt::WindowFullScreen);
        if(!view->isVisible()){
            view->showNormal();
        }
#if 0
        if(!view->isMaximized()){
            view->setGeometry(100, 100, scene->width(), scene->height());
        }
#endif
    }
    view->fitContent();
    OSD_ClearWindow(Wh);
}

void QtP6VXApplication::layoutBitmap(HWINDOW Wh, int x, int y, double scaleX, double scaleY, QImage image)
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
    }
    pItem->setPixmap(QPixmap::fromImage(image));
    pItem->resetTransform();
    // アスペクト比に従って縦サイズを調整
    QTransform trans;
    trans.scale(scaleX, scaleY);
    trans.translate(x, y);
    pItem->setTransform(trans);
}

void QtP6VXApplication::getWindowImage(HWINDOW Wh, QRect pos, void **pixels)
{
    QGraphicsView* view = static_cast<QGraphicsView*>(Wh);
    Q_ASSERT(view);
    QGraphicsScene* scene = view->scene();
    QImage image(pos.width(), pos.height(), QImage::Format_RGB32);

    QPainter painter(&image);
    scene->render(&painter, image.rect(), pos);
    memcpy(*pixels, image.bits(), image.byteCount());
}

void QtP6VXApplication::clearLayout(HWINDOW Wh)
{
    QGraphicsView* view = static_cast<QGraphicsView*>(Wh);
    Q_ASSERT(view);
    Q_ASSERT(view->scene());
    QGraphicsScene* scene = view->scene();
    scene->clear();

#ifndef PANDORA
    // ステータスバー非表示またはフルスクリーン、かつTILTモードが有効になっている場合、背景を描く
    if( (!Cfg.GetDispStat() || Cfg.GetFullScreen()) &&
        #ifndef NOMONITOR
            !Cfg.GetMonDisp() &&
        #endif
            isTiltEnabled()){
        //画面に対する、枠を含めたサイズの比率
        qreal merginRatio = 1.0;
        QGraphicsPixmapItem* background = NULL;
        switch(this->Cfg.GetModel()){
        case 60:
        case 61:
            //初代機の場合はPC-6042Kを使う
            background = new QGraphicsPixmapItem(QPixmap::fromImage(QImage(":/res/background60.png")));
            merginRatio = 1.45;
            break;
        default:
            //それ以外の場合はPC-60m43を使う
            background = new QGraphicsPixmapItem(QPixmap::fromImage(QImage(":/res/background.png")));
            merginRatio = 1.2;
        }
        background->setTransformationMode(Qt::SmoothTransformation);
        //最前面に配置(他のアイテムのZ値はデフォルトの0)
        background->setZValue(1);
        QTransform trans;
        //画像の拡大倍率
        qreal ratio = qMax(scene->width() / background->sceneBoundingRect().width(),
                           scene->height() / background->sceneBoundingRect().height());
        qreal scaleRatio = ratio * merginRatio;

        int scaledWidth = background->sceneBoundingRect().width() * scaleRatio;
        int scaledHeight = background->sceneBoundingRect().height() * scaleRatio;

        //画像のオフセットを計算(枠の分だけ左上に移動)
        trans.translate(-(scaledWidth - scene->width()) / 2, -(scaledHeight - scene->height()) / 2);
        trans.scale(ratio * merginRatio, ratio * merginRatio);
        background->setTransform(trans);
        scene->addItem(background);
    }
#endif
}

void QtP6VXApplication::showPopupMenu(int x, int y)
{
    //メニュー表示中に右クリックすると二重にメニューが表示されてしまうため、その対処
    if(MenuMutex.tryLock()){
        P6Core->Stop();
        P6Core->ShowPopupImpl(x, y);
        P6Core->Start();
        MenuMutex.unlock();
    }
}


bool QtP6VXApplication::isTiltEnabled()
{
    QMutexLocker lock(&propretyMutex);
    return TiltEnabled;
}

void QtP6VXApplication::enableTilt(bool enable)
{
    QMutexLocker lock(&propretyMutex);
    TiltEnabled = enable;
}

TiltDirection QtP6VXApplication::getTiltDirection()
{
    QMutexLocker lock(&propretyMutex);
    return TiltDir;
}

void QtP6VXApplication::setTiltDirection(TiltDirection dir)
{
    QMutexLocker lock(&propretyMutex);
    TiltDir = dir;
}

void QtP6VXApplication::setTiltAngle(qreal angle)
{
    QMutexLocker lock(&propretyMutex);
    TiltAngle = angle;
}

qreal QtP6VXApplication::getTiltAngle()
{
    QMutexLocker lock(&propretyMutex);
    return TiltAngle;
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
        if(OSD_Message( QString(tr("ROMファイルが見つかりません。\n"
                                "ROMフォルダ(%1)にROMファイルをコピーするか、"
                                "別のROMフォルダを指定してください。\n"
                                "別のROMフォルダを指定しますか?")).arg(Cfg.GetRomPath()).toUtf8().constData(), MSERR_ERROR, OSDM_YESNO | OSDM_ICONERROR ) == OSDR_YES){
            //ROMフォルダ再設定
            char folder[PATH_MAX];
            strncpy(folder, Cfg.GetRomPath(), PATH_MAX);
            OSD_AddDelimiter(folder);
            OSD_FolderDiaog(NULL, folder);
            OSD_DelDelimiter(folder);

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
            int ret = OSD_Message( tr("ROMイメージのCRCが不正です。\n"
                                   "CRCが一致しないROMを使用すると、予期せぬ不具合を引き起こす可能性があります。\n"
                                   "それでも起動しますか?").toUtf8(),
                                   MSERR_ERROR, OSDM_YESNO | OSDM_ICONWARNING );
            if(ret == OSDR_YES) {
                Cfg.SetCheckCRC(false);
                Cfg.Write();
                P6Core->deleteLater();
                emit vmRestart();
                return;
            } else {
                P6Core->deleteLater();
                exit();
                return;
            }
        } else {
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
            exit();
            return;
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

    // P6オブジェクトを解放
    // 本来QtオブジェクトはdeleteLater()を使うべきであるが、再起動の場合、
    // オブジェクト解放の遅延により、次のP6Coreインスタンスが生成されてから
    // デストラクタが呼ばれるため、次のP6Coreインスタンスが初期化時に確保したリソース
    // (ジョイスティックなど)を解放してしまう。のでやむなくdeleteを使う。
    // disconnectは解放後に飛んできたシグナルを処理させないため。
    P6Core->disconnect();
    delete P6Core;
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

        // 以下の場合は入力イベントをエミュレータ側に渡さず、Qtに渡す
        // (メニューやダイアログでキーボードを使えるようにするため)
        bool processKeyEventInQt = false;
        // ・エミュレーションポーズ中
        // 　例外としてF9キー(ポーズ解除)とF12(スナップショット)は
        // 　エミュレータで受け付ける
        if(!(P6Core && !P6Core->IsCancel() && !P6Core->GetPauseEnable())
                && keyCode != Qt::Key_F9
                && keyCode != Qt::Key_F12){
            processKeyEventInQt = true;
        }
        // ・エミュレータウィンドウ以外のウィンドウ(Aboutダイアログ、環境設定ダイアログ)が
        // 　最前面にある場合
        if(activeWindow() && activeWindow()->metaObject()->className() != QString("RenderView")){
            processKeyEventInQt = true;
        }
        // メニュー表示中の場合
        if(!MenuMutex.tryLock()){
            processKeyEventInQt = true;
        } else {
            MenuMutex.unlock();
        }
        if(processKeyEventInQt){
            return QtSingleApplication::notify(receiver, event);
        }

        // 特殊キー対策
        quint32 nativeKey = ke->nativeScanCode();
        qDebug("nativekeycode %d\n", nativeKey);

        //X11の場合
        if (QGuiApplication::platformName() == QLatin1String("xcb")){
            // 「ろ」
            if(keyCode == Qt::Key_Backslash){
                keyCode = nativeKey == 97 ? Qt::Key_Underscore : Qt::Key_Backslash;
            }
        }
        //Windowsの場合
        else if (QGuiApplication::platformName() == QLatin1String("windows")){
            // 「ろ」
            if(keyCode == Qt::Key_Backslash){
                keyCode = nativeKey == 115 ? Qt::Key_Underscore : Qt::Key_Backslash;
            }
            // 変換
            else if(nativeKey == 121){
                keyCode = Qt::Key_Henkan;
            }
            // 無変換
            else if(nativeKey == 123){
                keyCode = Qt::Key_Muhenkan;
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
        ev.type = EV_CONTEXTMENU;
        OSD_PushEvent(ev.type);
        break;
    }
    case QEvent::GraphicsSceneWheel:
    {
        QGraphicsSceneWheelEvent* we = dynamic_cast<QGraphicsSceneWheelEvent*>(event);
        Q_ASSERT(we);
        ev.type = EV_MOUSEBUTTONUP;
        ev.mousebt.button = we->delta() > 0 ? MBT_WHEELUP : MBT_WHEELDOWN;
        OSD_PushEvent(ev);
        break;
    }
    case QEvent::GraphicsSceneMouseRelease:
    {
        QGraphicsSceneMouseEvent* me = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
        Q_ASSERT(me);
        if(me->button() == Qt::LeftButton){
            ev.type = EV_MOUSEBUTTONUP;
            ev.mousebt.button = MBT_LEFT;
            OSD_PushEvent(ev);
        }
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


