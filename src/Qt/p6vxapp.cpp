#include <QtWidgets>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QThread>

#include <map>

#include "../pc6001v.h"
#include "../event.h"
#include "../osd.h"
#include "../error.h"
#include "../console.h"
#include "../error.h"
#include "../osd.h"
#include "../common.h"
#include "../keyboard.h"

#include "renderview.h"
#include "mainwidget.h"
#include "keypanel.h"
#include "keystatewatcher.h"
#include "p6vxapp.h"


const QString P6VXApp::keyGeometry				= "window/geometry";
const QString P6VXApp::keyMaximized				= "window/maximized";
const QString P6VXApp::keyHwAccel				= "graph/hwAccel";
const QString P6VXApp::keyFixMagnification		= "graph/fixMagnification";
const QString P6VXApp::keyMagnification			= "graph/magnification";
const QString P6VXApp::keyKeyPanelVisible		= "keypalette/visible";
const QString P6VXApp::keyKeyPanelPosition		= "keypalette/position";
const QString P6VXApp::keyVirtualKeyVisible		= "virtualkey/visible";
const QString P6VXApp::keyVirtualKeyTabIndex	= "virtualkey/tabindex";
const QString P6VXApp::keyVirtualKeyPosition	= "virtualkey/positionInLandscape";
const QString P6VXApp::keyTimerResolution		= "generic/timerResolution";


///////////////////////////////////////////////////////////
// ROMファイル存在チェック&機種変更
///////////////////////////////////////////////////////////
bool SerchRom( std::shared_ptr<CFG6> &cfg )
{
	P6VPATH RomSearch;

	int IniModel = cfg->GetValue(CV_Model);
	RomSearch = QSTR2P6VPATH(QString("*.%1").arg( IniModel ));
	OSD_AddPath( RomSearch, cfg->GetValue(CF_RomPath), RomSearch );
	if( OSD_FileExist( RomSearch ) ){
		Error::Clear();
		return true;
	}

	int models[] = { 60, 61, 62, 66, 64, 68 };
	for( int i=0; i < COUNTOF(models); i++ ){
		RomSearch = QSTR2P6VPATH(QString("*.%1").arg( models[i] ));
		OSD_AddPath( RomSearch, cfg->GetValue(CF_RomPath), RomSearch );
		if( OSD_FileExist( RomSearch ) ){
			cfg->SetValue( CV_Model,  models[i] );
			Error::SetError( Error::RomChange );
			return true;
		}
	}
	Error::SetError( Error::NoRom );
	return false;
}

P6VXApp::P6VXApp(int &argc, char **argv)
#ifndef NOSINGLEAPP
	: ParentAppClass(argc, argv, true, SingleApplication::User | SingleApplication::SecondaryNotification)
	#else
	: ParentAppClass(argc, argv)
#endif
	, P6Core(nullptr)
	, Restart(EL6::Quit)
	, Cfg(std::make_shared<CFG6>())
	, Adaptor(new EmulationAdaptor())
	, KPanel(nullptr)
	, Setting(P6VPATH2QSTR(OSD_GetConfigPath()) + "/pc6001vx.ini", QSettings::IniFormat)
	, TiltEnabled(false)
	, TiltDir(NEWTRAL)
	, TiltStep(0)
	, SafeMode(false)
{
	// アプリで定義した型名をシグナルの引数として使えるようにする
	qRegisterMetaType<HWINDOW>("HWINDOW");
	qRegisterMetaType<TiltDirection>("TiltDirection");
	qRegisterMetaType<FileMode>("FileMode");
	qRegisterMetaType<EL6::ReturnCode>("EL6::ReturnCode");

	// エミュレーションコア部分用スレッドを生成
	QThread* emulationThread = new QThread(this);
	emulationThread->start();
	Adaptor->moveToThread(emulationThread);

	// ウィンドウを閉じても終了しない。
	// 終了はEL6::Quitが返ってきた時に行う。
	setQuitOnLastWindowClosed (false);

	connect(this, SIGNAL(initialized()), this, SLOT(executeEmulation()), Qt::QueuedConnection);
	connect(this, SIGNAL(vmPrepared(EL6::ReturnCode)), Adaptor, SLOT(doEventLoop(EL6::ReturnCode)), Qt::QueuedConnection);
	connect(this, SIGNAL(vmRestart()), this, SLOT(executeEmulation()), Qt::QueuedConnection);
	connect(Adaptor, SIGNAL(finished()), this, SLOT(postExecuteEmulation()), Qt::QueuedConnection);

	// スクリーンセーバー抑止用タイマー
	QTimer* timer = new QTimer(this);
	timer->setInterval(30000);
	connect(timer, SIGNAL(timeout()), this, SLOT(inhibitScreenSaver()));
	timer->start();
}

P6VXApp::~P6VXApp()
{
	if (Adaptor){
		Adaptor->thread()->exit();
		Adaptor->thread()->wait();
		Adaptor->deleteLater();
	}
	if (!MWidget.isNull()){
		MWidget->deleteLater();
	}
}

RenderView *P6VXApp::getView()
{
	return MWidget->getMainView();
}

KeyPanel *P6VXApp::getKeyPanel()
{
	return KPanel;
}

VirtualKeyTabWidget* P6VXApp::getVirtualKeyboard()
{
	return MWidget->getVirtualKeyboard();
}

const QVector<QRgb> &P6VXApp::getPaletteTable() const
{
	return PaletteTable;
}

