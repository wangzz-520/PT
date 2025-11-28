#ifndef WSERIALWORKER_H
#define WSERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include "global.h"

class WSerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit WSerialWorker(Global::SerialType type,QObject *parent = nullptr);
     ~WSerialWorker();

signals:
    //收到数据
    void sigDataReceived(const QByteArray &data,int type);

    //有错误发生
    void sigErrorOccurred(const QString &error,int type);

    //串口打开
    void sigPortOpened(bool success,int type);

public slots:
    //打开串口 串口名、波特率
    void slotOpenSerialPort(const QString &portName, int baudRate);

    //关闭串口
    void slotCloseSerialPort();

    //写数据
    void slotWriteData(const QByteArray &data);

private slots:
    void slotHandleReadyRead();
    void slotHandleError(QSerialPort::SerialPortError error);

private:
    void processData(const QByteArray &data);
    QString getSerialNameByType();

private:
    QSerialPort *m_serialPort = nullptr;
    Global::SerialType m_type;

    QByteArray m_array;
};

#endif // WSERIALWORKER_H
