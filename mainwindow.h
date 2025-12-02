#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

namespace Ui {
class MainWindow;
}
class TipWidget;
class MenuItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotMenuClicked();

    void slotShowTips(const QString &tipText,QPoint pos);

    void on_btnMin_clicked();

    void on_btnMax_clicked();

    void on_btnClose_clicked();

private:
    Ui::MainWindow *ui;

private:
    QList<MenuItem *> m_lstWidget;

    TipWidget *m_tipWidget = nullptr;

    bool m_isShowNormal = true;
};

#endif // MAINWINDOW_H