void P6VXApp::startup()
{
#ifndef NOSINGLEAPP
	// 二重起動禁止
	if( OSD_IsWorking() ) {
#ifdef Q_OS_WINDOWS
		AllowSetForegroundWindow( DWORD( primaryPid() ) );
		sendMessage("RAISE_WIDGET");
#endif
		exit();
	}
#endif

#ifdef Q_OS_ANDROID
	// #TODO
#if 0
	// SDカードへのアクセス許可を要求
	QtAndroid::PermissionResult r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
	if(r == QtAndroid::PermissionResult::Denied) {
		QtAndroid::requestPermissionsSync( QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE" );
		r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
		if(r == QtAndroid::PermissionResult::Denied) {
			OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr, tr("Storage access denied.")).toStdString(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONERROR );
		}
	}
#endif
#endif

	// 設定ファイルフォルダの存在チェック&作成
	if( !OSD_FileExist( OSD_GetConfigPath() ) ) OSD_CreateFolder( OSD_GetConfigPath() );


	// OSD関連初期化
	if( !OSD_Init() ){
		Error::SetError( Error::InitFailed );
		OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr, Error::GetErrorText(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONERROR );
		exit();
		return;
	}

	// 各種フォルダの存在チェック&作成
	std::vector<TCPath> paths = { CF_RomPath, CF_TapePath, CF_DiskPath, CF_ExtRomPath, CF_ImgPath, CF_WavePath, CF_FontPath, CF_DokoPath };
	for( auto& cf : paths ){
		P6VPATH folderPath;
		OSD_AddPath( folderPath, OSD_GetConfigPath(), Cfg->GetValue( cf ) );
		if( !OSD_FileExist( folderPath ) ) {
			OSD_CreateFolder( folderPath );
		}
	}

	// コンソール用フォント読込み
	P6VPATH FontZ, FontH;
	FontZ = QSTR2P6VPATH(QString(":/res/font/%1").arg(FILE_FONTZ));
	FontH = QSTR2P6VPATH(QString(":/res/font/%1").arg(FILE_FONTH));
	if( !JFont::OpenFont( FontZ, FontH ) ){
		Error::SetError( Error::FontLoadFailed );
		OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr, Error::GetErrorText(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONERROR );
		Error::SetError( Error::NoError );
	}

	// P6VXデフォルト設定
#ifndef NO_HWACCEL
	setDefaultSetting(keyHwAccel, true);
#endif
	setDefaultSetting(keyFixMagnification, false);
	setDefaultSetting(keyMagnification, 1.0);
	setDefaultSetting(keyKeyPanelVisible, false);
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	setDefaultSetting(keyVirtualKeyVisible, true);
#else
	setDefaultSetting(keyVirtualKeyVisible, false);
#endif
	setDefaultSetting(keyVirtualKeyPosition, 0);
#ifndef DELAY_TIMER_MS_DEFAULT
#define DELAY_TIMER_MS_DEFAULT 1
#endif
	setDefaultSetting(keyTimerResolution, DELAY_TIMER_MS_DEFAULT);

	// INIファイル読込み
	if( !Cfg->Init() ){
		switch( Error::GetError() ){
		case Error::IniDefault:
			OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr, Error::GetErrorText(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONWARNING );
			Error::SetError( Error::NoError );
			break;

		default:
			OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr, Error::GetErrorText(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONERROR );
			exit();
			return;
		}
	}
	overrideSettings(Cfg);

	// ウィンドウ、ウィジェットの生成
	MWidget = new MainWidget();
	KPanel = new KeyPanel(MWidget);

#ifndef NOSINGLEAPP
	// 二重起動時には既存ウィンドウの方を前面に出す
	connect(this, &SingleApplication::instanceStarted, this, &P6VXApp::raiseWidget);
#endif

	emit initialized();
}

int P6VXApp::showMessageBox(void *hwnd, const char *mes, const char *cap, int type)
{
	QWidget* parent = reinterpret_cast<QWidget*>(hwnd);
	QMessageBox::StandardButtons Type = QMessageBox::Ok;
	QMessageBox::Icon IconType = QMessageBox::Information;

	// メッセージボックスのタイプ
	switch( type&0x000f ){
	case OSDM_OK:			Type = QMessageBox::Ok;                         break;
	case OSDM_OKCANCEL:		Type = QMessageBox::Ok | QMessageBox::Cancel;	break;
	case OSDM_YESNO:		Type = QMessageBox::Yes | QMessageBox::No;	break;
	case OSDM_YESNOCANCEL:	Type = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;    break;
	}

	// メッセージボックスのアイコンタイプ
	switch( type&0x00f0 ){
	case OSDM_ICONERROR:	IconType = QMessageBox::Critical;	break;
	case OSDM_ICONQUESTION:	IconType = QMessageBox::Question;	break;
	case OSDM_ICONWARNING:	IconType = QMessageBox::Warning;	break;
	case OSDM_ICONINFO:		IconType = QMessageBox::Information;	break;
	}

	QMessageBox mb(IconType, TRANS(cap), TRANS(mes), Type, parent);
	int res = mb.exec();

	switch( res ){
	case QMessageBox::Ok:	return OSDR_OK;
	case QMessageBox::Yes:	return OSDR_YES;
	case QMessageBox::No:	return OSDR_NO;
	default:	return OSDR_CANCEL;
	}
}

bool P6VXApp::fileDialog(void *hwnd, FileMode mode, const char *title, const char *filter, char *fullpath, char *path, const char *ext)
{
	QSharedPointer<QFileDialog> dialog(createFileDialog(hwnd));
	QString result;
	// 検索パスが指定されていない場合はホームフォルダとする
	QString pathStr = strlen(path) ? QFileInfo(path).dir().path() : QDir::homePath();

	dialog->setWindowTitle(title);
	dialog->setDirectory(pathStr);
#ifndef Q_OS_ANDROID // Androidでは拡張子フィルタがうまく働かない
	dialog->setNameFilter(filter);
#endif

	OSD_ShowCursor(true);
	if(mode == FM_Save){
		dialog->setFileMode(QFileDialog::AnyFile);
		if (dialog->exec() == QDialog::Accepted) {
			result = dialog->selectedFiles().value(0);
		}
		if(result.isEmpty()) return false;
		// 入力されたファイル名に拡張子がついていない場合は付与する
		QFileInfo info(result);
		if(info.suffix() != ext){
			result += QString(".") + ext;
		}
		if (OSD_FileExist(QSTR2P6VPATH(result))){
			if (OSD_Message(P6Core ? P6Core->GetWindowHandle() : nullptr,
							tr("ファイルはすでに存在しています。上書きしますか?").toStdString(),
							title, OSDM_OKCANCEL | OSDM_ICONQUESTION)	== OSDM_OKCANCEL){
				return false;
			}
		}
	} else {
		dialog->setFileMode(QFileDialog::ExistingFile);
		if (dialog->exec() == QDialog::Accepted) {
			result = dialog->selectedFiles().value(0);
		}
		if(result.isEmpty()) return false;
	}

	QDir dir(result);

	if( path ) strcpy( path, EncodeContentURI(dir.path()).toUtf8().constData() );
	if( fullpath ) strcpy( fullpath, EncodeContentURI(result).toUtf8().constData() );
	QFile file(result);
	return true;
}

