#include "tipitem.h"
#include "ui_tipitem.h"

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
