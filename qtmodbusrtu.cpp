#include "QtModbusRTU.h"
#include <QModbusReply>
#include <QDebug>
#include <QEventLoop>

//// 创建 ModbusRTU 实例
//ModbusRTU modbusRTU;

//// 连接信号槽（处理结果和错误）
//QObject::connect(&modbusRTU, &ModbusRTU::readBitsSuccess, [](quint8 slaveAddr, quint16 startAddr, const QList<bool> &states) {
//    qDebug() << QString("读线圈/离散输入成功 - 从机%1，起始地址%2，数量%3").arg(slaveAddr).arg(startAddr).arg(states.size());
//    for (int i = 0; i < states.size(); ++i) {
//        qDebug() << QString("地址%1: %2").arg(startAddr + i).arg(states.at(i) ? "ON" : "OFF");
//    }
//});

//QObject::connect(&modbusRTU, &ModbusRTU::readRegistersSuccess, [](quint8 slaveAddr, quint16 startAddr, const QList<quint16> &values) {
//    qDebug() << QString("读寄存器成功 - 从机%1，起始地址%2，数量%3").arg(slaveAddr).arg(startAddr).arg(values.size());
//    for (int i = 0; i < values.size(); ++i) {
//        qDebug() << QString("地址%1: 0x%2（十进制：%3）").arg(startAddr + i).arg(values.at(i), 4, 16, QChar('0')).arg(values.at(i));
//    }
//});

//QObject::connect(&modbusRTU, &ModbusRTU::writeSuccess, [](quint8 slaveAddr, quint16 startAddr, quint16 count) {
//    qDebug() << QString("写入成功 - 从机%1，起始地址%2，数量%3").arg(slaveAddr).arg(startAddr).arg(count);
//});

//QObject::connect(&modbusRTU, &ModbusRTU::errorOccurred, [](quint8 errorCode, const QString &errorMsg) {
//    qDebug() << QString("错误 - 代码%1：%2").arg(errorCode).arg(errorMsg);
//});

//// 初始化串口（根据实际设备修改串口名）
//#ifdef Q_OS_WIN
//bool initOk = modbusRTU.initSerial("COM3", QSerialPort::Baud9600); // Windows 串口
//#else
//bool initOk = modbusRTU.initSerial("/dev/ttyUSB0", QSerialPort::Baud9600); // Linux 串口
//#endif

QtModbusRTU::QtModbusRTU(QObject *parent)
    : QObject(parent)
    , m_modbusDevice(nullptr)
    , m_responseTimeout(3000)
    , m_numberOfRetries(3)
    , m_interFrameDelay(10)
    , m_lastError(NoError)
    , m_currentRetryCount(0)
    , m_batchMode(false)
    , m_batchTotal(0)
    , m_batchCurrent(0)
{
    m_modbusDevice = new QModbusRtuSerialMaster(this);

    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &QtModbusRTU::onRequestTimeout);

    m_retryTimer = new QTimer(this);
    m_retryTimer->setSingleShot(true);

    connect(m_modbusDevice, &QModbusClient::stateChanged,
            this, &QtModbusRTU::onModbusStateChanged);
    connect(m_modbusDevice, &QModbusClient::errorOccurred,
            this, &QtModbusRTU::onModbusErrorOccurred);
}

QtModbusRTU::~QtModbusRTU()
{
    disconnectDevice();
}

bool QtModbusRTU::connectDevice(const QString &portName,
                                QSerialPort::BaudRate baudRate,
                                QSerialPort::DataBits dataBits,
                                QSerialPort::Parity parity,
                                QSerialPort::StopBits stopBits)
{
    if (!m_modbusDevice) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Modbus device not initialized";
        return false;
    }

    if (m_modbusDevice->state() != QModbusDevice::UnconnectedState) {
        m_modbusDevice->disconnectDevice();
    }

    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, portName);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudRate);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, dataBits);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, parity);
    m_modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, stopBits);

    m_modbusDevice->setTimeout(m_responseTimeout);
    m_modbusDevice->setNumberOfRetries(m_numberOfRetries);

    if (!m_modbusDevice->connectDevice()) {
        m_lastError = ConnectionError;
        m_lastErrorString = m_modbusDevice->errorString();
        return false;
    }

    return true;
}