bool P6VXApp::folderDialog(void *hwnd, char *Result)
{
	QSharedPointer<QFileDialog> dialog(createFileDialog(hwnd));
	dialog->setDirectory(strcmp(Result, "/") ? Result : QDir::homePath());
	dialog->setFileMode(QFileDialog::Directory);
	dialog->setOption(QFileDialog::ShowDirsOnly);

	QByteArray result;
	OSD_ShowCursor(true);
	if (dialog->exec() == QDialog::Accepted) {
		result = EncodeContentURI(dialog->selectedFiles().value(0)).toUtf8();
	} else {
		return false;
	}

	strcpy(Result, result);
	return true;
}

void P6VXApp::createWindow(HWINDOW Wh, bool fsflag)
{
	RenderView* view = reinterpret_cast<RenderView*>(Wh);
	Q_ASSERT(view);

#ifdef ALWAYSFULLSCREEN
	MWidget->showFullScreen();
#else
	if (fsflag) {
		MWidget->setWindowState(MWidget->windowState() | Qt::WindowFullScreen);
		MWidget->showFullScreen();
		MWidget->updateLayout();
	} else {
		MWidget->setWindowState(MWidget->windowState() & ~Qt::WindowFullScreen);
		if(!MWidget->isVisible()){
			MWidget->showNormal();
		}
	}
#endif

	view->fitContent();
	OSD_ClearWindow(Wh);

	if(getSetting(keyKeyPanelVisible).toBool()){
		// プラットフォームによっては子ウィンドウをここで作りなおさないと表示されない場合がある
		if (!KPanel.isNull()){
			KPanel->deleteLater();
		}
		KPanel = new KeyPanel(view);
		KPanel->show();
	}
}

void P6VXApp::setWindowIcon(const QIcon &icon)
{
	MWidget->setWindowIcon(icon);
}

void P6VXApp::layoutBitmap(HWINDOW Wh, int x, int y, double scaleX, double scaleY, QImage image)
{
	// QtではSceneRectの幅を返す
	QGraphicsView* view = reinterpret_cast<QGraphicsView*>(Wh);
	Q_ASSERT(view);
	QGraphicsScene* scene = view->scene();

	// 指定座標に生成済みのQPixmapItemが存在するかチェック
	// (同一座標にビットマップが重なることはないという前提)
	QGraphicsItem* item = nullptr;
	QGraphicsPixmapItem* pItem = nullptr;
	foreach(item, scene->items()){
		if(item->scenePos() == QPointF(x, y)){
			// QPixmapItemが見つかったら一旦sceneから除去する
			// (scene上にある状態で画像を更新すると真っ黒になる場合がある)
			pItem = dynamic_cast<QGraphicsPixmapItem*>(item);
			if(pItem) {
				scene->removeItem(pItem);
				break;
			}
		}
	}

	if(pItem == nullptr){
		pItem = new QGraphicsPixmapItem(nullptr);
		// フィルタリング
		if(Cfg->GetValue( CB_Filtering )){
			pItem->setTransformationMode(Qt::SmoothTransformation);
		}
	}
	// QPixmapItemの画像を更新してsceneに追加
	pItem->setPixmap(QPixmap::fromImage(image));
	scene->addItem(pItem);

	// アスペクト比に従って縦サイズを調整
	pItem->resetTransform();
	QTransform trans;
	trans.scale(scaleX, scaleY);
	trans.translate(x, y);
	pItem->setTransform(trans);
}

void P6VXApp::getWindowImage(HWINDOW Wh, QRect pos, void *pixels)
{
	QGraphicsView* view = reinterpret_cast<QGraphicsView*>(Wh);
	Q_ASSERT(view);
	QGraphicsScene* scene = view->scene();
	QImage image(pos.width(), pos.height(), QImage::Format_RGB888);

	QPainter painter(&image);
	scene->render(&painter, image.rect(), pos);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
	memcpy(pixels, image.bits(), image.sizeInBytes());
#else
	memcpy(pixels, image.bits(), image.byteCount());
#endif
}

void P6VXApp::raiseWidget()
{
#ifdef Q_OS_WINDOWS
	HWND hwnd = (HWND)MWidget->winId();

	// check if widget is minimized to Windows task bar
	if (::IsIconic(hwnd)) {
		::ShowWindow(hwnd, SW_RESTORE);
	}

	::SetForegroundWindow(hwnd);
#else
	MWidget->show();
	MWidget->raise();
	MWidget->activateWindow();
#endif
}

void P6VXApp::clearLayout(HWINDOW Wh)
{
	QGraphicsView* view = reinterpret_cast<QGraphicsView*>(Wh);
	Q_ASSERT(view);
	Q_ASSERT(view->scene());
	QGraphicsScene* scene = view->scene();
	scene->clear();

#ifndef NO_HWACCEL
	// ステータスバー非表示またはフルスクリーン、かつTILTモードが有効になっている場合、背景を描く
	if( (!Cfg->GetValue(CB_DispStatus)|| Cfg->GetValue(CB_FullScreen)) &&
		#ifndef NOMONITOR
			!(P6Core && P6Core->IsMonitor()) &&
		#endif
			isTiltEnabled()){
		// 画面に対する、枠を含めたサイズの比率
		qreal merginRatio = 1.0;
		QGraphicsPixmapItem* background = nullptr;
		switch(this->Cfg->GetValue(CV_Model)) {
		case 60:
		case 61:
			// 初代機の場合はPC-6042Kを使う
			background = new QGraphicsPixmapItem(QPixmap::fromImage(QImage(":/res/background60.png")));
			merginRatio = 1.45;
			break;
		default:
			// それ以外の場合はPC-60m43を使う
			background = new QGraphicsPixmapItem(QPixmap::fromImage(QImage(":/res/background.png")));
			merginRatio = 1.1;
		}
		background->setTransformationMode(Qt::SmoothTransformation);
		// 最前面に配置(他のアイテムのZ値はデフォルトの0)
		background->setZValue(1);
		QTransform trans;
		// 画像の拡大倍率
		qreal ratio = qMax(scene->width() / background->sceneBoundingRect().width(),
						   scene->height() / background->sceneBoundingRect().height());
		qreal scaleRatio = ratio * merginRatio;

		int scaledWidth = background->sceneBoundingRect().width() * scaleRatio;
		int scaledHeight = background->sceneBoundingRect().height() * scaleRatio;

		// 画像のオフセットを計算(枠の分だけ左上に移動)
		trans.translate(-(scaledWidth - scene->width()) / 2, -(scaledHeight - scene->height()) / 2);
		trans.scale(ratio * merginRatio, ratio * merginRatio);
		background->setTransform(trans);
		scene->addItem(background);
	}
#endif
}

