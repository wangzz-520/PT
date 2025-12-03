#ifndef QTMODBUSRTU_H
#define QTMODBUSRTU_H

#include <QObject>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QThread>

struct ModbusRequest {
    int slaveAddress;
    QModbusDataUnit::RegisterType registerType;
    int startAddress;
    quint16 numberOfValues;
    QVector<quint16> values; // 用于写操作
    int timeout;
    int retryCount;
};

class QtModbusRTU : public QObject
{
    Q_OBJECT

public:
    enum ModbusError {
        NoError = 0,
        ConnectionError,
        TimeoutError,
        ProtocolError,
        SerialPortError,
        BusyError
    };

    explicit QtModbusRTU(QObject *parent = nullptr);
    ~QtModbusRTU();

    // 连接管理
    bool connectDevice(const QString &portName,
                       QSerialPort::BaudRate baudRate = QSerialPort::Baud115200,
                       QSerialPort::DataBits dataBits = QSerialPort::Data8,
                       QSerialPort::Parity parity = QSerialPort::NoParity,
                       QSerialPort::StopBits stopBits = QSerialPort::OneStop);

    void disconnectDevice();
    bool isConnected() const;

    // 同步读取（阻塞）读保持寄存器
    bool readHoldingRegisters(int slaveAddress, int startAddress,
                             quint16 numberOfRegisters, QVector<quint16> &results,
                             int timeout = 3000);

    /**
    * @brief 读输入寄存器（功能码 0x04）
    * @param slaveAddr 从机地址
    * @param startAddr 起始地址
    * @param numberOfRegisters 寄存器数量（1-125）
    * @return 是否发送成功
    */
    bool readInputRegisters(int slaveAddress, int startAddress,
                           quint16 numberOfRegisters, QVector<quint16> &results,
                           int timeout = 3000);

    /**
     * @brief 读线圈状态（功能码 0x01）
     * @param slaveAddr 从机地址（1-247）
     * @param startAddr 起始地址（0-65535）
     * @param coilCount 线圈数量（1-2000）
     * @return 是否发送成功（成功不代表从机响应成功，结果通过 signal 返回）
     */
    bool readCoils(int slaveAddress, int startAddress,
                  quint16 numberOfCoils, QVector<quint16> &results,
                  int timeout = 3000);

    /**
    * @brief 读离散输入（功能码 0x02）
    * @param slaveAddr 从机地址
    * @param startAddr 起始地址
    * @param numberOfInputs 输入数量（1-2000）
    * @return 是否发送成功
    */
    bool readDiscreteInputs(int slaveAddress, int startAddress,
                           quint16 numberOfInputs, QVector<quint16> &results,
                           int timeout = 3000);

    // 异步读取（非阻塞）
    void asyncReadHoldingRegisters(int slaveAddress, int startAddress,
                                  quint16 numberOfRegisters, int timeout = 3000);

    void asyncReadInputRegisters(int slaveAddress, int startAddress,
                                quint16 numberOfRegisters, int timeout = 3000);

    // 同步写入（阻塞）

    bool writeSingleRegister(int slaveAddress, int address, quint16 value, int timeout = 3000);
    bool writeMultipleRegisters(int slaveAddress, int startAddress,
                               const QVector<quint16> &values, int timeout = 3000);

    /**
    * @brief 写单个线圈（功能码 0x05）
    * @param slaveAddr 从机地址
    * @param coilAddr 线圈地址
    * @param state 线圈状态（true=ON(0xFF00), false=OFF(0x0000)）
    * @return 是否发送成功
    */
    bool writeSingleCoil(int slaveAddress, int address, bool value, int timeout = 3000);


    // 异步写入（非阻塞）
    void asyncWriteSingleRegister(int slaveAddress, int address, quint16 value, int timeout = 3000);
    void asyncWriteMultipleRegisters(int slaveAddress, int startAddress,
                                    const QVector<quint16> &values, int timeout = 3000);

    // 批量操作
    void addReadRequest(const ModbusRequest &request);
    void executeBatch(int batchTimeout = 5000);

    // 配置
    void setResponseTimeout(int timeout);
    void setNumberOfRetries(int retries);
    void setInterFrameDelay(int delayMs);

    // 获取错误信息
    ModbusError lastError() const;
    QString errorString() const;

signals:
    // 连接状态信号
    void connected();
    void disconnected();
    void connectionError(const QString &error);

    // 异步操作完成信号
    void readCompleted(int slaveAddress, QModbusDataUnit::RegisterType type,
                      int startAddress, const QVector<quint16> &values);

    void writeCompleted(int slaveAddress, QModbusDataUnit::RegisterType type,
                       int startAddress, int numberOfValues);

    void operationError(int slaveAddress, QModbusDataUnit::RegisterType type,
                       int startAddress, ModbusError error);

    // 批量操作完成信号
    void batchCompleted();
    void batchProgress(int current, int total);

private slots:
    void onModbusReplyFinished();
    void onModbusStateChanged(QModbusDevice::State state);
    void onModbusErrorOccurred(QModbusDevice::Error error);
    void onRequestTimeout();

private:
    // 私有方法
    bool waitForReply(QModbusReply *reply, int timeout);
    QModbusReply* createReadRequest(const ModbusRequest &request);
    QModbusReply* createWriteRequest(const ModbusRequest &request);
    void processNextRequest();
    void cleanupReply(QModbusReply *reply);

    // 私有成员
    QModbusRtuSerialMaster *m_modbusDevice;
    QQueue<ModbusRequest> m_requestQueue;
    QMutex m_queueMutex;

    QTimer *m_timeoutTimer;
    QTimer *m_retryTimer;

    int m_responseTimeout;
    int m_numberOfRetries;
    int m_interFrameDelay;

    ModbusError m_lastError;
    QString m_lastErrorString;

    QMap<QModbusReply*, ModbusRequest> m_pendingRequests;
    ModbusRequest m_currentRequest;
    int m_currentRetryCount;

    bool m_batchMode;
    int m_batchTotal;
    int m_batchCurrent;
};

#endif // QTMODBUSRTU_H
