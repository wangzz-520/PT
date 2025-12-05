#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <QString>
#include <Eigen/Dense>
#include <Eigen/Geometry>


//串口类型
enum SerialType
{
    EnumStUnKnown,
    EnumStA0188,
    EnumStGenlock
};

enum MotionState {
    STATIC,   // 静止状态
    MOVING,   // 运动状态
    DYNAMIC   // 剧烈运动状态
};

// 信号强度级别（0=无信号，4=满信号）
enum SignalLevel {
    Level0 = 0,
    Level1 = 1,
    Level2 = 2,
    Level3 = 3,
    Level4 = 4,
    Level5 = 5
};

//PairedFrame（包含position, rotation, IMU data, confidence, mode）
struct PairedFrame {
    Eigen::Vector3d position;           // 位置 (m)
    Eigen::Quaterniond rotation;        // 旋转 (输入WXYZ，内部XYZW)
    Eigen::Vector3d acceleration;       // 加速度 (m/s²)
    Eigen::Vector3d angular_velocity;   // 角速度 (rad/s)
    int64_t timestamp_ns;               // 时间戳 (ns)
    double confidence;                  // 置信度 (0.0-1.0)
    //WorkMode work_mode;               // 工作模式 (VIO/Map)
    // ... 其他字段见标准定义
};

// 注意：这是简化版本，完整定义请参考总体文档
struct StabilizedFrame {
    Eigen::Vector3d position;           // 增稳后的位置 (m)
    Eigen::Quaterniond rotation;        // 旋转（直接传递，不滤波）
    int64_t timestamp_ns;               // 时间戳 (ns)
    MotionState motion_state;           // 运动状态（STATIC/MOVING/DYNAMIC）
    Eigen::Vector3d acceleration;       // 加速度 (m/s²)
    Eigen::Vector3d angular_velocity;   // 角速度 (rad/s)
    double confidence;                  // 置信度 (0.0-1.0)
    // ... 其他字段见标准定义
};

//首页展示数据
struct OverviewInfo
{
    QString protocal;       //协议类型 FreeD/
    QString freq;           //发送频率 60Hz
    QString dstIp;          //目标IP
    QString dstPort;        //目标端口
    QString dstId;          //目标相机ID
    QString encodeZoom;     //编码器Zoom
    QString encodeFocus = "N/A"; //编码器Focus
};


struct PcInfo
{
    QString cpu;
    QString temp;
    QString mem;
    QString Ip;
};

#endif // GLOBAL_H
