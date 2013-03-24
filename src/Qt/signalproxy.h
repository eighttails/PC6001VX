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
    void menuRequested(int x, int y);
public slots:
    void updateImage(HWINDOW Wh, int x, int y, double aspect, QImage image);
    void showPopupMenu(int x, int y);
};

extern SignalProxy signalProxy;

#endif // SIGNALPROXY_H
