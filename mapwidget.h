#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>

namespace Ui {
class MapWidget;
}

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = 0);
    ~MapWidget();

private slots:
    void on_btnLoadMap_clicked();

    void on_btnNewMap_clicked();

    void on_rbMap_toggled(bool checked);

    void on_rbReal_toggled(bool checked);

    void on_btnSetOri_clicked();

private:
    Ui::MapWidget *ui;
};

#endif // MAPWIDGET_H
