#include "UdpDataWorkThread.h"

UdpDataWorkThread::UdpDataWorkThread(QObject *parent)
    : QThread(parent)
{

}

UdpDataWorkThread::~UdpDataWorkThread()
{
}

void UdpDataWorkThread::push(const QByteArray &data)
{
    m_mutex.lock();
    m_dataQue.enqueue(data);
    m_mutex.unlock();
}

QByteArray UdpDataWorkThread::pop()
{
    m_mutex.lock();
    QByteArray ba = m_dataQue.dequeue();
    m_mutex.unlock();
    return ba;
}

void UdpDataWorkThread::stop()
{
    m_isStart = false;
}

void UdpDataWorkThread::run()
{
    m_isStart = true;
    while(m_isStart)
    {
        if(!m_dataQue.isEmpty())
        {
            QByteArray dataArray = pop();

        }
        else
        {
            msleep(5);
        }
        msleep(5);
    }
}
