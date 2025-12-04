#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QLabel>
#include "global.h"

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

public:
    //更新cpu占用、内存、温度等信息
    void updatePcInfo(PcInfo info);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void initUi();

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

    QLabel *m_lbCpu = nullptr;
    QLabel *m_lbTemp = nullptr;
    QLabel *m_lbMem = nullptr;
    QLabel *m_lbIp = nullptr;

    QWidget *m_statusWidget = nullptr;
    QWidget *m_infoWidget = nullptr;
};

#endif // MAINWINDOW_H
