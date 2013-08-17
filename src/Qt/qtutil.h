#ifndef QTUTIL_H
#define QTUTIL_H
#include <QDebug>
#include "../osd.h"

// 処理時間計測用クラス
class MeasureTime
{
public:
    MeasureTime(QString name, quint32 threshold = 0)
        : start_(0)
        , end_(0)
        , name_(name)
        , threshold_(threshold)
    {
        start_ = OSD_GetTicks();
    }
    ~MeasureTime()
    {
        end_ = OSD_GetTicks();
        if(end_ - start_ >= threshold_){
            qDebug() << "ELAPSED TIME(" << name_ << "):" << end_ - start_ << "ms.";
        }
    }

private:
    DWORD start_;
    DWORD end_;
    QString name_;
    quint32 threshold_; //経過時間が一定値を超えた場合のみ出力する
};

#endif // QTUTIL_H
