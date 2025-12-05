#include "customplot.h"

CustomPlot::CustomPlot(QWidget *parent)
    : QCustomPlot(parent)
{
    init();
}

void CustomPlot::clear()
{
    this->clearGraphs();
    this->clearItems();
    init();
    this->replot();
}

void CustomPlot::addPoint(const QPointF &point)
{
    if(this->graphCount() > 0)
    {
        QCPGraph *graph = this->graph(0);
        graph->addData(point.x(),point.y());
    }
}

void CustomPlot::init()
{
    this->xAxis->setLabel("x");
    this->xAxis->setLabelColor(QColor(Qt::white));
    this->xAxis->setTickLabelSide(QCPAxis::lsInside);
    this->xAxis->setTickLabelFont(QFont("微软雅黑",8));
    this->xAxis->setTickLabelColor(QColor(Qt::white));
    this->xAxis->setBasePen(QPen(Qt::white));
    this->xAxis->setSubTickPen(QPen(Qt::white));

    this->yAxis->setLabel("y");
    this->yAxis->setLabelColor(QColor(Qt::white));
    this->yAxis->setTickLabelSide(QCPAxis::lsInside);
    this->yAxis->setTickLabelFont(QFont("微软雅黑",8));
    this->yAxis->setTickLabelColor(QColor(Qt::white));
    this->yAxis->setNumberFormat("f");
    this->yAxis->setNumberPrecision(1);
    this->yAxis->setBasePen(QPen(Qt::white));
    this->yAxis->setSubTickPen(QPen(Qt::white));

    this->setInteraction(QCP::iRangeDrag, true); //鼠标单击拖动
    this->setInteraction(QCP::iRangeZoom, true); //滚轮滑动缩放
    this->setInteraction(QCP::iSelectPlottables);//设置曲线可选

    this->setBackground(QBrush(QColor(105,105,105)));

    QCPGraph *graph = this->addGraph();

    //设置pen
    QPen pen;
    pen.setColor(QColor(Qt::black));
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(2.5);

    graph->setPen(pen);
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));
}
