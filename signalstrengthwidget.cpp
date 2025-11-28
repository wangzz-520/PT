#include "SignalStrengthWidget.h"
#include <QPainter>
#include <QColor>

SignalStrengthWidget::SignalStrengthWidget(QWidget *parent)
    : QWidget(parent), m_signalLevel(Level0)
{
    // 设置控件默认大小
    //setFixedSize(60, 40);
}

void SignalStrengthWidget::setSignalLevel(SignalLevel level)
{
    if (m_signalLevel != level) {
        m_signalLevel = level;
        update(); // 触发重绘
    }
}

void SignalStrengthWidget::setShowSignal(bool isShow)
{
    m_isShowSignal = isShow;
}

void SignalStrengthWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if(!m_isShowSignal)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    const int barCount = 5; // 共4个条形（对应4级信号）
    const int totalWidth = width() - 10; // 条形总宽度（留边距）
    const int totalHeight = height() - 5; // 条形总高度
    const int barSpacing = 2; // 条形之间的间距

    // 计算每个条形的宽度（从左到右递增）
    int barWidths[barCount] = {totalWidth/8, totalWidth/8, totalWidth/8, totalWidth/8,totalWidth/8};
    // 计算每个条形的高度（从左到右递增，对应信号强度）
    int barHeights[barCount] = {totalHeight/2-20, totalHeight/2-10, totalHeight/2, totalHeight/2+10,totalHeight/2+20};

    // 计算第一个条形的起始X坐标（居中显示）
    int totalBarWidth = 0;
    for (int w : barWidths)
        totalBarWidth += w;

    totalBarWidth += barSpacing * (barCount - 1);

    int startX = (width() - totalBarWidth) / 2;
    int startY = height() - 5; // 条形底部Y坐标（贴底）


    // 绘制每个条形
    QColor activeColor(14, 14, 14); // 有效信号颜色（红色）
    QColor inactiveColor(200, 200, 200); // 无效信号颜色（浅灰）

    for (int i = 0; i < barCount; ++i)
    {
        // 确定当前条形的颜色（信号强度≥当前级别则为有效色）
        QColor color = (m_signalLevel >= i+1) ? activeColor : inactiveColor;
        painter.setBrush(color);
        painter.setPen(Qt::NoPen); // 无轮廓线

        // 绘制矩形条形
        QRect barRect(
            startX,
            startY - barHeights[i], // 顶部Y坐标 = 底部Y - 高度
            barWidths[i],
            barHeights[i]
        );
        painter.drawRoundedRect(barRect, 3, 3); // 轻微圆角

        // 更新下一个条形的X坐标
        startX += barWidths[i] + barSpacing;
    }
}
