#ifndef NOOPENGL
#include <QtOpenGL>
#endif
#include <QtWidgets>
#include <QSettings>
#include <QGestureEvent>
#include <QTapAndHoldGesture>

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
#if QT_VERSION >= 0x050400 && !defined (USE_QGLWIDGET)
		QOpenGLWidget* glw = new QOpenGLWidget(this);
#else
		// 描画に不具合が見られるプラットフォームでは当面QGLWidgetを使う。
		QGLWidget* glw = new QGLWidget(this);
#endif
        // QGraphicsViewに使うにはOpenGL2以上が必要
        if(glw->format().majorVersion() >= 2){
            setViewport(glw);
            setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        } else {
            glw->deleteLater();
        }
    }
#endif
    grabGesture(Qt::TapAndHoldGesture);

#ifdef WIN32
    //IMEを無効化
    ImmAssociateContext( (HWND)winId(), NULL );
#endif

}

RenderView::~RenderView()
{
}

void RenderView::fitContent()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	const bool fixMag = app->getSetting(P6VXApp::keyFixMagnification).toBool();
    //ウィンドウ全体に表示されるように表示倍率を調整
    qreal scaleRatio = fixMag
			? app->getSetting(P6VXApp::keyMagnification).toReal()
            : qMin(width() / scene()->width(), height() / scene()->height());
	resetTransform();
	centerOn(sceneRect().center());
	scale(scaleRatio, scaleRatio);

    //表示倍率固定の場合は中心に配置
    if (fixMag){
        translate((width() - scene()->width()) / 2, (height() - scene()->height()) / 2);
    }else {
		app->setSetting(P6VXApp::keyMagnification, scaleRatio);
    }
}

void RenderView::resizeWindowByRatio(int ratio)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	qreal r = double(ratio) / 100;
    // 最大化、フルスクリーン中は倍率固定モードにする。
    if((windowState() & Qt::WindowMaximized) || (windowState() & Qt::WindowFullScreen)){
		app->setSetting(P6VXApp::keyFixMagnification, true);
		app->setSetting(P6VXApp::keyMagnification, r);
        return;
    } else {
		app->setSetting(P6VXApp::keyFixMagnification, false);
    }
    setGeometry(x(), y(), scene()->width() * r, scene()->height() * r);

	emit resized(size());
}

bool RenderView::event(QEvent *event)
{
    if(QGestureEvent* gEvent = dynamic_cast<QGestureEvent*>(event)){
        if(QTapAndHoldGesture* tGesture = dynamic_cast<QTapAndHoldGesture*>(gEvent->gesture(Qt::TapAndHoldGesture))){
            QGraphicsItem* item = scene()->itemAt(mapToScene(tGesture->position().toPoint()), transform());
            // タップしたアイテムが原点にある場合、メイン画面とみなして長押しメニューを出す
            if(tGesture->state() == Qt::GestureFinished && item && item->pos() == QPoint(0, 0)){
                Event e;
                e.type = EV_CONTEXTMENU;
                e.mousebt.x = tGesture->position().x();
                e.mousebt.y = tGesture->position().y();
                OSD_PushEvent(e);
                return true;
            }
        }
    }
    return QGraphicsView::event(event);
}

void RenderView::paintEvent(QPaintEvent *event)
{
    P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
    if(isActiveWindow()){
        fitContent();
        if(app->isTiltEnabled()){
            //TILTモードの回転
            const qreal unit = 0.5; //0.5度単位で回転
            rotate(unit * app->getTiltStep());
        }
    }
    QGraphicsView::paintEvent(event);
}