void P6VXApp::showPopupMenu(int x, int y)
{
	// メニュー表示中に右クリックすると二重にメニューが表示されてしまうため、その対処
	if(MenuMutex.tryLock()){
		P6Core->Stop();
		P6Core->ShowPopupImpl(x, y);
		P6Core->Start();
		MenuMutex.unlock();
	}
}

void P6VXApp::toggleKeyPanel()
{
	if(KPanel->isVisible()){
		KPanel->close();
	} else {
		KPanel->show();
	}
}

void P6VXApp::toggleVirtualKeyboard()
{
	MWidget->toggleVirtualKeyboard();
}

void P6VXApp::resetSettings()
{
	if (OSD_Message(P6Core ? P6Core->GetWindowHandle() : nullptr, tr("本当に設定を初期化しますか?").toStdString(),
					APPNAME, OSDM_YESNO | OSDM_ICONWARNING) == OSDR_YES){
		if(OSD_Message(P6Core ? P6Core->GetWindowHandle() : nullptr, tr("設定を反映するには一度終了しますがよろしいですか?").toStdString(),
					   GetText(T_QUITC), OSDM_YESNO | OSDM_ICONWARNING) == OSDR_YES){

			P6VPATH path;
#ifdef AUTOSUSPEND
			// 自動ステートセーブファイルを削除
			OSD_AddPath( path, Cfg->GetValue(CF_DokoPath), ".0.dds" );
			QFile::remove(P6VPATH2QSTR(path));
			OSD_AddPath( path, Cfg->GetValue(CF_DokoPath), ".-1.dds" );
			QFile::remove(P6VPATH2QSTR(path));
#endif
			// P6V設定ファイルを削除
			OSD_AddPath( path, OSD_GetConfigPath(), FILE_CONFIG );
			QFile::remove(P6VPATH2QSTR(path));
			// P6VX設定ファイルを削除
			OSD_AddPath( path, OSD_GetConfigPath(), "pc6001vx.ini" );
			QFile::remove(P6VPATH2QSTR(path));
			// アプリを終了(終了時に設定を保存しないようにしてから)
			this->Cfg->SetValue(CB_SaveQuit, false);
			OSD_PushEvent(EV_QUIT);
		}
	}
}

bool P6VXApp::isTiltEnabled()
{
	QMutexLocker lock(&PropretyMutex);
	return TiltEnabled;
}

void P6VXApp::enableTilt(bool enable)
{
	QMutexLocker lock(&PropretyMutex);
	TiltEnabled = enable;
}

TiltDirection P6VXApp::getTiltDirection()
{
	QMutexLocker lock(&PropretyMutex);
	return TiltDir;
}

void P6VXApp::setTiltDirection(TiltDirection dir)
{
	QMutexLocker lock(&PropretyMutex);
	TiltDir = dir;
}

void P6VXApp::setTiltStep(int step)
{
	QMutexLocker lock(&PropretyMutex);
	TiltStep = step;
}

bool P6VXApp::isPlatform(const QString platform)
{
	return platformName() == platform;
}

bool P6VXApp::isAVI()
{
	return P6Core && P6Core->IsAVI();
}

int P6VXApp::getTiltStep()
{
	QMutexLocker lock(&PropretyMutex);
	return TiltStep;
}

bool P6VXApp::isSafeMode()
{
	QMutexLocker lock(&PropretyMutex);
	return SafeMode;
}

void P6VXApp::enableSafeMode(bool enable)
{
	QMutexLocker lock(&PropretyMutex);
	SafeMode = enable;
}

QString P6VXApp::getCustomRomPath()
{
	QMutexLocker lock(&PropretyMutex);
	return CustomRomPath;
}

void P6VXApp::setCustomRomPath(QString path)
{
	QMutexLocker lock(&PropretyMutex);
	CustomRomPath = path;
}

void P6VXApp::enableCompatibleRomMode(std::shared_ptr<CFG6>& config, bool enable)
{
	QMutexLocker lock(&PropretyMutex);
	// P6V本体の設定ファイルに対して互換ROMに必要な設定を行う。
	// この時点では設定ファイルに反映しない点に注意。
	if(enable){
		// 互換ROM使用時の設定
		auto model = config->GetValue(CV_Model);
		if (model != 60 && model != 61 && model != 62 && model != 66){
			config->SetValue(CV_Model, 60);
		}
		config->SetValue(CB_CheckCRC, false);
		config->SetValue(CF_RomPath, STR2P6VPATH(std::string(":/res/rom")));
	} else {
		config->SetValue(CB_CheckCRC, true);
		config->SetValue(CF_RomPath, STR2P6VPATH(std::string("")));
	}
}

void P6VXApp::exportSavedTape()
{
	// TAPE(SAVE)ファイル名を取得
	auto src = Cfg->GetValue(CF_Save);
	if (!OSD_FileExist(src)){
		OSD_Message(P6Core ? P6Core->GetWindowHandle() : nullptr, tr("TAPE(SAVE)ファイルが存在しません。").toStdString(),
					GetText(TERR_ERROR), OSDM_OK | OSDM_ICONERROR);
		return;
	}
//#ifdef Q_OS_ANDROID
#if 0 // 後で対応 #TODO
	// Androidの場合はインテントで他のアプリに送る
	ShareUtils util;
	int req = 0;
	bool altImpl = false;
	util.sendFile(QDir::cleanPath(P6VPATH2QSTR(src)), "TAPE(TAPE)", "application/octet-stream", req, altImpl);
#else
	// エクスポート先を指定
	P6VPATH dest = QSTR2P6VPATH(QDir::homePath());
	QFile savedTape(P6VPATH2QSTR(src));
	if(OSD_FileSelect( nullptr, FD_TapeSave, dest, src )){
		if (OSD_FileExist(dest)){
			if (OSD_Message(P6Core ? P6Core->GetWindowHandle() : nullptr,tr("ファイルはすでに存在しています。上書きしますか?").toStdString(),
							APPNAME, OSDM_OKCANCEL | OSDM_ICONQUESTION)
					== OSDR_OK){
				CFG6 cfg;
				Cfg->Write();
			}
		}
		savedTape.rename(P6VPATH2QSTR(dest));
	}
#endif

}

