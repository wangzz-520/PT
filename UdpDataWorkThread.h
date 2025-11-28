#ifndef UDPDATAWORKTHREAD_H
#define UDPDATAWORKTHREAD_H

#include <QThread>
#include <QObject>
#include <QByteArray>
#include <QQueue>
#include <QMutex>

class UdpDataWorkThread : public QThread
{
    Q_OBJECT
public:
    UdpDataWorkThread(QObject *parent = nullptr);
    ~UdpDataWorkThread();

public:
    void push(const QByteArray& data);
    QByteArray pop();

public:
    //停止线程
    void stop();

protected:
    void run();

private:
    bool m_isStart = false;     //线程是否开始

    QQueue<QByteArray> m_dataQue;

    QMutex m_mutex;
};

#endif // UDPDATAWORKTHREAD_H
