#include "tipitem.h"
#include "ui_tipitem.h"
#include <QMessageBox>

TipItem::TipItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TipItem)
{
    ui->setupUi(this);
}

TipItem::~TipItem()
{
    delete ui;
}

void TipItem::setInfo(TipInfo info)
{
    m_tip = info;
    ui->btnQuestion->setHidden(!info.isShowQuestion);
    ui->widget->setShowSignal(info.isShowSignal);
    ui->widget->setHidden(!info.isShowQuestion);
    ui->lbText->setText(info.text);
}

void TipItem::setSignalLevel(SignalLevel level)
{
    ui->widget->setSignalLevel(level);
}

void TipItem::on_btnQuestion_clicked()
{
    QPoint btnGlobalPos = ui->btnQuestion->mapToGlobal(QPoint(0, 0));
    emit sigShowText(m_tip.tips,btnGlobalPos);
}
