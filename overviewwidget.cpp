#include "overviewwidget.h"
#include "ui_overviewwidget.h"

OverviewWidget::OverviewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverviewWidget)
{
    ui->setupUi(this);

    TipItem::TipInfo info = TipItem::TipInfo(true,"特征点质量",true,"特征点质量");
    ui->itemFeature->setInfo(info);

    TipItem::TipInfo info2 = TipItem::TipInfo(true,"地图匹配度",true,"地图匹配度");
    ui->itemMap->setInfo(info2);

    connect(ui->itemFeature,&TipItem::sigShowText,this,&OverviewWidget::sigShowText);
    connect(ui->itemMap,&TipItem::sigShowText,this,&OverviewWidget::sigShowText);
}

OverviewWidget::~OverviewWidget()
{
    delete ui;
}

void OverviewWidget::updatePos(double x, double y, double z)
{
    //XYZ 范围- 131.0720M~- 131.0720 M 小数点后保留3位
    ui->lbX->setText(QString::number(x,'f',3));
    ui->lbY->setText(QString::number(y,'f',3));
    ui->lbZ->setText(QString::number(z,'f',3));
}

void OverviewWidget::updateRotatePos(double pitch, double yaw, double roll)
{
    //小数点后保留2位
    ui->lbP->setText(QString::number(pitch,'f',2));
    ui->lbT->setText(QString::number(yaw,'f',3));
    ui->lbR->setText(QString::number(roll,'f',3));
}

void OverviewWidget::setInfo(OverviewInfo info)
{
    ui->lbProtocalType->setText(info.protocal);
    ui->lbFreq->setText(info.freq);
    ui->lbDstIp->setText(info.dstIp);
    ui->lbDstPort->setText(info.dstPort);
    ui->lbCameraID->setText(info.dstId);
    ui->lbZoom->setText(info.encodeZoom);
    ui->lbFocus->setText(info.encodeFocus);
}

void OverviewWidget::setFeatureQuality(SignalLevel level)
{
    ui->itemFeature->setSignalLevel(level);
}

void OverviewWidget::setMapQuality(SignalLevel level)
{
    ui->itemMap->setSignalLevel(level);
}
