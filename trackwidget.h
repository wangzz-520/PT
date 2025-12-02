#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>

namespace Ui {
class TrackWidget;
}

class TrackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrackWidget(QWidget *parent = 0);
    ~TrackWidget();

private:
    Ui::TrackWidget *ui;
};

#endif // TRACKWIDGET_H
