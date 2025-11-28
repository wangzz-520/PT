#include "menuitem.h"
#include "ui_menuitem.h"
#include <QIcon>

MenuItem::MenuItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuItem)
{
    ui->setupUi(this);

    ui->frameBtn->installEventFilter(this);
}

MenuItem::~MenuItem()
{
    delete ui;
}

void MenuItem::setInfo(QString url, QString name)
{
    ui->btnItem->setIcon(QIcon(url));
    ui->btnItem->setIconSize(QSize(60,50));
    ui->btnItem->setText(name);
    ui->btnItem->setStyleSheet("background: rgb(255,255,255);border:none;color:rgb(64,149,229);");
}

void MenuItem::setSelect(bool select)
{
    if(select)
    {
        ui->frameMenu->setStyleSheet("background: rgb(33,33,33);");
        ui->btnItem->setStyleSheet("background: rgb(33,33,33);border:none;color:rgb(64,149,229);");
        ui->frameBtn->setStyleSheet("border: none;");
    }
    else
    {
        ui->frameMenu->setStyleSheet("background: rgb(255,255,255);");
        ui->btnItem->setStyleSheet("background: rgb(255,255,255);border:none;color:rgb(64,149,229);");
        ui->frameBtn->setStyleSheet("border: 1px solid #CECECE;");
    }
}

bool MenuItem::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && obj == ui->frameBtn) {
        emit clicked();
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

void MenuItem::on_btnItem_clicked()
{
    emit clicked();
}

void MenuItem::on_btnItem_triggered(QAction *arg1)
{

}
