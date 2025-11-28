#include "usbbutton.h"
#include "ui_usbbutton.h"
#include <QPainter>

UsbButton::UsbButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsbButton)
{
    ui->setupUi(this);
    ui->btnItem->setIconSize(QSize(60,50));
}

UsbButton::~UsbButton()
{
    delete ui;
}

void UsbButton::setConnected(bool isConnected,QString name)
{
    m_isConnected = isConnected;
    if(isConnected)
    {
        ui->btnItem->setIcon(QIcon(":/image/USB_fill.png"));
        ui->btnItem->setText(name);
        ui->btnItem->setStyleSheet("background: rgb(255,255,255);border:none;color:#00FF00;");
    }
    else
    {
        ui->btnItem->setIcon(QIcon(":/image/USB-nofill.png"));
        ui->btnItem->setText(name);
        ui->btnItem->setStyleSheet("background: rgb(255,255,255);border:none;color:#BD3124;");
    }
}

void UsbButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
    //画字
    if(!m_isConnected)
    {
        QImage image(":/image/USB_error.png");
        image = image.scaled(30,30);
        painter.drawImage(this->width()-30,0,image);
    }

    QWidget::paintEvent(event);
}
