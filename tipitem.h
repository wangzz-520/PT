#ifndef TIPITEM_H
#define TIPITEM_H

#include <QWidget>

namespace Ui {
class TipItem;
}

class TipItem : public QWidget
{
    Q_OBJECT

public:
    explicit TipItem(QWidget *parent = 0);
    ~TipItem();

private:
    Ui::TipItem *ui;
};

#endif // TIPITEM_H
