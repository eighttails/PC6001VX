#include <QtOpenGL>
#include "renderview.h"

RenderView::RenderView(QGraphicsScene* scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    setSizeIncrement(1, 1);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(Qt::black);

    QGLWidget* glw = new QGLWidget(this);
    if(glw->isValid()){
        setViewport(glw);
    } else {
        delete glw;
    }
}

void RenderView::fitContent()
{
    qreal scaleRatio = qMin(width() / scene()->width(), height() / scene()->height());
    resetMatrix();
    scale(scaleRatio, scaleRatio);
}

void RenderView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitContent();
}

