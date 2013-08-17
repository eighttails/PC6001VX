#ifndef QTUTIL_H
#define QTUTIL_H
#include <QDebug>
#include "../osd.h"

// 処理時間計測用クラス
class MeasureTime
{
public:
    MeasureTime(QString name)
        : start_(0)
        , end_(0)
        , name_(name)
    {
        start_ = OSD_GetTicks();
    }
    ~MeasureTime()
    {
        end_ = OSD_GetTicks();
        qDebug() << "ELAPSED TIME(" << name_ << "):" << end_ - start_ << "ms.";
    }

private:
    DWORD start_;
    DWORD end_;
    QString name_;
};

#endif // QTUTIL_H
