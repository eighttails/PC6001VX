#include <QtWidgets>
#include <QSettings>
#include <QTapGesture>
#include <QScopedPointer>
#include <QQmlError>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSGRendererInterface>

#include "../osd.h"
#include "rendercanvas.h"
#include "renderview.h"
#include "p6vxapp.h"

namespace {
const char *graphicsApiName(QSGRendererInterface::GraphicsApi api)
{
	struct GraphicsApiName {
		QSGRendererInterface::GraphicsApi api;
		const char *name;
	};
	static constexpr GraphicsApiName names[] = {
		{ QSGRendererInterface::Unknown, "Unknown" },
		{ QSGRendererInterface::Software, "Software" },
		{ QSGRendererInterface::OpenGL, "OpenGL" },
		{ QSGRendererInterface::Direct3D11, "Direct3D 11" },
		{ QSGRendererInterface::Vulkan, "Vulkan" },
		{ QSGRendererInterface::Metal, "Metal" },
		{ QSGRendererInterface::Null, "Null" },
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
		{ QSGRendererInterface::Direct3D12, "Direct3D 12" },
#endif
	};

	for (const auto &entry : names) {
		if (entry.api == api) {
			return entry.name;
		}
	}

	return "Unknown";
}
}

RenderView::RenderView(QWidget *parent)
	: QQuickWidget(parent)
	, Canvas(nullptr)
{
	static bool qmlTypeRegistered = false;
	if (!qmlTypeRegistered) {
		qmlRegisterType<RenderCanvas>("PC6001VX", 1, 0, "RenderCanvas");
		qmlTypeRegistered = true;
	}

	setSizeIncrement(1, 1);
	setClearColor(Qt::black);
	setResizeMode(QQuickWidget::SizeRootObjectToView);
	setSource(QUrl(QStringLiteral("qrc:/qml/RenderScene.qml")));
	if (status() == QQuickWidget::Error) {
		for (const QQmlError &error : errors()) {
			qWarning() << error;
		}
	}
	connect(quickWindow(), &QQuickWindow::sceneGraphInitialized, this, [this] {
		qInfo() << "Qt Quick graphics API:"
				<< graphicsApiName(quickWindow()->rendererInterface()->graphicsApi());
	}, Qt::SingleShotConnection);

	if (!canvas()) {
		qWarning() << "RenderCanvas was not created.";
	}

	grabGesture(Qt::TapGesture);
	setAttribute(Qt::WA_Hover);
	setAcceptDrops(true);
}

RenderView::~RenderView()
{
}

void RenderView::fitContent()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	if (!app) return;

	updateRootProperties();
}

void RenderView::resizeWindowByRatio(int ratio)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	qreal r = double(ratio) / 100;
	// 最大化、フルスクリーン中は倍率固定モードにする。
	if ((windowState() & Qt::WindowMaximized) || (windowState() & Qt::WindowFullScreen)) {
		app->setSetting(P6VXApp::keyFixMagnification, true);
		app->setSetting(P6VXApp::keyMagnification, r);
		return;
	} else {
		app->setSetting(P6VXApp::keyFixMagnification, false);
	}
	setGeometry(x(), y(), SceneSize.width() * r, SceneSize.height() * r);

	emit resized(size());
}

void RenderView::initializeSize()
{
#ifdef Q_OS_ANDROID
	// Androidでは親ウィンドウの全画面領域をレイアウトに任せる
	return;
#else
	// Geometry設定がなかったらSceneRectサイズに合わせてリサイズ
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	if (!app->hasSetting(P6VXApp::keyGeometry))	{
		resizeWindowByRatio(int(app->getSetting(P6VXApp::keyMagnification).toReal() * 100));
	}
#endif
}

int RenderView::sceneWidth() const
{
	return SceneSize.width();
}

int RenderView::sceneHeight() const
{
	return SceneSize.height();
}

void RenderView::setSceneSize(int width, int height)
{
	QSize size(width, height);
	if (SceneSize == size) return;

	SceneSize = size;
	updateRootProperties();
	initializeSize();
}

RenderCanvas *RenderView::canvas() const
{
	// QMLオブジェクトの探索はメインスレッドでのみ行う
	if (Canvas.isNull() && QThread::currentThread() == thread()) {
		QQuickItem *root = rootObject();
		Canvas = root ? root->findChild<RenderCanvas*>(QStringLiteral("renderCanvas")) : nullptr;
	}
	return Canvas.data();
}

void RenderView::layoutBitmap(int x, int y, double scaleX, double scaleY, const QImage &image, bool smooth, qreal z)
{
	if (RenderCanvas *c = canvas()) {
		c->addOrUpdateLayer(x, y, scaleX, scaleY, image, smooth, z);
	}
}