//仮想マシンを開始させる
void P6VXApp::executeEmulation()
{
	// カスタムROMパスが設定されている場合はそちらを使う
	if(getCustomRomPath() != ""){
		Cfg->SetValue(CF_RomPath, QSTR2P6VPATH(getCustomRomPath()));
	}

	// ROMファイル存在チェック&機種変更
	if( SerchRom( Cfg ) ){
		if( Error::GetError() != Error::NoError ){
			OSD_Message(P6Core ? P6Core->GetWindowHandle() : nullptr,
						Error::GetErrorText(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONWARNING );
			Error::SetError( Error::NoError );
		}
	}else{
		bool romFolderSpecified = false;
		if(OSD_Message(P6Core ? P6Core->GetWindowHandle() : nullptr,
					   tr("ROMファイルが見つかりません。\n"
								  "ROMフォルダ(%1)にROMファイルをコピーするか、"
								  "別のROMフォルダを指定してください。\n"
								  "別のROMフォルダを指定しますか?").arg(P6VPATH2QSTR(Cfg->GetValue(CF_RomPath))).toStdString(),
					   GetText(TERR_ERROR), OSDM_YESNO | OSDM_ICONWARNING ) == OSDR_YES){
			// ROMフォルダ再設定
			P6VPATH folder = Cfg->GetValue(CF_RomPath);
			OSD_AddDelimiter(folder);
			OSD_FolderDiaog(MWidget, folder);
			OSD_DelDelimiter(folder);

			if(!folder.empty()){
				Cfg->SetValue(CF_RomPath, folder);
				Cfg->Write();
				Restart = EL6::Restart;
				romFolderSpecified = true;
			}
		}
		if (!romFolderSpecified){
			// 互換ROMを使用するか問い合わせる
			int ret = OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr,
								   tr("エミュレーター内蔵の互換ROMを使用しますか?").toStdString(),
								   APPNAME, OSDM_YESNO | OSDM_ICONQUESTION );
			if(ret == OSDR_YES) {
				enableCompatibleRomMode(Cfg, true);
				Cfg->Write();
				Restart = EL6::Restart;
			} else {
				terminateEmulation();
				exit();
				return;
			}
		}
		emit vmRestart();
		return;
	}

	// 機種別P6オブジェクト確保
	std::unique_ptr<QtEL6> P6CoreObj(new QtEL6(this));
	if( !P6CoreObj ){
		exit();
		return;
	}

	// VM初期化
	if( !P6CoreObj->Init( Cfg ) ){
		if(Error::GetError() == Error::RomCrcNG){
			// CRCが合わない場合
			int ret = OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr,
								   tr("ROMイメージのCRCが不正です。\n"
									  "CRCが一致しないROMを使用すると、予期せぬ不具合を引き起こす可能性があります。\n"
									  "それでも起動しますか?").toStdString(),
								   GetText(TERR_ERROR), OSDM_YESNO | OSDM_ICONWARNING );
			if(ret == OSDR_YES) {
				Cfg->SetValue(CB_CheckCRC, false);
				Cfg->Write();
				emit vmRestart();
				return;
			} else {
				terminateEmulation();
				exit();
				return;
			}
		}else if(Error::GetError() == Error::NoRom){
			// ROMの一部が見つからない場合
			int ret = OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr,
								   tr("ROMファイルの一部が見つかりません。\n"
									  "エミュレーター内蔵の互換ROMを使用しますか?").toStdString(),
								   GetText(TERR_ERROR), OSDM_YESNO | OSDM_ICONWARNING );
			if(ret == OSDR_YES) {
				enableCompatibleRomMode(Cfg, true);
				Cfg->Write();
				emit vmRestart();
				return;
			} else {
				terminateEmulation();
				exit();
				return;
			}
		}else{
			OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr,
						 Error::GetErrorText(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONERROR );
			exit();
			return;
		}
	}

	// スタートアップ時にどこでもSAVEファイルから復元するオプション
	auto loadState = property("loadstate");
	if (loadState.isValid()){
		auto loadStatePath = QSTR2P6VPATH(loadState.toString());
		if (!OSD_FileExist(loadStatePath)){
			Error::SetError(Error::DokoReadFailed);
			OSD_Message( P6CoreObj ? P6CoreObj->GetWindowHandle() : nullptr,
						Error::GetErrorText(), GetText( TERR_ERROR ), OSDR_OK | OSDM_ICONERROR );
			Error::Clear();
		}
		Cfg->SetDokoFile(loadStatePath);
		Restart = EL6::Dokoload;
		// スタートアップファイル名をリセット
		setProperty("loadstate", QVariant());
	}

	switch( Restart ){
	case EL6::Quit:	// 通常起動
#ifdef AUTOSUSPEND
		// 自動サスペンド有効時はここでLOAD
		P6CoreObj->UI_DokoLoad(0);
		[[fallthrough]];
#else
		break;
#endif
	case EL6::Dokoload:	// どこでもLOAD
		if( OSD_FileExist( Cfg->GetDokoFile() ) && !P6CoreObj->DokoDemoLoad( Cfg->GetDokoFile() ) ){
#ifdef AUTOSUSPEND
			// ロードに失敗した場合、フォールバックスロット(-1)からロード
			P6CoreObj->UI_DokoLoad(-1);
#else
			// 失敗した場合
			OSD_Message( P6CoreObj ? P6CoreObj->GetWindowHandle() : nullptr,
						 Error::GetErrorText(), GetText( TERR_ERROR ), OSDR_OK | OSDM_ICONERROR );
			Error::Clear();
#endif
		}else{
#ifdef AUTOSUSPEND
			// ロードに成功した場合(またはセーブファイルがない場合)、フォールバック用スロット(-1)に保存
			P6CoreObj->UI_DokoSave(-1);
#endif
		}
		Cfg->SetDokoFile( "" );
		break;

	case EL6::ReplayPlay:	// リプレイ再生
	case EL6::ReplayResume:	// リプレイ保存再開
	case EL6::ReplayMovie:	// リプレイを動画に変換
		if( !P6CoreObj->DokoDemoLoad( Cfg->GetDokoFile() ) ){
			// 失敗した場合
			OSD_Message( P6CoreObj ? P6CoreObj->GetWindowHandle() : nullptr,
						 Error::GetErrorText(), GetText( TERR_ERROR ), OSDR_OK | OSDM_ICONERROR );
			Error::Clear();
			Cfg->SetDokoFile( "" );
		}
		break;

	default:
		break;
	}

	// 前回実行時のエミュレーションコアオブジェクトを破棄、入れ替え
	if (!KeyWatcher.isNull()){
		KeyWatcher->stop();
		KeyWatcher->deleteLater();
	}
	if (!P6Core.isNull()){
		P6Core->Stop();
		P6Core->deleteLater();
	}
	P6Core = P6CoreObj.release();

	// パレット設定
	P6Core->SetPaletteTable(PaletteTable, Cfg->GetValue( CV_ScanLineBr ));

	// キーボード状態監視
	KeyWatcher = new KeyStateWatcher(P6Core->GetKeyboard(), P6Core->GetJoystick(), this);
	MWidget->setKeyStateWatcher(KeyWatcher);

	// 以降、ウィンドウが閉じたらアプリを終了する
	connect(this, SIGNAL(lastWindowClosed()), this, SLOT(terminateEmulation()));

	// VM実行
	Adaptor->setEmulationObj(P6Core);
	emit vmPrepared(Restart);
	P6Core->Start();
}

