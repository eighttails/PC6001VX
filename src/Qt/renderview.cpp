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

#ifndef NOOPENGL
    P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
    if(!app->isSafeMode() &&
            P6VXApp::getSetting(P6VXApp::keyHwAccel).toBool()){
        QGLWidget* glw = new QGLWidget(this);
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

    // ウィンドウ位置とサイズを復元
    restoreGeometry(P6VXApp::getSetting(P6VXApp::keyGeometry).toByteArray());
    if(P6VXApp::getSetting(P6VXApp::keyMaximized).toBool()){
        showMaximized();
    }

}

RenderView::~RenderView()
{
}

void RenderView::fitContent()
{
    const bool fixMag = P6VXApp::getSetting(P6VXApp::keyFixMagnification).toBool();
    //ウィンドウ全体に表示されるように表示倍率を調整
    qreal scaleRatio = fixMag
            ? P6VXApp::getSetting(P6VXApp::keyMagnification).toReal()
            : qMin(width() / scene()->width(), height() / scene()->height());
    resetTransform();
    scale(scaleRatio, scaleRatio);

    //表示倍率固定の場合は中心に配置
    if (fixMag){
        translate((width() - scene()->width()) / 2, (height() - scene()->height()) / 2);
    }else {
        P6VXApp::setSetting(P6VXApp::keyMagnification, scaleRatio);
    }
}

void RenderView::resizeWindowByRatio(int ratio)
{
    qreal r = double(ratio) / 100;
    // 最大化、フルスクリーン中は倍率固定モードにする。
    if((windowState() & Qt::WindowMaximized) || (windowState() & Qt::WindowFullScreen)){
        P6VXApp::setSetting(P6VXApp::keyFixMagnification, true);
        P6VXApp::setSetting(P6VXApp::keyMagnification, r);
        return;
    } else {
        P6VXApp::setSetting(P6VXApp::keyFixMagnification, false);
    }
    setGeometry(x(), y(), scene()->width() * r, scene()->height() * r);
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

void RenderView::closeEvent(QCloseEvent *event)
{
    // ウィンドウ位置とサイズを保存
    P6VXApp::setSetting(P6VXApp::keyGeometry, saveGeometry());
    P6VXApp::setSetting(P6VXApp::keyMaximized, isMaximized());
    QGraphicsView::closeEvent(event);
}


