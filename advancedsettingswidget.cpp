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