//仮想マシン終了後の処理
void P6VXApp::postExecuteEmulation()
{
	Restart = Adaptor->getReturnCode();
	Adaptor->setEmulationObj(nullptr);

	// エミュレーションコアオブジェクトを破棄
	if (!KeyWatcher.isNull()){
		KeyWatcher->stop();
		KeyWatcher->deleteLater();
	}
	if(!P6Core.isNull()){
		P6Core->Stop();
#ifdef AUTOSUSPEND
		// 自動サスペンド有効時はここでSAVE
		if( Restart == EL6::Quit ){
			P6Core->UI_DokoSave(0);
		}
#endif
		P6Core->deleteLater();
	}

	// 再起動ならばINIファイル再読込み
	if( Restart == EL6::Restart ){
		if( !Cfg->Init() ){
			Error::SetError( Error::IniDefault );
			OSD_Message( P6Core ? P6Core->GetWindowHandle() : nullptr,
						 Error::GetErrorText(), GetText(TERR_ERROR), OSDM_OK | OSDM_ICONWARNING );
			Error::SetError( Error::NoError );
		}
	}

	if( Restart == EL6::Quit ){
		// 終了処理
		if (Cfg->GetValue(CB_SaveQuit)){
			Cfg->Write();
		}
		if(MWidget && MWidget->isVisible()){
			// ウィンドウを閉じてサイズを保存
			MWidget->close();
		}
		exit();
		return;
	} else {
		emit vmRestart();
	}
}

void P6VXApp::terminateEmulation()
{
	OSD_PushEvent( EV_QUIT );
}



void P6VXApp::handleSpecialKeys(QKeyEvent* ke, int& keyCode, Qt::KeyboardModifiers &modifiers)
{
	quint32 nativeKey = ke->nativeScanCode();
#if 0
	qDebug("keytext %s\n", ke->text().toStdString().c_str());
	qDebug("keycode 0x%x\n", keyCode);
	qDebug("nativekeycode %d\n", nativeKey);
#endif

	// X11の場合
	if (isPlatform("xcb")){
		// 「ろ」
		if(keyCode == Qt::Key_Backslash){
			keyCode = nativeKey == 97 ? Qt::Key_Underscore : Qt::Key_Backslash;
		}
	}
	// Windowsの場合
	else if (isPlatform("windows")){
		// 「ろ」
		if(keyCode == Qt::Key_Backslash){
			keyCode = nativeKey == 115 ? Qt::Key_Underscore : Qt::Key_Backslash;
		}
		// カタカナ/ひらがな
		else if(nativeKey == 112){
			keyCode = Qt::Key_Hiragana_Katakana;
		}
		// 変換
		else if(nativeKey == 121){
			keyCode = Qt::Key_Henkan;
		}
		// 無変換
		else if(nativeKey == 123){
			keyCode = Qt::Key_Muhenkan;
		}
	// Macの場合
	}else if( isPlatform("cocoa")) {
		if(keyCode == Qt::Key_yen) {		// Yen key
			keyCode = Qt::Key_Backslash;
		}
	}
}

