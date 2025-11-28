#ifndef SIGNALSTRENGTHWIDGET_H
#define SIGNALSTRENGTHWIDGET_H

#include <QWidget>

class SignalStrengthWidget : public QWidget
{
    Q_OBJECT
public:
    // 信号强度级别（0=无信号，4=满信号）
    enum SignalLevel {
        Level0 = 0,
        Level1 = 1,
        Level2 = 2,
        Level3 = 3,
        Level4 = 4,
        Level5 = 5
    };

    explicit SignalStrengthWidget(QWidget *parent = nullptr);

    // 设置信号强度（会触发重绘）
    void setSignalLevel(SignalLevel level);

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent *event) override;

private:
    SignalLevel m_signalLevel; // 当前信号强度
};

#endif // SIGNALSTRENGTHWIDGET_H
