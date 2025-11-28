#include "UdpServer.h"
#include <QNetworkInterface>
#include <QDebug>
#include <QtEndian>
#include <QTimer>

QTimer timer;
UdpServer::UdpServer(UdpDataWorkThread *thread,const QString &addr,int port,QObject *parent)
    : QObject(parent)
    , m_groupPort(port)
    , m_socket(NULL)
    , m_thread(thread)
{
    m_groupAddr.setAddress(addr);

    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()),
                 this, SLOT(slotReadPendingDatagrams()));

    m_socket1 = new QUdpSocket(this);
    connect(m_socket1, SIGNAL(readyRead()),
                 this, SLOT(slotReadPendingDatagrams1()));

    timer.start(2000);
    connect(&timer,&QTimer::timeout,this,[=]{
       m_cur.clear();
       QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

       for(int i=0;i<interfaces.size();i++)
       {
          if(interfaces.at(i).flags().testFlag(QNetworkInterface::IsUp) &&
                 interfaces.at(i).flags().testFlag(QNetworkInterface::IsRunning)
                  && (interfaces.at(i).name() == "eth0" || interfaces.at(i).name() == "eth1"))
          {
              QString name = interfaces.at(i).name();
              m_cur.append(name);

              if(!m_pre.contains(name) && name == "eth0")
              {
                  joinMulticastGroup(m_socket,m_groupAddr,interfaces.at(i));
              }

              if(!m_pre.contains(name) && name == "eth1")
              {
                  joinMulticastGroup(m_socket1,m_groupAddr,interfaces.at(i));
              }
          }
       }

       m_pre = m_cur;
   });
}

UdpServer::~UdpServer()
{
    if(m_socket)
    {
        m_socket->abort();
        delete m_socket;
    }
}

bool UdpServer::joinMultiGroup()
{
    bool isSuccess = false;
//    if(m_socket->bind(QHostAddress(BussinessLogic::getInstance()->getPowerIP()),m_groupPort,QAbstractSocket::DefaultForPlatform))
//    {
//       QNetworkInterface interface;
//       QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
//       for(int i=0;i<interfaces.size();i++)
//       {
//          if(interfaces.at(i).flags().testFlag(QNetworkInterface::IsUp) &&
//                 interfaces.at(i).flags().testFlag(QNetworkInterface::IsRunning)
//                  && (interfaces.at(i).name() == "eth0" || interfaces.at(i).name() == "eth1"))
//          {

//             interface = interfaces.at(i);

//             QString log = QString("interface name = %1,flag = %2,isvalid = %3)")
//                     .arg(interfaces.at(i).name())
//                     .arg(interfaces.at(i).flags())
//                     .arg(interfaces.at(i).isValid());

//             BaseFunc::info(log);

//             if(interface.name() == "eth0")
//             {
//                m_socket->setMulticastInterface(interface);
//                m_socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption,0);

//                isSuccess = m_socket->joinMulticastGroup(m_groupAddr,interface);

//                if(isSuccess)
//                {
//                  m_cur.append(interface.name());
//                  m_pre.append(interface.name());
//                  qDebug()<<QString("(%1:%2,%3)joinMultiGroup success !!!")
//                            .arg(m_groupAddr.toString()).arg(m_groupPort).arg(interface.name());
//                  BaseFunc::info(QString("(%1:%2,%3)joinMultiGroup success !!!")
//                            .arg(m_groupAddr.toString()).arg(m_groupPort).arg(interface.name()));
//                }

//                else
//                {
//                  qDebug()<<QString("(%1:%2,%3)joinMultiGroup failed error str = %4!!!").arg(m_groupAddr.toString())
//                                  .arg(m_groupPort).arg(interface.name()).arg(this->errorString());
//                  BaseFunc::info(QString("(%1:%2,%3)joinMultiGroup failed error str = %4!!!").arg(m_groupAddr.toString())
//                                 .arg(m_groupPort).arg(interface.name()).arg(this->errorString()));
//                }
//             }
//          }
//       }
//    }

//    if(m_socket1->bind(QHostAddress(BussinessLogic::getInstance()->getPowerIP2()),m_groupPort,QAbstractSocket::DefaultForPlatform))
//    {
//        QNetworkInterface interface;
//        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
//        for(int i=0;i<interfaces.size();i++)
//        {
//           if(interfaces.at(i).flags().testFlag(QNetworkInterface::IsUp) &&
//                  interfaces.at(i).flags().testFlag(QNetworkInterface::IsRunning)
//                   && (interfaces.at(i).name() == "eth0" || interfaces.at(i).name() == "eth1"))
//           {

//              interface = interfaces.at(i);

//              if(interface.name() == "eth1")
//              {
//                  m_socket1->setMulticastInterface(interface);
//                  m_socket1->setSocketOption(QAbstractSocket::MulticastLoopbackOption,0);

//                  isSuccess = m_socket1->joinMulticastGroup(m_groupAddr,interface);

//                  if(isSuccess)
//                  {
//                    m_cur.append(interface.name());
//                    m_pre.append(interface.name());
//                    qDebug()<<QString("(%1:%2,%3)joinMultiGroup success !!!")
//                              .arg(m_groupAddr.toString()).arg(m_groupPort).arg(interface.name());
//                    BaseFunc::info(QString("(%1:%2,%3)joinMultiGroup success !!!")
//                              .arg(m_groupAddr.toString()).arg(m_groupPort).arg(interface.name()));
//                  }

//                  else
//                  {
//                    qDebug()<<QString("(%1:%2,%3)joinMultiGroup failed error str = %4!!!").arg(m_groupAddr.toString())
//                                    .arg(m_groupPort).arg(interface.name()).arg(this->errorString());
//                    BaseFunc::info(QString("(%1:%2,%3)joinMultiGroup failed error str = %4!!!").arg(m_groupAddr.toString())
//                                   .arg(m_groupPort).arg(interface.name()).arg(this->errorString()));
//                  }
//              }
//           }
//        }
//    }

    return isSuccess;
}

