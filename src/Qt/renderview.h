#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QGraphicsView>

class RenderView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RenderView(QGraphicsScene *scene, QWidget *parent = 0);
    
signals:
    
public slots:
protected:
    virtual void resizeEvent(QResizeEvent* event);
};

#endif // RENDERVIEW_H
