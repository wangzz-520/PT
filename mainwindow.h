#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

namespace Ui {
class MainWindow;
}

class MenuItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotMenuClicked();

private:
    Ui::MainWindow *ui;

private:
    QList<MenuItem *> m_lstWidget;
};

#endif // MAINWINDOW_H