bool UdpServer::joinMultiGroup(const QString &addr, int port)
{
    m_groupAddr.clear();
    m_groupAddr.setAddress(addr);
    m_groupPort = port;

    return joinMultiGroup();
}

bool UdpServer::joinMulticastGroup(QUdpSocket *socket,const QHostAddress &groupAddress, const QNetworkInterface &iface)
{
    socket->setMulticastInterface(iface);
    socket->setSocketOption(QAbstractSocket::MulticastLoopbackOption,0);
    return socket->joinMulticastGroup(groupAddress,iface);
}

QString UdpServer::errorString()
{
    return m_socket->errorString();
}

QAbstractSocket::SocketError UdpServer::error()
{
    return m_socket->error();
}

bool UdpServer::leaveMultiGroup()
{
    bool success = m_socket->leaveMulticastGroup(m_groupAddr);
    m_socket->abort();

    success = m_socket1->leaveMulticastGroup(m_groupAddr);
    m_socket1->abort();

    return success;
}

qint64 UdpServer::write(const QByteArray &datagram)
{
    qint64 size = 0;
    if(m_pre.contains("eth0"))
        size = m_socket->writeDatagram(datagram,m_groupAddr,m_groupPort);

    if(m_pre.contains("eth1"))
        size = m_socket1->writeDatagram(datagram,m_groupAddr,m_groupPort);
    //qDebug()<<QString("write %1:%2  size = %3").arg(m_groupAddr.toString()).arg(m_groupPort).arg(size);
    return size;
}

qint64 UdpServer::write(const char *data, qint64 size)
{
    qint64 sizeRet = 0;
    if(m_pre.contains("eth0"))
        sizeRet = m_socket->writeDatagram(data,size,m_groupAddr,m_groupPort);

    if(m_pre.contains("eth1"))
        sizeRet = m_socket1->writeDatagram(data,size,m_groupAddr,m_groupPort);
    return sizeRet;
}

void UdpServer::abort()
{
    m_socket->abort();
}

void UdpServer::slotReadPendingDatagrams()
{
    //qDebug()<<"hasPendingDatagrams....";
    while (m_socket->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(m_socket->pendingDatagramSize());

        QHostAddress peerAddr;
        quint16 peerPort;

        //读出的数据是大端格式
        m_socket->readDatagram(data.data(),data.size(),&peerAddr,&peerPort);

        QByteArray littleData = qToLittleEndian(data);

        if(m_thread)
            m_thread->push(littleData);
    }
}

void UdpServer::slotReadPendingDatagrams1()
{
    while (m_socket1->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(m_socket1->pendingDatagramSize());

        QHostAddress peerAddr;
        quint16 peerPort;

        //读出的数据是大端格式
        m_socket1->readDatagram(data.data(),data.size(),&peerAddr,&peerPort);

        QByteArray littleData = qToLittleEndian(data);

        //QMessageBox::information(NULL,"mul",QString::fromLocal8Bit( littleData.toHex()));

        if(m_thread)
            m_thread->push(littleData);

        //if(BussinessLogic::getInstance()->getTest())
//        {
//            printf("*****receive udp frame from (%s) data = ",peerAddr.toString().toStdString().c_str());
//            for(int i=0;i<littleData.length();i++)
//            {
//                printf("%02x",littleData.data()[i]);
//            }
//            printf("\n");
//        }
    }
}
