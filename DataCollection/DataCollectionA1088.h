#ifndef DATA_COLLECTION_A1088_H
#define DATA_COLLECTION_A1088_H

#include "DataCollectionInterface.h"

//模组A1088
class DataCollectionA1088 : public i_DataCollectionModule
{
public:
    DataCollectionA1088();
    virtual ~DataCollectionA1088();

    // 初始化
    virtual bool initialize(const DataCollectionConfig& config);

    //清理
    virtual void cleanup();

    // 开始
    virtual bool start();

    // 停止
    virtual void stop();

    // 获取数据
    virtual PairedFrame getFrame();

    // 设置工作模式(map_file_path为空时,创建地图。不为空时，加载地图)
    virtual bool setWorkMode(WorkMode mode, std::string map_file_path = "");

    //地图是否加载成功
    virtual bool isMapLoaded() const;

    // 检查地图定位是否稳定
    virtual bool isMapStable() const;

    //设置VIO初始坐标
    virtual void setVIOInitialPose(const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation);

    // 建图质量分析
    virtual MappingQuality analyzeMappingQuality(int64_t duration_ns);

    // 设置曝光模式
    virtual void setExposureMode(ExposureMode mode);

    //手动设置曝光值
    virtual void setExposureValue(double exposure_value);

    // 第二期扩展：NDI视频接收
    //bool initializeNDIReceiver(const std::string& ndi_source_name);
    //void setNDIVideoEnabled(bool enabled);
    //bool isNDIVideoEnabled() const;
};

#endif // DATA_COLLECTION_A1088_H
