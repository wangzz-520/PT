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
}

OverviewWidget::~OverviewWidget()
{
    delete ui;
}
