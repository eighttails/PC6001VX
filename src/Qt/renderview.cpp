#ifndef NOOPENGL
#include <QtOpenGL>
#endif
#include <QtWidgets>
#include "renderview.h"

RenderView::RenderView(QGraphicsScene* scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    setSizeIncrement(1, 1);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(Qt::black);

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

}

void RenderView::fitContent()
{
    qreal scaleRatio = qMin(width() / scene()->width(), height() / scene()->height());
    resetTransform();
    scale(scaleRatio, scaleRatio);
}

void RenderView::paintEvent(QPaintEvent *event)
{
    if(isActiveWindow()){
        fitContent();
        if(qApp->property("TILTEnabled").toBool()){
            rotate(qApp->property("TILT").toReal());
        }
    }
    QGraphicsView::paintEvent(event);
}

