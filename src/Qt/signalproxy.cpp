#include "signalproxy.h"

SignalProxy::SignalProxy(QObject *parent) :
    QObject(parent)
{
}

void SignalProxy::updateImage(HWINDOW Wh, int x, int y, double aspect, QImage image){
    emit imageUpdated(Wh, x, y, aspect, image);
}