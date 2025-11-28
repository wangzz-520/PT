#ifndef USBBUTTON_H
#define USBBUTTON_H

#include <QWidget>

namespace Ui {
class UsbButton;
}

class UsbButton : public QWidget
{
    Q_OBJECT

public:
    explicit UsbButton(QWidget *parent = 0);
    ~UsbButton();

    void setConnected(bool isConnected,QString name);

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::UsbButton *ui;

    bool m_isConnected = false;
};

#endif // USBBUTTON_H
