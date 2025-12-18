#include "mapwidget.h"
#include "ui_mapwidget.h"
#include <QButtonGroup>

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWidget)
{
    ui->setupUi(this);

    TipItem::TipInfo info = TipItem::TipInfo(false,"环境地图",true,"环境地图");
    ui->widgetMap->setInfo(info);

    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->rbManual);
    btnGroup->addButton(ui->rbAuto);
    btnGroup->setExclusive(true);
    ui->rbManual->setChecked(true);

    QButtonGroup *btnGroup2 = new QButtonGroup(this);
    btnGroup2->addButton(ui->rbMap);
    btnGroup2->addButton(ui->rbReal);
    btnGroup2->setExclusive(true);
    ui->rbMap->setChecked(true);
}

MapWidget::~MapWidget()
{
    delete ui;
}

void MapWidget::on_btnLoadMap_clicked()
{
    //加载地图
}

void MapWidget::on_btnNewMap_clicked()
{
    //新建地图
}

void MapWidget::on_rbMap_toggled(bool checked)
{
    if(checked)
        ui->stackedWidget->setCurrentIndex(0);
}

void MapWidget::on_rbReal_toggled(bool checked)
{
    if(checked)
        ui->stackedWidget->setCurrentIndex(1);
}

void MapWidget::on_btnSetOri_clicked()
{

}
