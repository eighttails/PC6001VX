#ifndef NOOPENGL
#include <QtOpenGL>
#endif
#include <QtWidgets>
#include <QSettings>
#include <QTapGesture>
#include <QScopedPointer>

#include "../osd.h"
#include "renderview.h"
#include "p6vxapp.h"


RenderView::RenderView(QGraphicsScene* scene, QWidget *parent)
	: QGraphicsView(scene, parent)
{
	setSizeIncrement(1, 1);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setBackgroundBrush(Qt::black);
	setStyleSheet( "QGraphicsView { border-style: none; }" );

	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
#ifndef NOOPENGL
	if(!app->isSafeMode() &&
			app->getSetting(P6VXApp::keyHwAccel).toBool()){
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)) && !defined (USE_QGLWIDGET)
		QScopedPointer<QOpenGLWidget> glw(new QOpenGLWidget(this));
#else
		// 描画に不具合が見られるプラットフォームでは当面QGLWidgetを使う。
		QScopedPointer<QGLWidget> glw(new QGLWidget(this));
#endif
		// QGraphicsViewに使うにはOpenGL2以上が必要
		if(glw->format().majorVersion() >= 2){
			setViewport(glw.take());
			setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
		}
	}
#endif
	grabGesture(Qt::TapGesture);
	setAttribute(Qt::WA_Hover);
	setAcceptDrops(true);

	// 初回起動時にシーングラフが構築されたらウィンドウサイズを初期化
	connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(initializeSize()));
}

RenderView::~RenderView()
{
}

void RenderView::fitContent()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	const bool fixMag = app->getSetting(P6VXApp::keyFixMagnification).toBool();
	// ウィンドウ全体に表示されるように表示倍率を調整
	qreal scaleRatio = fixMag
			? app->getSetting(P6VXApp::keyMagnification).toReal()
			: qMin(width() / scene()->width(), height() / scene()->height());
	resetTransform();
	centerOn(sceneRect().center());
	scale(scaleRatio, scaleRatio);

	// 表示倍率固定の場合は中心に配置
	if (fixMag){
		translate((width() - scene()->width()) / 2, (height() - scene()->height()) / 2);
	}
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
	setGeometry(x(), y(), scene()->width() * r, scene()->height() * r);

	emit resized(size());
}

void RenderView::initializeSize()
{
	// Geometry設定がなかったらSceneRectサイズに合わせてリサイズ
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	if (!app->hasSetting(P6VXApp::keyGeometry))	{
		resizeWindowByRatio(int(app->getSetting(P6VXApp::keyMagnification).toReal() * 100));
	}
}

bool RenderView::event(QEvent *event)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	switch (event->type()) {
	case QEvent::TouchBegin:
		if(QTouchEvent* tEvent = dynamic_cast<QTouchEvent*>(event)){
			auto state = tEvent->touchPoints()[0].state();
			if (state == Qt::TouchPointPressed){
				auto point = tEvent->touchPoints()[0].pos().toPoint();
				QGraphicsItem* item = scene()->itemAt(mapToScene(point), transform());
				// タップしたアイテムが原点にある場合、メイン画面とみなしてメニューを出す
				if(item && item->pos() == QPoint(0, 0)){
					Event e;
					e.type = EV_MOUSEBUTTONUP;
					e.mousebt.button = MBT_RIGHT;
					e.mousebt.x = point.x();
					e.mousebt.y = point.y();
					OSD_PushEvent(e);
					return true;
				}
			}
		}
		break;
	case QEvent::Enter:
	case QEvent::HoverMove:
		OSD_ShowCursor(true);
		app->activateMouseCursorTimer();
		break;
	case QEvent::Leave:
		OSD_ShowCursor(true);
		break;
	default:;
	}

	return QGraphicsView::event(event);
}

void RenderView::paintEvent(QPaintEvent *event)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	fitContent();
	if(isActiveWindow()){
		if(app->isTiltEnabled()){
			// TILTモードの回転
			const qreal unit = 0.5; // 0.5度単位で回転
			rotate(unit * app->getTiltStep());
		}
	}
	QGraphicsView::paintEvent(event);
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
	case Qt::LeftButton:
		ev.mousebt.button = MBT_LEFT;	break;
	case Qt::RightButton:
		ev.mousebt.button = MBT_RIGHT;	break;
	case Qt::MiddleButton:
		ev.mousebt.button = MBT_MIDDLE;	break;
	default:
		return;
	}

	auto p = event->globalPos();
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
			char *data = new char[filename.size()];
			strcpy(data, filename.data());
			ev.drop.file = data;
			OSD_PushEvent(ev);
		}
	}
}


