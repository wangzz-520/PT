#ifndef MOUNTING_ANGLE_CALIBRATOR_H
#define MOUNTING_ANGLE_CALIBRATOR_H

#include <string>
#include <vector>
#include "global.h"

struct MountingAngleConfig {
    enum class AngleType {
        NORMAL,           // 正装（0度）
        SIDE_LEFT,        // 左侧装（绕Y轴90度）
        SIDE_RIGHT,       // 右侧装（绕Y轴-90度）
        // 绕X轴旋转的常用角度
        ROTATE_X_45,      // 绕X轴旋转45度
        ROTATE_X_60,      // 绕X轴旋转60度
        ROTATE_X_90,      // 绕X轴旋转90度
        ROTATE_X_NEG_45,  // 绕X轴旋转-45度
        ROTATE_X_NEG_60,  // 绕X轴旋转-60度
        ROTATE_X_NEG_90   // 绕X轴旋转-90度
    };

    AngleType angle_type = AngleType::NORMAL;
    Eigen::Quaterniond calibration_quat = Eigen::Quaterniond(1.0, 0.0, 0.0, 0.0);
    bool enabled = false;

    // 序列化支持（用于保存/加载配置）
    std::string toJson() const;
    bool fromJson(const std::string& json_str);
};


/*!
    @ClassName   : MountingAngleCalibrator
    @Description : 角度安装校准算法
    @Author      : xxx
    @Data        : 2025-12-02 10:25:41
*/

class MountingAngleCalibrator {
public:
    MountingAngleCalibrator();
    ~MountingAngleCalibrator() = default;

    // 配置管理
    void setConfig(const MountingAngleConfig& config);
    const MountingAngleConfig& getConfig() const { return config_; }

    // 预设角度设置
    void setPresetAngle(MountingAngleConfig::AngleType angle_type);
    void setEnabled(bool enabled) { config_.enabled = enabled; }

    // 校准处理（核心函数）
    Eigen::Quaterniond calibrate(const Eigen::Quaterniond& input_quat);

    // 批量校准（优化版本）
    void calibrateBatch(
        const std::vector<Eigen::Quaterniond>& input_quats,
        std::vector<Eigen::Quaterniond>& output_quats
    );

    // 配置保存/加载
    bool saveConfig(const std::string& filepath) const;
    bool loadConfig(const std::string& filepath);

    // 验证校准参数
    bool validateConfig() const;

private:
    MountingAngleConfig config_;
    Eigen::Quaterniond calibration_quat_inv_;

    void updateCalibrationQuat();
    Eigen::Quaterniond normalizeQuaternion(const Eigen::Quaterniond& q);
};

#endif // MOUNTING_ANGLE_CALIBRATOR_H
