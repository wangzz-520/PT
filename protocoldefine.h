#ifndef PROTOCOLDEFINE_H
#define PROTOCOLDEFINE_H

#include <qglobal.h>
#include <QMap>


#pragma pack(1)


namespace Protocol
{
    //************Genlock************
    //## 串口配置
    //- **波特率**: 115200
    //- **数据格式**: 8N1 (8位数据位，无校验位，1位停止位)
    //- **协议版本**: v1.1 (VER=0x11)
    //每次检测到同步脉冲边沿时发送

    //**健康标志位 (healthFlags)**:
    //- Bit 0: 输入信号丢失 (1=丢失)
    //- Bit 1: 频率不稳定 (±0.3%阈值)
    //- Bit 4: 脉冲宽度过窄

    //## 帧率代码表 fpsCode

    //| 代码 | 帧率 | 周期 (ns) |
    //|------|------|-----------|
    //| 0x01 | 23.976 fps | 41,708,333.33 |
    //| 0x02 | 24 fps | 41,666,666.67 |
    //| 0x03 | 25 fps | 40,000,000.00 |
    //| 0x04 | 29.97 fps | 33,366,666.67 |
    //| 0x05 | 30 fps | 33,333,333.33 |
    //| 0x06 | 48 fps | 20,833,333.33 |
    //| 0x07 | 50 fps | 20,000,000.00 |
    //| 0x08 | 59.94 fps | 16,683,333.33 |
    //| 0x09 | 60 fps | 16,666,666.67 |
    //| 0x0A | 100 fps | 10,000,000.00 |
    //| 0x0B | 120 fps | 8,333,333.33 |
    //| 0x0C | 144 fps | 6,944,444.44 |


    //设备通过以下逻辑判断锁定状态：
    //1. **锁定条件**:
    //   - 窗口抖动P95 ≤ min(1.05 * 周期/1000, 3500) 纳秒
    //   - 连续10个稳定窗口

    //2. **解锁条件**:
    //   - 窗口抖动P95 > 阈值
    //   - 连续5个不稳定窗口

    typedef struct
    {
        quint16 header; //帧头 固定值 0xAA 0x55
        quint8 version; //版本`0x11` (v1.1)
        quint16 len;    //Big-Endian (BE) 16位无符号整数 载荷长度（不包括帧头、版本、长度、CRC）
        quint8 msgType = 0x01; //类型
        quint16 seq;    //序列号，递增
        quint64 deviceMonoNs;   //Big-Endian (BE) 设备单调时钟纳秒
        quint8 edgePolarity;    //边沿极性:0=FALLING, 1=RISING
        quint64 pulseMonoNs;    //Big-Endian (BE) 脉冲时间戳（纳秒）
        quint64 periodNs;       //Big-Endian (BE) 周期（纳秒）
        quint8 fpsCode;         //帧率代码（见帧率代码表）
        quint16 healthFlags;    //Big-Endian (BE) 健康标志位
        qint32 jitterNs;        //Big-Endian (BE) 抖动（纳秒，有符号）
        quint32 windowJitterNsP95;  //Big-Endian (BE) 窗口抖动P95（纳秒）
        quint8 qualityScore;        //质量分数 (0-100)
        quint16 crc;                //VER + LEN(2字节) + PAYLOAD(载荷) CRC16使用Little-Endian
    }GenLockPluse;

    //每1秒发送一次心跳包。
    typedef struct
    {
        quint16 header; //帧头 固定值 0xAA 0x55
        quint8 version; //版本`0x11` (v1.1)
        quint16 len;    //Big-Endian (BE) 16位无符号整数 载荷长度（不包括帧头、版本、长度、CRC）
        quint8 msgType = 0x02; //类型
        quint16 seq;    //序列号，递增
        quint64 deviceMonoNs;   //Big-Endian (BE) 设备单调时钟纳秒
        quint16 healthFlags;    //Big-Endian (BE) 健康标志位
        quint8 lockState;       //锁定状态: 0=UNLOCKED, 1=LOCKED
        quint8 fpsCode;         //帧率代码（见帧率代码表）
        quint32 windowJitterNsP95;  //Big-Endian (BE) 窗口抖动P95（纳秒）
        quint32 uptimeSec;          //Big-Endian (BE) 运行时间（秒）
        quint16 crc;                //VER + LEN(2字节) + PAYLOAD(载荷) CRC16使用Little-Endian
    }GenLockHeartBeat;

    //每2秒发送一次状态包。
    typedef struct
    {
        quint16 header; //帧头 固定值 0xAA 0x55
        quint8 version; //版本`0x11` (v1.1)
        quint16 len;    //Big-Endian (BE) 16位无符号整数 载荷长度（不包括帧头、版本、长度、CRC）
        quint8 msgType = 0x03; //类型
        quint16 seq;    //序列号，递增
        quint64 deviceMonoNs;   //Big-Endian (BE) 设备单调时钟纳秒
        quint16 healthFlags;    //Big-Endian (BE) 健康标志位
        quint8 fpsCode;         //帧率代码（见帧率代码表）
        quint64 periodNsNominal;    //Big-Endian (BE) 标称周期（纳秒）
        quint64 periodNsAvg;        //Big-Endian (BE) 平均周期（纳秒）
        quint64 periodNsP95;        //Big-Endian (BE) 周期P95（纳秒）
        quint32 missedPulses;       //Big-Endian (BE) 丢失脉冲计数
        quint32 glitchCount;        //Big-Endian (BE) 毛刺计数
        quint16 crc;                //VER + LEN(2字节) + PAYLOAD(载荷) CRC16使用Little-Endian
    }GenLockStatus;

//************Genlock************

}


#pragma pack()



#endif // PROTOCOLDEFINE_H
