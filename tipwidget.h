#ifndef TIPWIDGET_H
#define TIPWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QPoint>

// 自定义提示弹窗（无边框、悬浮、带阴影）
class TipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TipWidget(const QString &tipText, QWidget *parent = nullptr);

    // 设置弹窗显示位置（相对于目标按钮）
    void setPopupPos(const QString &tipText,QPoint pos);

protected:
    // 重写鼠标点击事件：点击弹窗外部关闭
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *tipLabel; // 提示文字标签
};

#endif // TIPWIDGET_H