void QtModbusRTU::disconnectDevice()
{
    if (m_modbusDevice && m_modbusDevice->state() != QModbusDevice::UnconnectedState) {
        m_modbusDevice->disconnectDevice();
    }
}

bool QtModbusRTU::isConnected() const
{
    return m_modbusDevice &&
           m_modbusDevice->state() == QModbusDevice::ConnectedState;
}

bool QtModbusRTU::readHoldingRegisters(int slaveAddress, int startAddress,
                                      quint16 numberOfRegisters, QVector<quint16> &results,
                                      int timeout)
{
    if (!isConnected()) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Not connected to device";
        return false;
    }

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfRegisters);

    if (auto *reply = m_modbusDevice->sendReadRequest(readUnit, slaveAddress)) {
        if (!reply->isFinished()) {
            if (waitForReply(reply, timeout)) {
                if (reply->error() == QModbusDevice::NoError) {
                    const QModbusDataUnit unit = reply->result();
                    results = unit.values();
                    m_lastError = NoError;
                    cleanupReply(reply);
                    return true;
                }
            }
        }
        m_lastError = ProtocolError;
        m_lastErrorString = reply->errorString();
        cleanupReply(reply);
    } else {
        m_lastError = ProtocolError;
        m_lastErrorString = "Failed to send read request";
    }

    return false;
}

bool QtModbusRTU::readInputRegisters(int slaveAddress, int startAddress,
                                    quint16 numberOfRegisters, QVector<quint16> &results,
                                    int timeout)
{
    if (!isConnected()) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Not connected to device";
        return false;
    }

    QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters, startAddress, numberOfRegisters);

    if (auto *reply = m_modbusDevice->sendReadRequest(readUnit, slaveAddress)) {
        if (!reply->isFinished()) {
            if (waitForReply(reply, timeout)) {
                if (reply->error() == QModbusDevice::NoError) {
                    const QModbusDataUnit unit = reply->result();
                    results = unit.values();
                    m_lastError = NoError;
                    cleanupReply(reply);
                    return true;
                }
            }
        }
        m_lastError = ProtocolError;
        m_lastErrorString = reply->errorString();
        cleanupReply(reply);
    } else {
        m_lastError = ProtocolError;
        m_lastErrorString = "Failed to send read request";
    }

    return false;
}

bool QtModbusRTU::readCoils(int slaveAddress, int startAddress,
                           quint16 numberOfCoils, QVector<quint16> &results,
                           int timeout)
{
    if (!isConnected()) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Not connected to device";
        return false;
    }

    QModbusDataUnit readUnit(QModbusDataUnit::Coils, startAddress, numberOfCoils);

    if (auto *reply = m_modbusDevice->sendReadRequest(readUnit, slaveAddress)) {
        if (!reply->isFinished()) {
            if (waitForReply(reply, timeout)) {
                if (reply->error() == QModbusDevice::NoError) {
                    const QModbusDataUnit unit = reply->result();
                    results = unit.values();
                    m_lastError = NoError;
                    cleanupReply(reply);
                    return true;
                }
            }
        }
        m_lastError = ProtocolError;
        m_lastErrorString = reply->errorString();
        cleanupReply(reply);
    } else {
        m_lastError = ProtocolError;
        m_lastErrorString = "Failed to send read request";
    }

    return false;
}

