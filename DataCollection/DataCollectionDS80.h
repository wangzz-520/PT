#ifndef DATA_COLLECTION_DS80_H
#define DATA_COLLECTION_DS80_H
#include <thread>
#include <queue>
#include <list>
#include <fstream>
#include "xv-sdk.h"
#include "xv-sdk-ex.h"
#include "DataCollectionInterface.h"


#define M_PI 3.14159265358979323846

//模组DS80
class DataCollectionDS80 : public i_DataCollectionModule
{
public:
    DataCollectionDS80();
    virtual ~DataCollectionDS80();

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

public:
    void poseThread();
    //时间配对算法
    bool find_nearest_imu(int64_t pose_timestamp_ns, xv::Imu& nearest_imu, double& pair_delta_us, double& pair_quality);

private:
    std::shared_ptr<xv::FisheyeCameras> m_fisheye = nullptr;
    std::shared_ptr<xv::Device> m_device = nullptr;
    std::shared_ptr<xv::Slam> m_slam = nullptr;
    int m_slam_callbackId = 0;
    int m_imu_callbackId = 0;
    bool m_run_posethread = false;
    std::thread m_pose_thread;
    std::mutex m_lock_pose_data;
    std::queue<xv::Pose> m_pose_data;
    std::mutex m_lock_imu_data;
    std::list<xv::Imu> m_imu_data;
    bool m_map_load = false;    //地图加载状态
    std::filebuf m_mapStream;   //地图保存
    bool m_isMapStable = false; //地图是否稳定
};

#endif // DATA_COLLECTION_DS80_H
