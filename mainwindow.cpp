#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tipitem.h"
#include "tipwidget.h"
#include <Eigen/Core>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    m_tipWidget = new TipWidget("",this);

    ui->usbBtn->setConnected(false,"USB未连接");

    ui->widgetOverView->setInfo(":/image/overview.png","总览");
    ui->widgetMap->setInfo(":/image/map.png","环境识别");
    ui->widgetTrack->setInfo(":/image/track.png","跟踪发送位置");
    ui->widgetSetting->setInfo(":/image/setting.png","高级");

    ui->widgetOverView->setSelect(true);

    connect(ui->widgetOverView,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);
    connect(ui->widgetMap,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);
    connect(ui->widgetTrack,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);
    connect(ui->widgetSetting,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);

    m_lstWidget<<ui->widgetOverView<<ui->widgetMap<<ui->widgetTrack<<ui->widgetSetting;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotMenuClicked()
{
    for(int i=0;i<m_lstWidget.size();i++)
    {
        m_lstWidget.at(i)->setSelect(false);
    }

    MenuItem* item = qobject_cast<MenuItem*>(sender());
    item->setSelect(true);

    if(item == ui->widgetOverView)
    {
        ui->frameVideo->show();
        ui->stackedWidget->setCurrentWidget(ui->pageOverView);
    }
    else if(item == ui->widgetMap)
    {
        ui->frameVideo->show();
        ui->stackedWidget->setCurrentWidget(ui->pageMap);
    }
    else if(item == ui->widgetTrack)
    {
        ui->frameVideo->hide();
        ui->stackedWidget->setCurrentWidget(ui->pageTrack);
    }
    else if(item == ui->widgetSetting)
    {
        ui->frameVideo->hide();
        ui->stackedWidget->setCurrentWidget(ui->pageAdvanced);
    }

}

void MainWindow::slotShowTips(const QString &tipText, QPoint pos)
{
    m_tipWidget->setPopupPos(tipText,pos);
    m_tipWidget->show();
}

void MainWindow::on_btnMin_clicked()
{
    this->showMinimized();
}

void MainWindow::on_btnMax_clicked()
{
    if(m_isShowNormal)
        this->showMaximized();
    else
        this->showNormal();

    m_isShowNormal = !m_isShowNormal;
}

void MainWindow::on_btnClose_clicked()
{
    qApp->exit();
}
