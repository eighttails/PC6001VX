#ifndef SIGNALPROXY_H
#define SIGNALPROXY_H

#include <QObject>
#include <QImage>
#include "../typedef.h"

class SignalProxy : public QObject
{
    Q_OBJECT
public:
    explicit SignalProxy(QObject *parent = 0);
    
signals:
    void imageUpdated(HWINDOW Wh, int x, int y, double aspect, QImage image);
public slots:
    void updateImage(HWINDOW Wh, int x, int y, double aspect, QImage image);
};

#endif // SIGNALPROXY_H
