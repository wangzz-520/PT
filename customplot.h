#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H


#include "qcustomplot.h"
#include <QPointF>

/*!
    @ClassName   : Class
    @Description : 自定义画轨迹图曲线
    @Author      : xxx
    @Data        : xxx
*/

//更新频率按视觉帧率 1s 25帧

class CustomPlot : public QCustomPlot
{
public:
    CustomPlot(QWidget *parent = nullptr);

public:
    //清空
    void clear();

    void addPoint(const QPointF &point);

private:
    void init();
};

#endif // CUSTOMPLOT_H
