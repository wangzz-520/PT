#ifndef MENUITEM_H
#define MENUITEM_H

#include <QWidget>

namespace Ui {
class MenuItem;
}

class MenuItem : public QWidget
{
    Q_OBJECT

public:
    explicit MenuItem(QWidget *parent = 0);
    ~MenuItem();

    void setInfo(QString url,QString name);

    void setSelect(bool select);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void clicked();

private slots:
    void on_btnItem_clicked();

    void on_btnItem_triggered(QAction *arg1);

private:
    Ui::MenuItem *ui;
};

#endif // MENUITEM_H
