#include <QtOpenGL>
#include "renderview.h"

RenderView::RenderView(QGraphicsScene* scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    QGLWidget* glw = new QGLWidget(this);
    glw->setFormat(QGLFormat(QGL::SampleBuffers|QGL::DirectRendering));
    setViewport(glw);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::HighQualityAntialiasing);
    fitInView(this->scene()->sceneRect(),Qt::KeepAspectRatio);
}

void RenderView::resizeEvent(QResizeEvent *event)
{
    fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}

