#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QAbstractSocket>
#include "UdpDataWorkThread.h"

//组播发送
class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(UdpDataWorkThread *thread,const QString &addr,int port,QObject *parent = nullptr);
    ~UdpServer();

public:
    bool joinMultiGroup();
    bool joinMultiGroup(const QString &addr,int port);
    bool joinMulticastGroup(QUdpSocket *socket,const QHostAddress &groupAddress, const QNetworkInterface &iface);

    QString errorString();
    QAbstractSocket::SocketError error();

    bool leaveMultiGroup();

    qint64 write(const QByteArray &datagram);

    qint64 write(const char *data, qint64 size);

    void abort();

private slots:
    void slotReadPendingDatagrams();
    void slotReadPendingDatagrams1();

private:
    QUdpSocket *m_socket = nullptr; //eth0
    QUdpSocket *m_socket1 = nullptr; //eth1
    QHostAddress m_groupAddr; //组播地址
    int m_groupPort;

    UdpDataWorkThread *m_thread = NULL;
    QList<QString> m_cur;
    QList<QString> m_pre;
};

#endif // UDPSERVER_H
