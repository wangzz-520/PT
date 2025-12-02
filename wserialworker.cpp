#include "wserialworker.h"
#include "WLog.h"
#include <QDebug>

WSerialWorker::WSerialWorker(SerialType type,QObject *parent)
    : QObject(parent)
    , m_type(type)
{
    m_serialPort = new QSerialPort(this);

    connect(m_serialPort, &QSerialPort::readyRead,
            this, &WSerialWorker::slotHandleReadyRead);

    connect(m_serialPort, &QSerialPort::errorOccurred,
            this, &WSerialWorker::slotHandleError);
}

WSerialWorker::~WSerialWorker()
{
    slotCloseSerialPort();
    m_serialPort->deleteLater();
}

void WSerialWorker::slotOpenSerialPort(const QString &portName, int baudRate)
{
    if (!portName.isEmpty() && m_serialPort->isOpen())
    {
        m_serialPort->close();
    }

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    bool success = m_serialPort->open(QIODevice::ReadWrite);
    emit sigPortOpened(success,(int)m_type);

    WLog::getInstance()->info(QString("[serial:%4] open %1:%2 %3!")
                              .arg(portName).arg(baudRate)
                              .arg(success?"success":"failed").arg(getSerialNameByType()));
}

void WSerialWorker::slotCloseSerialPort()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }

    WLog::getInstance()->info(QString("[serial:%4] close %1:%2!")
                              .arg(m_serialPort->portName()).arg(m_serialPort->baudRate()).arg(getSerialNameByType()));
}
//    void WSerialWorker::writedata(const QByteArray &data)
//    {
//        if (m_serialPort->isOpen())
//        {
//            m_serialPort->write(data);
//        }

//    }

void WSerialWorker::slotWriteData(const QByteArray &data)
{
    WLog::getInstance()->info(QString("[serial:%1,%2,%3] send data %4")
                              .arg(m_serialPort->portName()).arg(m_serialPort->baudRate())
                              .arg(getSerialNameByType()).arg(data.toHex().data()));
    if (m_serialPort->isOpen())
    {
        m_serialPort->write(data);

        WLog::getInstance()->debug(QString("[serial:%1,%2,%3] send data %4")
                                   .arg(m_serialPort->portName()).arg(m_serialPort->baudRate())
                                   .arg(getSerialNameByType()).arg(data.toHex().data()));
        qDebug()<<"serial send "<<data.toHex();
    }
}

void WSerialWorker::slotHandleReadyRead()
{
    //emit sigDataReceived(data,(int)m_type);

    QByteArray data = m_serialPort->readAll();
    processData(data);

    WLog::getInstance()->debug(QString("[serial:%4] recv data %1:%2 msg:%3")
                               .arg(m_serialPort->portName()).arg(m_serialPort->baudRate())
                               .arg(data.toHex().data()).arg(getSerialNameByType()));
}

void WSerialWorker::slotHandleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        emit sigErrorOccurred(m_serialPort->errorString(),(int)m_type);

        WLog::getInstance()->info(QString("[serial:%4] error %1:%2 errmsg:%3!")
                                  .arg(m_serialPort->portName()).arg(m_serialPort->baudRate())
                                  .arg(m_serialPort->errorString()).arg(getSerialNameByType()));
    }
}

void WSerialWorker::processData(const QByteArray &data)
{
    m_array.append(data);

    if(m_type == EnumStA0188)
    {

    }
    else if(m_type == EnumStGenlock)
    {
//        if(m_array.size() < sizeof(Protocol::Cloud2Ret))
//        {
//            return;
//        }
//        while(m_array.size() >= sizeof(Protocol::Cloud2Ret))
//        {
//            Protocol::Cloud2Ret* tmp = (Protocol::Cloud2Ret*)m_array.data();
//            if(tmp->byte1 != 0x08 || tmp->byte2 != 0x00 || tmp->byte3 != 0x00)
//            {
//                //qDebug("remove tmp->head = %x",tmp->head);
//                if(!m_array.isEmpty())
//                    m_array.remove(0,1);
//                continue;
//            }

//            QByteArray byteArray = m_array.mid(0,sizeof(Protocol::Cloud2Ret));
//            m_array = m_array.mid(sizeof(Protocol::Cloud2Ret));
//            emit sigDataReceived(byteArray,(int)m_type);
//        }
    }
}

QString WSerialWorker::getSerialNameByType()
{
    switch(m_type)
    {
    case EnumStA0188: //主控箱
        return "A0188模组";
    case EnumStGenlock:   //遥控箱
        return "Genlock";
    default:
        return "未知串口";
    }
}
