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

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

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

    bool m_bMousePress = false;         //标记鼠标状态：按下；未按下
    QPoint m_Move_point;                //标记鼠标移动距离
};

#endif // MAINWINDOW_H
