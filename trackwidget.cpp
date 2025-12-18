#include "trackwidget.h"
#include "ui_trackwidget.h"

TrackWidget::TrackWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackWidget)
{
    ui->setupUi(this);
}

TrackWidget::~TrackWidget()
{
    delete ui;
}

void TrackWidget::on_btnApply_clicked()
{

}