bool QtModbusRTU::readDiscreteInputs(int slaveAddress, int startAddress,
                                    quint16 numberOfInputs, QVector<quint16> &results,
                                    int timeout)
{
    if (!isConnected()) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Not connected to device";
        return false;
    }

    QModbusDataUnit readUnit(QModbusDataUnit::DiscreteInputs, startAddress, numberOfInputs);

    if (auto *reply = m_modbusDevice->sendReadRequest(readUnit, slaveAddress)) {
        if (!reply->isFinished()) {
            if (waitForReply(reply, timeout)) {
                if (reply->error() == QModbusDevice::NoError) {
                    const QModbusDataUnit unit = reply->result();
                    results = unit.values();
                    m_lastError = NoError;
                    cleanupReply(reply);
                    return true;
                }
            }
        }
        m_lastError = ProtocolError;
        m_lastErrorString = reply->errorString();
        cleanupReply(reply);
    } else {
        m_lastError = ProtocolError;
        m_lastErrorString = "Failed to send read request";
    }

    return false;
}

bool QtModbusRTU::writeSingleRegister(int slaveAddress, int address, quint16 value, int timeout)
{
    if (!isConnected()) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Not connected to device";
        return false;
    }

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, address, 1);
    writeUnit.setValue(0, value);

    if (auto *reply = m_modbusDevice->sendWriteRequest(writeUnit, slaveAddress)) {
        if (!reply->isFinished()) {
            if (waitForReply(reply, timeout)) {
                if (reply->error() == QModbusDevice::NoError) {
                    m_lastError = NoError;
                    cleanupReply(reply);
                    return true;
                }
            }
        }
        m_lastError = ProtocolError;
        m_lastErrorString = reply->errorString();
        cleanupReply(reply);
    } else {
        m_lastError = ProtocolError;
        m_lastErrorString = "Failed to send write request";
    }

    return false;
}

bool QtModbusRTU::writeMultipleRegisters(int slaveAddress, int startAddress,
                                        const QVector<quint16> &values, int timeout)
{
    if (!isConnected()) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Not connected to device";
        return false;
    }

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, startAddress, values.size());
    for (int i = 0; i < values.size(); ++i) {
        writeUnit.setValue(i, values[i]);
    }

    if (auto *reply = m_modbusDevice->sendWriteRequest(writeUnit, slaveAddress)) {
        if (!reply->isFinished()) {
            if (waitForReply(reply, timeout)) {
                if (reply->error() == QModbusDevice::NoError) {
                    m_lastError = NoError;
                    cleanupReply(reply);
                    return true;
                }
            }
        }
        m_lastError = ProtocolError;
        m_lastErrorString = reply->errorString();
        cleanupReply(reply);
    } else {
        m_lastError = ProtocolError;
        m_lastErrorString = "Failed to send write request";
    }

    return false;
}

bool QtModbusRTU::writeSingleCoil(int slaveAddress, int address, bool value, int timeout)
{
    if (!isConnected()) {
        m_lastError = ConnectionError;
        m_lastErrorString = "Not connected to device";
        return false;
    }

    QModbusDataUnit writeUnit(QModbusDataUnit::Coils, address, 1);
    writeUnit.setValue(0, value ? 0xFF00 : 0x0000);

    if (auto *reply = m_modbusDevice->sendWriteRequest(writeUnit, slaveAddress)) {
        if (!reply->isFinished()) {
            if (waitForReply(reply, timeout)) {
                if (reply->error() == QModbusDevice::NoError) {
                    m_lastError = NoError;
                    cleanupReply(reply);
                    return true;
                }
            }
        }
        m_lastError = ProtocolError;
        m_lastErrorString = reply->errorString();
        cleanupReply(reply);
    } else {
        m_lastError = ProtocolError;
        m_lastErrorString = "Failed to send write request";
    }

    return false;
}

void QtModbusRTU::asyncReadHoldingRegisters(int slaveAddress, int startAddress,
                                           quint16 numberOfRegisters, int timeout)
{
    ModbusRequest request;
    request.slaveAddress = slaveAddress;
    request.registerType = QModbusDataUnit::HoldingRegisters;
    request.startAddress = startAddress;
    request.numberOfValues = numberOfRegisters;
    request.timeout = timeout;
    request.retryCount = m_numberOfRetries;

    addReadRequest(request);
}

