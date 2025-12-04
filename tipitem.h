#ifndef TIPITEM_H
#define TIPITEM_H

#include <QWidget>
#include "signalstrengthwidget.h"

namespace Ui {
class TipItem;
}

class TipItem : public QWidget
{
    Q_OBJECT

public:
    explicit TipItem(QWidget *parent = 0);
    ~TipItem();

    struct TipInfo
    {
        TipInfo(){}
        TipInfo(bool show,QString t,bool q,QString s)
            : isShowSignal(show),text(t),isShowQuestion(q),tips(s)
        {

        }
        bool isShowSignal = false;
        QString text;
        bool isShowQuestion = false;
        QString tips;
    };

    void setInfo(TipInfo info);

    void setSignalLevel(SignalLevel level);

signals:
    void sigShowText(QString text,QPoint pos);

private slots:
    void on_btnQuestion_clicked();

private:
    Ui::TipItem *ui;

private:
    TipInfo m_tip;
};

#endif // TIPITEM_H
