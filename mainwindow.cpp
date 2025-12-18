#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tipitem.h"
#include "tipwidget.h"
#include "grayimageglwidget.h"
#include "customplot.h"
#include <Eigen/Core>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUi();

    m_customPlot = new CustomPlot(this);
    m_glWidget = new GrayImageGLWidget(this);
    m_brightnessPlot = new CustomPlot(this);

    m_tipWidget = new TipWidget("",this);

    ui->widgetLeftLayout->addWidget(m_customPlot);
    ui->widgetRightUpLayout->addWidget(m_glWidget);
    ui->widgetRightDownLayout->addWidget(m_brightnessPlot);

    connect(ui->widgetOverView,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);
    connect(ui->widgetMap,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);
    connect(ui->widgetTrack,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);
    connect(ui->widgetSetting,&MenuItem::clicked,this,&MainWindow::slotMenuClicked);
    connect(ui->pageOverView,&OverviewWidget::sigShowText,this,&MainWindow::slotShowTips);

    QTimer::singleShot(2000,this,[=]{
        m_glWidget->loadGrayImage(QImage(":/image/noPic.png"));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePcInfo(PcInfo info)
{
    m_lbCpu->setText(info.cpu);
    m_lbTemp->setText(info.temp);
    m_lbMem->setText(info.mem);
    m_lbIp->setText(info.Ip);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        m_bMousePress = false;
        return QWidget::mousePressEvent(event);
    }
    else if (event->button() == Qt::LeftButton)
    {
        // 只有在标题栏区域内按下鼠标左键才可以进行拖动操作
        if (ui->frameTitle->geometry().contains(event->pos()))
        {
            m_bMousePress = true;
        }
    }
    m_Move_point = event->globalPos() - pos();
    return QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMousePress = false;
    return QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMousePress)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - m_Move_point);
    }
    return QWidget::mouseMoveEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (ui->frameTitle->geometry().contains(event->pos()))
    {
        if(m_isShowNormal){
            this->showMaximized();
            this->move(0,0);
        }
        else
            this->showNormal();

        m_isShowNormal = !m_isShowNormal;
    }
    return QWidget::mouseDoubleClickEvent(event);
}

void MainWindow::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);

    m_infoWidget = new QWidget(this);
    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->setContentsMargins(0,0,60,0);
    m_infoWidget->setLayout(infoLayout);
    infoLayout->addStretch();

    m_statusWidget = new QWidget(this);
    m_statusWidget->setObjectName("statusWidget");
    m_statusWidget->setStyleSheet("*\
                                   {\
                                       font-size: 20px;\
                                       color: #ECECEC;\
                                   }\
                                   QWidget#statusWidget\
                                   {\
                                       background:#7F7F7F;\
                                   }");
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->setSpacing(20);
    m_statusWidget->setLayout(hLayout);
    infoLayout->addWidget(m_statusWidget);

    ui->statusBar->addPermanentWidget(m_infoWidget);

    QHBoxLayout *hLayoutCpu = new QHBoxLayout();
    hLayoutCpu->setContentsMargins(0,0,0,0);
    hLayoutCpu->addWidget(new QLabel("CPU:"));
    m_lbCpu = new QLabel("0%");
    m_lbCpu->setStyleSheet("color:#FFC000");
    hLayoutCpu->addWidget(m_lbCpu);

    QHBoxLayout *hLayoutTemp = new QHBoxLayout();
    hLayoutTemp->setContentsMargins(0,0,0,0);
    hLayoutTemp->addWidget(new QLabel("Temp:"));
    m_lbTemp = new QLabel("0℃");
    m_lbTemp->setStyleSheet("color:#FFC000");
    hLayoutTemp->addWidget(m_lbTemp);

    QHBoxLayout *hLayoutMem = new QHBoxLayout();
    hLayoutMem->setContentsMargins(0,0,0,0);
    hLayoutMem->addWidget(new QLabel("Mem:"));
    m_lbMem = new QLabel("0%");
    m_lbMem->setStyleSheet("color:#FFC000");
    hLayoutMem->addWidget(m_lbMem);

    QHBoxLayout *hLayoutIP = new QHBoxLayout();
    hLayoutIP->setContentsMargins(0,0,0,0);
    hLayoutIP->addWidget(new QLabel("IP:"));
    m_lbIp = new QLabel("0.0.0.0");
    m_lbIp->setStyleSheet("color:#FFC000");
    hLayoutIP->addWidget(m_lbIp);

    hLayout->addLayout(hLayoutCpu);
    hLayout->addLayout(hLayoutTemp);
    hLayout->addLayout(hLayoutMem);
    hLayout->addLayout(hLayoutIP);


    ui->usbBtn->setConnected(false,"USB未连接");
    ui->widgetOverView->setInfo(":/image/overview.png","总览");
    ui->widgetMap->setInfo(":/image/map.png","环境识别");
    ui->widgetTrack->setInfo(":/image/track.png","跟踪发送位置");
    ui->widgetSetting->setInfo(":/image/setting.png","高级");

    ui->widgetOverView->setSelect(true);
    m_lstWidget<<ui->widgetOverView<<ui->widgetMap<<ui->widgetTrack<<ui->widgetSetting;
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

        ui->widgetRightUpLayout->addWidget(m_glWidget);
        m_customPlot->show();
        ui->widgetRightUp->show();
        ui->videoLayout->setStretch(0,2);
        ui->videoLayout->setStretch(1,1);
    }
    else if(item == ui->widgetMap)
    {
        ui->frameVideo->show();
        ui->stackedWidget->setCurrentWidget(ui->pageMap);
        ui->widgetLeftLayout->addWidget(m_glWidget);

        m_customPlot->hide();
        ui->widgetRightUp->hide();
        ui->videoLayout->setStretch(0,1);
        ui->videoLayout->setStretch(1,1);
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