void QtModbusRTU::asyncReadInputRegisters(int slaveAddress, int startAddress,
                                         quint16 numberOfRegisters, int timeout)
{
    ModbusRequest request;
    request.slaveAddress = slaveAddress;
    request.registerType = QModbusDataUnit::InputRegisters;
    request.startAddress = startAddress;
    request.numberOfValues = numberOfRegisters;
    request.timeout = timeout;
    request.retryCount = m_numberOfRetries;

    addReadRequest(request);
}

void QtModbusRTU::asyncWriteSingleRegister(int slaveAddress, int address,
                                          quint16 value, int timeout)
{
    ModbusRequest request;
    request.slaveAddress = slaveAddress;
    request.registerType = QModbusDataUnit::HoldingRegisters;
    request.startAddress = address;
    request.numberOfValues = 1;
    request.values.append(value);
    request.timeout = timeout;
    request.retryCount = m_numberOfRetries;

    QMutexLocker locker(&m_queueMutex);
    m_requestQueue.enqueue(request);

    if (!m_batchMode) {
        processNextRequest();
    }
}

void QtModbusRTU::asyncWriteMultipleRegisters(int slaveAddress, int startAddress,
                                             const QVector<quint16> &values, int timeout)
{
    ModbusRequest request;
    request.slaveAddress = slaveAddress;
    request.registerType = QModbusDataUnit::HoldingRegisters;
    request.startAddress = startAddress;
    request.numberOfValues = values.size();
    request.values = values;
    request.timeout = timeout;
    request.retryCount = m_numberOfRetries;

    QMutexLocker locker(&m_queueMutex);
    m_requestQueue.enqueue(request);

    if (!m_batchMode) {
        processNextRequest();
    }
}

void QtModbusRTU::addReadRequest(const ModbusRequest &request)
{
    QMutexLocker locker(&m_queueMutex);
    m_requestQueue.enqueue(request);

    if (!m_batchMode) {
        processNextRequest();
    }
}

void QtModbusRTU::executeBatch(int batchTimeout)
{
    m_batchMode = true;
    m_batchTotal = m_requestQueue.size();
    m_batchCurrent = 0;

    emit batchProgress(m_batchCurrent, m_batchTotal);

    processNextRequest();
}

void QtModbusRTU::setResponseTimeout(int timeout)
{
    m_responseTimeout = timeout;
    if (m_modbusDevice) {
        m_modbusDevice->setTimeout(timeout);
    }
}

void QtModbusRTU::setNumberOfRetries(int retries)
{
    m_numberOfRetries = retries;
    if (m_modbusDevice) {
        m_modbusDevice->setNumberOfRetries(retries);
    }
}

void QtModbusRTU::setInterFrameDelay(int delayMs)
{
    m_interFrameDelay = delayMs;
}

QtModbusRTU::ModbusError QtModbusRTU::lastError() const
{
    return m_lastError;
}

QString QtModbusRTU::errorString() const
{
    return m_lastErrorString;
}

bool QtModbusRTU::waitForReply(QModbusReply *reply, int timeout)
{
    QEventLoop loop;
    QObject::connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(timeout);
    loop.exec();

    return timer.isActive();
}

void QtModbusRTU::processNextRequest()
{
    if (m_requestQueue.isEmpty()) {
        if (m_batchMode) {
            m_batchMode = false;
            emit batchCompleted();
        }
        return;
    }

    QMutexLocker locker(&m_queueMutex);
    m_currentRequest = m_requestQueue.dequeue();
    m_currentRetryCount = 0;

    if (m_batchMode) {
        m_batchCurrent++;
        emit batchProgress(m_batchCurrent, m_batchTotal);
    }

    QModbusReply *reply = nullptr;

    if (m_currentRequest.values.isEmpty()) {
        reply = createReadRequest(m_currentRequest);
    } else {
        reply = createWriteRequest(m_currentRequest);
    }

    if (reply) {
        m_pendingRequests[reply] = m_currentRequest;
        connect(reply, &QModbusReply::finished, this, &QtModbusRTU::onModbusReplyFinished);
        m_timeoutTimer->start(m_currentRequest.timeout);
    } else {
        processNextRequest();
    }
}

