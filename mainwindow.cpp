#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tipitem.h"
#include "tipwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    ui->widget->setSignalLevel(SignalStrengthWidget::Level1);

    TipItem::TipInfo info = TipItem::TipInfo(true,"1234",true,"123214");
    ui->widget_2->setInfo(info);
    connect(ui->widget_2,&TipItem::sigShowText,this,&MainWindow::slotShowTips);
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
}

void MainWindow::slotShowTips(const QString &tipText, QPoint pos)
{
    m_tipWidget->setPopupPos(tipText,pos);
    m_tipWidget->show();
}
