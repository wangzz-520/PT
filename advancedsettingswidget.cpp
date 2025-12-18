#include "advancedsettingswidget.h"
#include "ui_advancedsettingswidget.h"

AdvancedSettingsWidget::AdvancedSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvancedSettingsWidget)
{
    ui->setupUi(this);
}

AdvancedSettingsWidget::~AdvancedSettingsWidget()
{
    delete ui;
}

void AdvancedSettingsWidget::on_btnReset_clicked()
{

}

void AdvancedSettingsWidget::on_btnApply_clicked()
{

}
