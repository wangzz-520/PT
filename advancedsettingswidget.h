#ifndef ADVANCEDSETTINGSWIDGET_H
#define ADVANCEDSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class AdvancedSettingsWidget;
}

class AdvancedSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedSettingsWidget(QWidget *parent = 0);
    ~AdvancedSettingsWidget();

private:
    Ui::AdvancedSettingsWidget *ui;
};

#endif // ADVANCEDSETTINGSWIDGET_H
