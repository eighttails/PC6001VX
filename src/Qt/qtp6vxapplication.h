#ifndef QTP6VXAPPLICATION_H
#define QTP6VXAPPLICATION_H

#include <QtSingleApplication>

class QtP6VXApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit QtP6VXApplication(int &argc, char **argv);
    
    virtual bool notify(QObject *receiver, QEvent *event);
signals:
    
public slots:
    
};

#endif // QTP6VXAPPLICATION_H
