#ifndef OVERVIEWWIDGET_H
#define OVERVIEWWIDGET_H

#include <QWidget>
#include "global.h"

namespace Ui {
class OverviewWidget;
}

class OverviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OverviewWidget(QWidget *parent = 0);
    ~OverviewWidget();

signals:
    void sigShowText(QString text,QPoint pos);

public:
    //更新x、y、z
    void updatePos(double x,double y,double z);

    //更新p、t、r
    void updateRotatePos(double pitch,double yaw,double roll);

    //更新界面展示
    void setInfo(OverviewInfo info);

    //设置特征点信号质量
    void setFeatureQuality(SignalLevel level);

    //设置地图匹配度质量
    void setMapQuality(SignalLevel level);

private:
    Ui::OverviewWidget *ui;
};

#endif // OVERVIEWWIDGET_H
