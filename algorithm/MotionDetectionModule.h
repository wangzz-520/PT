#ifndef MOTION_DETECTION_MODULE_H
#define MOTION_DETECTION_MODULE_H

#include <cstdint>
#include <string>
#include <sstream>
#include <algorithm>

#include "global.h"

// 前向声明
class OneEuroFilter;

// 动静判断配置结构
struct MotionDetectionConfig {
    // 静态阈值（IMU数据）
    double static_threshold_acc = 0.05;      // m/s²（加速度模值静态阈值）
    double static_threshold_ang = 0.05;       // rad/s（角速度模值静态阈值）

    // 动态阈值（IMU数据）
    double dynamic_threshold_acc = 0.5;       // m/s²（加速度模值动态阈值）
    double dynamic_threshold_ang = 0.5;       // rad/s（角速度模值动态阈值）

    // 运动强度阈值（基于IMU角速度模值）
    double static_threshold_intensity = 0.05;  // 静态阈值（运动强度 < 0.05）
    double dynamic_threshold_intensity = 0.5;   // 动态阈值（运动强度 ≥ 0.5）

    // 一欧元滤波参数（用于IMU模值滤波）
    double filter_cutoff = 1.0;               // 截止频率
    double filter_beta = 0.1;                 // 速度系数
    double filter_d_cutoff = 1.0;             // 速度滤波的截止频率

    // 状态切换参数
    int min_stable_frames = 3;                // 最少稳定帧数（备选，低帧率时使用）
    int min_stable_time_ms = 10;             // 最少稳定时间（毫秒）- 推荐使用
    int state_hold_time_ms = 100;            // 状态保持时间（毫秒）
    double transition_time_ms = 200;         // 参数过渡时间（毫秒）

    // 序列化支持（用于保存/加载配置）
    std::string toJson() const;
    bool fromJson(const std::string& json_str);
};

/*!
    @ClassName   : MotionDetectionModule
    @Description : 动静判断算法
    @Author      : xxx
    @Data        : xxx
*/

class MotionDetectionModule {
public:
    MotionDetectionModule();
    ~MotionDetectionModule();

    // 配置管理
    void setConfig(const MotionDetectionConfig& config);
    const MotionDetectionConfig& getConfig() const { return config_; }

    // 检测运动状态（核心函数）
    MotionState detectMotion(const PairedFrame& frame);

    // 获取当前运动状态
    MotionState getCurrentState() const { return current_state_; }

    // 获取运动强度（0.0-1.0）
    double getMotionIntensity() const { return current_motion_intensity_; }

    // 重置状态
    void reset();

private:
    MotionDetectionConfig config_;
    MotionState current_state_;
    double current_motion_intensity_;
    OneEuroFilter* filter_acc_;
    OneEuroFilter* filter_ang_vel_;
    int state_hold_counter_;
    int64_t last_state_change_time_;
    int64_t pending_state_start_time_;
    MotionState pending_state_;

    double calculateMotionIntensity(
        const Eigen::Vector3d& acceleration,
        const Eigen::Vector3d& angular_velocity,
        int64_t timestamp
    );
    MotionState determineState(double motion_intensity);
    void updateStateHistory(MotionState new_state, int64_t timestamp);
    bool shouldTransitionState(MotionState new_state, int64_t timestamp);
};

#endif // MOTION_DETECTION_MODULE_H