QModbusReply* QtModbusRTU::createReadRequest(const ModbusRequest &request)
{
    if (!m_modbusDevice || !isConnected()) {
        return nullptr;
    }

    QModbusDataUnit dataUnit(request.registerType, request.startAddress, request.numberOfValues);
    return m_modbusDevice->sendReadRequest(dataUnit, request.slaveAddress);
}

QModbusReply* QtModbusRTU::createWriteRequest(const ModbusRequest &request)
{
    if (!m_modbusDevice || !isConnected()) {
        return nullptr;
    }

    QModbusDataUnit dataUnit(request.registerType, request.startAddress, request.numberOfValues);
    for (int i = 0; i < request.values.size() && i < request.numberOfValues; ++i) {
        dataUnit.setValue(i, request.values[i]);
    }

    return m_modbusDevice->sendWriteRequest(dataUnit, request.slaveAddress);
}

void QtModbusRTU::onModbusReplyFinished()
{
    QModbusReply *reply = qobject_cast<QModbusReply *>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }

    m_timeoutTimer->stop();

    ModbusRequest request = m_pendingRequests.take(reply);

    if (reply->error() == QModbusDevice::NoError) {
        if (request.values.isEmpty()) {
            // 读取操作
            const QModbusDataUnit unit = reply->result();
            emit readCompleted(request.slaveAddress, request.registerType,
                              request.startAddress, unit.values());
        } else {
            // 写入操作
            emit writeCompleted(request.slaveAddress, request.registerType,
                               request.startAddress, request.numberOfValues);
        }

        QThread::msleep(m_interFrameDelay);
        processNextRequest();
    } else {
        if (m_currentRetryCount < request.retryCount) {
            m_currentRetryCount++;
            m_retryTimer->start(100);
            QTimer::singleShot(100, this, [this, request]() {
                QMutexLocker locker(&m_queueMutex);
                m_requestQueue.prepend(request);
                processNextRequest();
            });
        } else {
            emit operationError(request.slaveAddress, request.registerType,
                              request.startAddress, ProtocolError);
            processNextRequest();
        }
    }

    cleanupReply(reply);
}

void QtModbusRTU::onModbusStateChanged(QModbusDevice::State state)
{
    switch (state) {
    case QModbusDevice::UnconnectedState:
        emit disconnected();
        break;
    case QModbusDevice::ConnectedState:
        emit connected();
        break;
    case QModbusDevice::ConnectingState:
        break;
    case QModbusDevice::ClosingState:
        break;
    }
}

void QtModbusRTU::onModbusErrorOccurred(QModbusDevice::Error error)
{
    if (error != QModbusDevice::NoError) {
        m_lastError = ProtocolError;
        m_lastErrorString = m_modbusDevice->errorString();
        emit connectionError(m_lastErrorString);
    }
}

void QtModbusRTU::onRequestTimeout()
{
    // 超时处理
    if (!m_pendingRequests.isEmpty()) {
        QModbusReply *reply = m_pendingRequests.keys().first();
        if (reply) {
            //reply->abort();
            ModbusRequest request = m_pendingRequests.take(reply);
            emit operationError(request.slaveAddress, request.registerType,
                              request.startAddress, TimeoutError);
            cleanupReply(reply);
        }
    }

    processNextRequest();
}

void QtModbusRTU::cleanupReply(QModbusReply *reply)
{
    if (reply) {
        reply->deleteLater();
    }
}
