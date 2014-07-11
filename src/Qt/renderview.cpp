#ifndef NOOPENGL
#include <QtOpenGL>
#endif
#include <QtWidgets>
#include "renderview.h"
#include "qtp6vxapplication.h"

RenderView::RenderView(QGraphicsScene* scene, QWidget *parent)
	: QGraphicsView(scene, parent)
{
    setSizeIncrement(1, 1);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(Qt::black);
    setStyleSheet( "QGraphicsView { border-style: none; }" );

#ifndef NOOPENGL
    QGLWidget* glw = new QGLWidget(this);
    // QGraphicsViewに使うにはOpenGL2以上が必要
    if(glw->format().majorVersion() >= 2){
        setViewport(glw);
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    } else {
        glw->deleteLater();
    }
#endif

#ifdef WIN32
    //IMEを無効化
    ImmAssociateContext( (HWND)winId(), NULL );
#endif
}

void RenderView::fitContent()
{
    //ウィンドウ全体に表示されるように表示倍率を調整
    qreal scaleRatio = qMin(width() / scene()->width(), height() / scene()->height());
    resetTransform();
    scale(scaleRatio, scaleRatio);
}

void RenderView::paintEvent(QPaintEvent *event)
{
    QtP6VXApplication* app = qobject_cast<QtP6VXApplication*>(qApp);
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