void RenderView::clearLayout()
{
	if (RenderCanvas *c = canvas()) {
		c->clearLayers();
	}
}

bool RenderView::isFilteringAt(int x, int y) const
{
	RenderCanvas *c = canvas();
	return c && c->isFilteringAt(x, y);
}

QImage RenderView::renderSceneImage(const QRect &rect) const
{
	if (RenderCanvas *c = canvas()) {
		return c->renderToImage(rect);
	}

	QImage image(rect.size(), QImage::Format_RGB888);
	image.fill(Qt::black);
	return image;
}

bool RenderView::event(QEvent *event)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	switch (event->type()) {
	case QEvent::TouchBegin:
		if(QTouchEvent* tEvent = dynamic_cast<QTouchEvent*>(event)){
			auto state = tEvent->points()[0].state();
			if (state == QEventPoint::Pressed){
				auto point = tEvent->points()[0].globalPosition().toPoint();
				Event e;
				e.type = EV_MOUSEBUTTONUP;
				e.mousebt.button = MBT_RIGHT;
				e.mousebt.x = point.x();
				e.mousebt.y = point.y();
				OSD_PushEvent(e);
				return true;
			}
		}
		break;
	case QEvent::Enter:
	case QEvent::HoverMove:
		OSD_ShowCursor(true);
		break;
	case QEvent::Leave:
		OSD_ShowCursor(true);
		break;
	default:;
	}

	return QQuickWidget::event(event);
}

void RenderView::paintEvent(QPaintEvent *event)
{
	fitContent();
	updateRootProperties();
	QQuickWidget::paintEvent(event);
}

void RenderView::contextMenuEvent(QContextMenuEvent *event)
{
	Event ev;
	ev.type = EV_MOUSEBUTTONUP;
	ev.mousebt.button = MBT_RIGHT;
	auto p = event->globalPos();
	ev.mousebt.x = p.x();
	ev.mousebt.y = p.y();
	event->accept();
	OSD_PushEvent(ev);
}

void RenderView::wheelEvent(QWheelEvent *event)
{
	Event ev;
	ev.type = EV_MOUSEWHEEL;
	ev.mousewh.y = event->angleDelta().y();
	event->accept();
	OSD_PushEvent(ev);
}

void RenderView::mouseReleaseEvent(QMouseEvent *event)
{
	Event ev;
	ev.type = EV_MOUSEBUTTONUP;
	auto button = event->button();
	switch (button) {
	case Qt::RightButton:
		return;
		// contextMenuEventと重複するため無視する
		// ev.mousebt.button = MBT_RIGHT;	break;
	// P6Vの左クリック→動作を等速に戻すの操作を中クリックにするためにボタンを交換
	case Qt::LeftButton:
		ev.mousebt.button = MBT_MIDDLE;	break;
	case Qt::MiddleButton:
		ev.mousebt.button = MBT_LEFT;	break;
	default:
		return;
	}

	auto p = event->globalPosition();
	ev.mousebt.x = p.x();
	ev.mousebt.y = p.y();
	ev.mousebt.state = false;
	event->accept();
	OSD_PushEvent(ev);
}

void RenderView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton){
		// ALT+F6(フルスクリーン切り替え)を送信
		Event ev;
		ev.type = EV_KEYDOWN;
		ev.key.sym = KVC_F6;
		ev.key.mod = KVM_LALT;
		event->accept();
		OSD_PushEvent(ev);
	}
}

void RenderView::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls()){
		event->acceptProposedAction();
	}
}

void RenderView::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
}

void RenderView::dropEvent(QDropEvent *event)
{
	auto mime = event->mimeData();
	if(mime->hasUrls()){
		foreach(QUrl url, mime->urls()){
			Event ev;
			ev.type = EV_DROPFILE;
			auto filename = url.toLocalFile().toStdString();
			char *data = new char[filename.length()+1];
			strcpy(data, filename.c_str());
			ev.drop.file = data;
			OSD_PushEvent(ev);
		}
	}
}

void RenderView::updateRootProperties()
{
	QQuickItem *root = rootObject();
	if (!root) return;

	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	root->setProperty("sceneWidth", SceneSize.width());
	root->setProperty("sceneHeight", SceneSize.height());
	root->setProperty("fixMagnification", app ? app->getSetting(P6VXApp::keyFixMagnification).toBool() : false);
	root->setProperty("magnification", app ? app->getSetting(P6VXApp::keyMagnification).toReal() : 1.0);
	root->setProperty("tiltAngle", (isActiveWindow() && app && app->isTiltEnabled()) ? 0.5 * app->getTiltStep() : 0.0);
}
