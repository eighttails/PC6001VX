#include <QtOpenGL>
#include "renderview.h"

RenderView::RenderView(QGraphicsScene* scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(Qt::black);

    QGLWidget* glw = new QGLWidget(this);
    glw->setFormat(QGLFormat(QGL::SampleBuffers));
    setViewport(glw);
    setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
}

void RenderView::resizeEvent(QResizeEvent *event)
{
    qreal scaleRatio = qMin(width() / scene()->width(), height() / scene()->height());
    resetMatrix();
    scale(scaleRatio, scaleRatio);
}

