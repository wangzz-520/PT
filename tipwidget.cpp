#include "TipWidget.h"
#include <QApplication>
#include <QDesktopWidget>

TipWidget::TipWidget(const QString &tipText, QWidget *parent)
    : QWidget(parent)
{
    // 1. 基础设置：无边框、置顶、透明背景
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground); // 透明背景（用于显示阴影）
    setFixedWidth(400); // 弹窗固定宽度

    // 2. 提示文字标签
    tipLabel = new QLabel(tipText, this);
    tipLabel->setWordWrap(true); // 自动换行
    tipLabel->setStyleSheet("color: #333; font-size: 12px; padding: 8px;");

    // 3. 布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tipLabel);
    layout->setContentsMargins(0, 0, 0, 0); // 内边距（预留阴影空间）

    // 5. 给整个应用安装事件过滤器（监听点击外部）
    qApp->installEventFilter(this);
}

// 设置弹窗位置：在目标按钮的右侧，顶部对齐
void TipWidget::setPopupPos(const QString &tipText,QPoint pos)
{
    if(tipLabel)
        tipLabel->setText(tipText);

    // 获取按钮在屏幕上的坐标（全局坐标）
    QPoint btnGlobalPos = pos;
    // 弹窗位置：按钮右侧10px，顶部对齐
    int x = btnGlobalPos.x()+5;
    int y = btnGlobalPos.y()+5;

    // 防止弹窗超出屏幕边界
    QRect screenRect = QApplication::desktop()->availableGeometry();
    if (x + width() > screenRect.right()) {
        // 若右侧超出，显示在按钮左侧
        x = btnGlobalPos.x() - width() - 10;
    }
    if (y + height() > screenRect.bottom()) {
        // 若底部超出，向上偏移
        y = screenRect.bottom() - height() - 10;
    }

    move(x, y);
}

// 事件过滤器：点击弹窗外部或按ESC键，关闭弹窗
bool TipWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        // 点击的不是弹窗本身，关闭
        if (watched != this && !this->underMouse()) {
            close();
        }
    } else if (event->type() == QEvent::KeyPress) {
        // 按ESC键关闭
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            close();
        }
    }
    return QWidget::eventFilter(watched, event);
}