void P6VXApp::finishKeyEvent(Event &ev)
{
#if 0
	qDebug() << ev.key.mod
			 << ev.key.sym
			 << ev.key.type
			 << QChar(ev.key.unicode);
#endif

	// イベント時点でのモディファイヤキー状態
	bool shiftStatus = (ev.key.mod & KVM_SHIFT) ? true : false;
	bool ctrlStatus = (ev.key.mod & KVM_CTRL) ? true : false;

	static const Event shiftDownEvent =
		{
			.key = { EV_KEYDOWN, true, KVC_LSHIFT, KVM_LSHIFT, 0 }
		};
	static const Event shiftUpEvent =
		{
			.key = { EV_KEYUP, false, KVC_LSHIFT, KVM_LSHIFT, 0 }
		};
	static const Event ctrlDownEvent =
		{
			.key = { EV_KEYDOWN, true, KVC_LCTRL, KVM_LCTRL, 0 }
		};
	static const Event ctrlUpEvent =
		{
			.key = { EV_KEYUP, false, KVC_LCTRL, KVM_LCTRL, 0 }
		};

	// 特殊ファンクションキー(P6VX独自アサイン)
	// 英語キーボード、省スペースキーボード向けに
	// ctrl+ファンクションキーで特殊キー入力をサポート
	if (ctrlStatus){
		bool specialFuncKey = true;
		switch (ev.key.sym) {
		// PAGEキー(Ctrl+F8)
		case KVC_F8:
			ev.key.sym = KVC_PAGEUP;		break;
		// STOPキー(Ctrl+F9)
		case KVC_F9:
			ev.key.sym = KVC_END;			break;
		// MODEキー(Ctrl+F10)
		case KVC_F10:
			ev.key.sym = KVC_PAGEDOWN;		break;
		// CAPSキー(Ctrl+F11)
		case KVC_F11:
			ev.key.sym = KVC_SCROLLLOCK;	break;
		// かなキー(Ctrl+F12)
		case KVC_F12:
			ev.key.sym = KVC_PAUSE;			break;
		default:
			specialFuncKey = false;
		}
		if (specialFuncKey) {
			// 特殊ファンクションキーに該当した場合は
			// 一旦ctrlキーを離すイベントを送る
			OSD_PushEvent(ctrlUpEvent);
			// 変換後のキーはctrl無しとして送る
			ev.key.mod = (PCKEYmod)(ev.key.mod & ~KVM_CTRL);
			OSD_PushEvent(ev);
			return;
		}
	}

	// 英語キーボードの場合、@の入力にSHIFTキーが必要なため、
	// この場合P6エミュ側にSHIFT+@のキーコードを送っても何も入力されない。
	// 本来意図した文字が入力されるよう、
	// キーシンボルとモディファイヤキーを修正する。
	struct FixInfo
	{
		PCKEYsym sym;
		bool shift;
	};
#define QUNI(a) QChar(a).unicode()
	static const std::map<uint16_t, FixInfo> fixMap = {
		{	QUNI('-'),	{ KVC_MINUS,		false}	},
		{	QUNI('^'),	{ KVC_CARET,		false}	},
		{	QUNI('@'),	{ KVC_AT,			false}	},
		{	QUNI('['),	{ KVC_LBRACKET,		false}	},
		{	QUNI(']'),	{ KVC_RBRACKET,		false}	},
		{	QUNI(';'),	{ KVC_SEMICOLON,	false}	},
		{	QUNI(':'),	{ KVC_COLON,		false}	},
		{	QUNI(','),	{ KVC_COMMA,		false}	},
		{	QUNI('.'),	{ KVC_PERIOD,		false}	},
		{	QUNI('/'),	{ KVC_SLASH,		false}	},
		{	QUNI('!'),	{ KVC_1,			true}	},
		{	QUNI('\"'),	{ KVC_2,			true}	},
		{	QUNI('#'),	{ KVC_3,			true}	},
		{	QUNI('$'),	{ KVC_4,			true}	},
		{	QUNI('%'),	{ KVC_5,			true}	},
		{	QUNI('&'),	{ KVC_6,			true}	},
		{	QUNI('\''),	{ KVC_7,			true}	},
		{	QUNI('('),	{ KVC_8,			true}	},
		{	QUNI(')'),	{ KVC_9,			true}	},
		{	QUNI('='),	{ KVC_MINUS,		true}	},
		{	QUNI('+'),	{ KVC_SEMICOLON,	true}	},
		{	QUNI('*'),	{ KVC_COLON,		true}	},
		{	QUNI('_'),	{ KVC_UNDERSCORE,	true}	},
		{	QUNI('<'),	{ KVC_COMMA,		true}	},
		{	QUNI('>'),	{ KVC_PERIOD,		true}	},
		{	QUNI('?'),	{ KVC_SLASH,		true}	},
	};
#undef QUNI

	// かな、グラフィックキー有効時はキーイベントをそのまま送信
	auto mod = KeyWatcher->GetModifierStatus();
	if ( mod & KeyStateWatcher::KANA ||
		mod & KeyStateWatcher::KKANA ||
		mod & KeyStateWatcher::GRAPH ||
		mod & KeyStateWatcher::KANA ) {
		OSD_PushEvent(ev);
		return;
	}

	if (fixMap.count(ev.key.unicode)){
		auto i = fixMap.at(ev.key.unicode);
		ev.key.sym = i.sym;
		if (i.shift){
			// P6でSHIFTが必要な記号はSHIFTモディファイヤーを付与する
			OSD_PushEvent(shiftDownEvent);	// SHIFTキーを押すイベントを挿入する
			// キーイベント本体にSHIFTモディファイヤーを付与
			ev.key.mod = (PCKEYmod)(ev.key.mod|KVM_SHIFT);
			// キーイベントを送信
			OSD_PushEvent(ev);
			// キーを離した後、物理SHIFTキーの状態が離されていたならば
			// SHIFTキーを離すイベントをキューに入れる(キーリピート中に離さないようにイベント優先度を下げる)
			if(!ev.key.state && !shiftStatus) {
				postEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Shift,
											  Qt::NoModifier), Qt::LowEventPriority);
			}
		} else {
			// P6でSHIFTなしで入力できる記号はSHIFTモディファイヤーを外す
			// SHIFTキーを離すイベントを挿入する
			OSD_PushEvent(shiftUpEvent);
			// キーイベント本体にSHIFTモディファイヤーを付与
			ev.key.mod = (PCKEYmod)(ev.key.mod&~KVM_SHIFT);
			// キーイベントを送信
			OSD_PushEvent(ev);
			// キーを離した後、物理SHIFTキーの状態が押されていたならば
			// SHIFTキーを押すイベントをキューに入れる(キーリピート中に押さないようにイベント優先度を下げる)
			if(!ev.key.state && shiftStatus) {
				postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Shift,
											  Qt::NoModifier), Qt::LowEventPriority);
			}
		}
	} else {
		// 修正対象に該当しなかったキーイベントはそのまま送信
		OSD_PushEvent(ev);
	}
}

void P6VXApp::overrideSettings(std::shared_ptr<CFG6>& cfg)
{
#ifdef Q_OS_ANDROID
	P6VPATH str;
	// 設定のデフォルト値でなく、プラットフォームごとのデータフォルダを探す。
	// Androidではこの方法でないとSDカードが検出できない場合がある。
	std::string dataPath = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)[0].toLocal8Bit().data();

	// ROM,TAPE,DISK,拡張ROM,WAVEパスの初期値にはこのデータフォルダを設定する。
	if(P6VPATH2QSTR(cfg->GetValue(CF_RomPath)).contains(P6VPATH2QSTR(OSD_GetConfigPath()))) cfg->SetValue(CF_RomPath, STR2P6VPATH(dataPath));
	if(P6VPATH2QSTR(cfg->GetValue(CF_TapePath)).contains(P6VPATH2QSTR(OSD_GetConfigPath()))) cfg->SetValue(CF_TapePath, STR2P6VPATH(dataPath));
	if(P6VPATH2QSTR(cfg->GetValue(CF_DiskPath)).contains(P6VPATH2QSTR(OSD_GetConfigPath()))) cfg->SetValue(CF_DiskPath, STR2P6VPATH(dataPath));
	if(P6VPATH2QSTR(cfg->GetValue(CF_ExtRomPath)).contains(P6VPATH2QSTR(OSD_GetConfigPath()))) cfg->SetValue(CF_ExtRomPath, STR2P6VPATH(dataPath));
	if(P6VPATH2QSTR(cfg->GetValue(CF_WavePath)).contains(P6VPATH2QSTR(OSD_GetConfigPath()))) cfg->SetValue(CF_WavePath, STR2P6VPATH(dataPath));

	// AndroidではSDカードにファイルを書き出せないため、
	// SnapshotおよびどこでもSAVEはアプリ内のサンドボックス領域に固定する。
	OSD_AddPath( str, OSD_GetConfigPath(), DIR_IMAGE );
	cfg->SetValue(CF_ImgPath,  str );
	OSD_AddPath( str, OSD_GetConfigPath(), DIR_DOKO );
	cfg->SetValue(CF_DokoPath,  str );
	cfg->Write();

	// Androidでは起動時にスナップショット画像を削除する
	auto imgDir = QDir(P6VPATH2QSTR(cfg->GetValue(CF_ImgPath)));
	imgDir.setNameFilters(QStringList() << "*.png");
	imgDir.setFilter(QDir::Files);
	foreach(QString dirFile, imgDir.entryList())
	{
		imgDir.remove(dirFile);
	}
