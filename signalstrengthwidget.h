#ifndef SIGNALSTRENGTHWIDGET_H
#define SIGNALSTRENGTHWIDGET_H

#include <QWidget>
#include "global.h"

class SignalStrengthWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalStrengthWidget(QWidget *parent = nullptr);

    // 设置信号强度（会触发重绘）
    void setSignalLevel(SignalLevel level);

    void setShowSignal(bool isShow);

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent *event) override;

private:
    SignalLevel m_signalLevel; // 当前信号强度

    bool m_isShowSignal;
};

#endif // SIGNALSTRENGTHWIDGET_H
