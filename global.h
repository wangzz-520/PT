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

typedef enum class _ExposureMode
{
    AUTO,   // 自动曝光模式
    MANUAL  // 手动曝光模式
}ExposureMode;

typedef enum class _LogLevel{
    LogLevel_debug = 1,
    LogLevel_info,
    LogLevel_warn,
    LogLevel_err,
    LogLevel_critical,
    LogLevel_off
}LogLevel;

typedef struct _MappingQuality
{
    double overall_score;      // 总体质量分数（0.0-1.0）
    double confidence_avg;     // 平均置信度
    double confidence_min;      // 最小置信度
    double confidence_max;      // 最大置信度
    double stability_score;    // 稳定性分数
    int64_t analysis_duration; // 分析时长（纳秒）
}MappingQuality;

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

typedef enum class _WorkMode
{
    VIO,           // VIO模式：实时SLAM位姿估计
    MAP,           // 地图模式：基于预建地图定位
    MAP_INIT_VIO   // 混合模式：初始使用地图获得坐标，然后切换到VIO模式
}WorkMode;


//PairedFrame（包含position, rotation, IMU data, confidence, mode）
//数据采集模块输出
typedef struct _PairedFrame
{
    bool valid_frame;
    // POSE数据
    Eigen::Vector3d position;          // 位置 [x, y, z] (m)
    Eigen::Quaterniond rotation;        // 旋转四元数 (输入WXYZ，内部XYZW，w≥0，已归一化)
    Eigen::Vector3d linear_velocity;    // 线速度 [vx, vy, vz] (m/s)
    Eigen::Vector3d angular_velocity;   // 角速度 [ωx, ωy, ωz] (rad/s)
    int64_t timestamp_ns;               // POSE时间戳 (纳秒)
    double confidence;                  // POSE置信度 (0.0-1.0)

    // IMU数据
    Eigen::Vector3d imu_angular_velocity;  // IMU角速度 [ωx, ωy, ωz] (rad/s)
    Eigen::Vector3d imu_acceleration;       // IMU加速度 [ax, ay, az] (m/s²)
    int64_t imu_timestamp_ns;              // IMU时间戳 (纳秒)

    // 配对信息
    double pair_delta_us;          // 配对时间差 (微秒)
    double pair_quality;           // 配对质量 (0.0-1.0)

    // 元数据
    uint32_t frame_id;             // 帧序号
    std::string source;            // 数据源标识（"DS80"或"A0188"）
    WorkMode work_mode;            // 工作模式 (VIO/MAP/MAP_INIT_VIO)
    bool map_loaded;               // 地图加载状态

    // 第二期扩展
    //std::optional<VideoFrame> video_frame;  // NDI视频帧（第二期）
}PairedFrame;

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