#endif

	// サンプリングレートは44100に固定
	cfg->SetValue(CV_SampleRate, 44100);
}

QFileDialog *P6VXApp::createFileDialog(void *hwnd)
{
	QWidget* parent = reinterpret_cast<QWidget*>(hwnd);
	auto dialog = new QFileDialog(parent);
	// GTKスタイル使用時にファイル選択ダイアログがフリーズする対策
	// https://bugreports.qt.io/browse/QTBUG-77214
	if (isPlatform("xcb")){
		dialog->setOptions(QFileDialog::DontUseNativeDialog);
	}
	return dialog;
}

bool P6VXApp::notify ( QObject * receiver, QEvent * event )
{
	Event ev;
	ev.type = EV_NOEVENT;

	switch(event->type()){
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	{
		QKeyEvent* ke = dynamic_cast<QKeyEvent*>(event);
		Q_ASSERT(ke);

		// キーコードを特定
		int keyCode = Qt::Key_unknown;
		Qt::KeyboardModifiers modifiers = ke->modifiers();
#ifdef IRREGULAR_KEYBOARD
		// UMPC内蔵小型キーボードなど、特殊なキーボードの場合
		// 一般的なキーボードとコードのアンマッチがあるので
		// 表示可能文字についてはtextを参照したほうが信頼できる
		if(!ke->text().isEmpty() && ke->text().at(0).isPrint()){
			keyCode = ke->text().at(0).toUpper().unicode();
		} else {
			keyCode = ke->key();
		}
#else
		keyCode = ke->key();
#endif
		// 特殊キー対策
		handleSpecialKeys(ke, keyCode, modifiers);

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
		// ・エミュレータウィンドウ、キーパレット以外のウィンドウ
		// (Aboutダイアログ、環境設定ダイアログ)が最前面にある場合
		QString activeWindowClass = activeWindow() ? activeWindow()->metaObject()->className() : "";
		if(activeWindowClass != "MainWidget" && activeWindowClass != "KeyPanel"){
			processKeyEventInQt = true;
		}
		// メニュー表示中の場合
		if(!MenuMutex.tryLock()){
			processKeyEventInQt = true;
		} else {
			MenuMutex.unlock();
		}
		if(processKeyEventInQt){
			return ParentAppClass::notify(receiver, event);
		}


		ev.type        = event->type() == QEvent::KeyPress ? EV_KEYDOWN : EV_KEYUP;
		ev.key.state   = event->type() == QEvent::KeyPress ? true : false;
		ev.key.sym     = OSD_ConvertKeyCode( keyCode );
		ev.key.mod	   = (PCKEYmod)(
					( modifiers & Qt::ShiftModifier ? KVM_SHIFT : KVM_NONE )
					| ( modifiers & Qt::ControlModifier ? KVM_CTRL : KVM_NONE )
					| ( modifiers & Qt::AltModifier ? KVM_ALT : KVM_NONE )
					| ( modifiers & Qt::MetaModifier ? KVM_CTRL : KVM_NONE )
					| ( modifiers & Qt::KeypadModifier ? KVM_NUM : KVM_NONE )
					// CAPSLOCKは検出できない？
					// | ( modifiers & Qt::caps ? KVM_NUM : KVM_NONE )
					);
		ev.key.unicode = ke->text().utf16()[0];
		finishKeyEvent(ev);
		break;
	}
	case QEvent::UpdateRequest:
	{
		// 画面更新に合わせてジョイスティックをポーリングするための疑似イベントを発行
		OSD_PushEvent(EV_JOYAXISMOTION);
		break;
	}
	case QEvent::ApplicationStateChange:
		if(P6Core){
			auto state = reinterpret_cast<QApplicationStateChangeEvent*>(event)->applicationState();
			switch (state){
			case Qt::ApplicationActive:
				P6Core->Start();
				break;
			default:;
#ifdef ALWAYSFULLSCREEN
				P6Core->Stop();
#endif
#ifdef AUTOSUSPEND
				// 自動サスペンド有効時はここでSAVE
				P6Core->UI_DokoSave(0);
#endif
			}
		}
		break;
	default:;
	}

	if(ev.type == EV_NOEVENT){
		return ParentAppClass::notify(receiver, event);
	} else {
		return true;
	}
}

bool P6VXApp::hasSetting(const QString &key)
{
	QMutexLocker lock(&SettingMutex);
	return Setting.contains(key);
}

const QVariant P6VXApp::getSetting(const QString &key)
{
	QMutexLocker lock(&SettingMutex);
	return Setting.value(key);
}

void P6VXApp::setSetting(const QString &key, const QVariant &value)
{
	QMutexLocker lock(&SettingMutex);
	if(Setting.value(key) != value){
		Setting.setValue(key, value);
	}
}

void P6VXApp::setDefaultSetting(const QString &key, const QVariant &value)
{
	QMutexLocker lock(&SettingMutex);
	if(!Setting.contains(key)){
		Setting.setValue(key, value);
	}
}

#ifdef USE_X11
#include <X11/Xlib.h>
#endif
void P6VXApp::inhibitScreenSaver()
{
#if defined USE_X11
	if(MWidget && MWidget->isFullScreen()){
		if (auto *x11App = qApp->nativeInterface<QNativeInterface::QX11Application>()){
			XResetScreenSaver(x11App->display());
		}
	}
#elif defined Q_OS_WIN
	if(MWidget && MWidget->isFullScreen()){
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
	} else {
		SetThreadExecutionState(ES_CONTINUOUS);
	}
#else
	// 何もしない
#endif
}

void P6VXApp::showMouseCursor()
{
	setOverrideCursor(Qt::ArrowCursor);
}

void P6VXApp::hideMouseCursor()
{
	setOverrideCursor(Qt::BlankCursor);
}

