#ifndef DATA_COLLECTION_INTERFACE_H
#define DATA_COLLECTION_INTERFACE_H
#include <functional>
#include <opencv2/core.hpp>
#include "Eigen/Eigen"
#include "global.h"

// 模组配置
typedef struct _DataCollectionConfig
{
    // 日志等级
    LogLevel log_level = LogLevel::LogLevel_err;

    // 时间配对配置
    double pair_tolerance_us = 2000.0;  // 配对容差（微秒）

    // 工作模式配置
    WorkMode work_mode = WorkMode::VIO;
    double getpose_prediction = 5;  //getPose预测（毫秒）
    // 地图保存路径
    std::string map_file_path;

    // 曝光控制配置
    ExposureMode exposure_mode = ExposureMode::AUTO;
    double exposure_value = 0.0;

    // 性能配置
    size_t buffer_size = 3000;  // 缓冲区大小
    int64_t timeout_ns = 1000000000;  // 超时时间（纳秒）

    // imgData图像数据回调
    std::function<void(const cv::Mat& left, const cv::Mat& right)> cb_img_data = nullptr;
    // mapPercent地图保存/加载进度回调（开始：0 >>> 完成：100）
    std::function<void(int percent)> cb_map_percent = nullptr;

    // 第二期：NDI配置
    //bool ndi_enabled = false;
    //std::string ndi_source_name = "";

}DataCollectionConfig;

//数据采集模块接口类
class i_DataCollectionModule 
{

public:
    virtual ~i_DataCollectionModule() {};

    // 初始化
    virtual bool initialize(const DataCollectionConfig& config) = 0;

    //清理
    virtual void cleanup() = 0;

    // 开始
    virtual bool start() = 0 ;

    // 停止
    virtual void stop() = 0 ;

    // 获取数据
    virtual PairedFrame getFrame() = 0;

    // 设置工作模式(map_file_path为空时,创建地图。不为空时，加载地图)
    virtual bool setWorkMode(WorkMode mode, std::string map_file_path = "") = 0;

    //获取当前工作模式
    virtual WorkMode getWorkMode() const { return m_dataCollectionConfig.work_mode; };

    //地图是否加载成功
    virtual bool isMapLoaded() const = 0;

    // 检查地图定位是否稳定
    virtual bool isMapStable() const = 0;

    //设置VIO初始坐标
    virtual void setVIOInitialPose(const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation) = 0;

    // 建图质量分析
    virtual MappingQuality analyzeMappingQuality(int64_t duration_ns) = 0;

    // 设置曝光模式
    virtual void setExposureMode(ExposureMode mode) = 0;

    //获取当前曝光模式
    virtual ExposureMode getExposureMode() const { return m_dataCollectionConfig.exposure_mode;  };

    //手动设置曝光值
    virtual void setExposureValue(double exposure_value) = 0;

    // 第二期扩展：NDI视频接收
    //bool initializeNDIReceiver(const std::string& ndi_source_name);
    //void setNDIVideoEnabled(bool enabled);
    //bool isNDIVideoEnabled() const;

protected:
    DataCollectionConfig m_dataCollectionConfig;
};

#endif // DATA_COLLECTION_INTERFACE_H
