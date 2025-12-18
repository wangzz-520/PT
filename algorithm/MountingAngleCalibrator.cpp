#include "MountingAngleCalibrator.h"

#include <cmath>
#include <fstream>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


MountingAngleCalibrator::MountingAngleCalibrator() {
    config_.enabled = false;
    config_.angle_type = MountingAngleConfig::AngleType::NORMAL;
    config_.calibration_quat = Eigen::Quaterniond(1.0, 0.0, 0.0, 0.0);
    updateCalibrationQuat();
}

void MountingAngleCalibrator::setConfig(const MountingAngleConfig& config) {
    //config_ = config;
    updateCalibrationQuat();
}

void MountingAngleCalibrator::setPresetAngle(MountingAngleConfig::AngleType angle_type) {
    config_.angle_type = angle_type;
    updateCalibrationQuat();
}

Eigen::Quaterniond MountingAngleCalibrator::calibrate(const Eigen::Quaterniond& input_quat) {
    if (!config_.enabled) {
        return normalizeQuaternion(input_quat);
    }

    // 应用校准旋转：q_calibrated = q_calibration.conjugate() * q_original
    Eigen::Quaterniond q_calibrated = calibration_quat_inv_ * input_quat;

    return normalizeQuaternion(q_calibrated);
}

void MountingAngleCalibrator::calibrateBatch(
    const std::vector<Eigen::Quaterniond>& input_quats,
    std::vector<Eigen::Quaterniond>& output_quats
) {
    output_quats.resize(input_quats.size());

    if (!config_.enabled) {
        for (size_t i = 0; i < input_quats.size(); ++i) {
            output_quats[i] = normalizeQuaternion(input_quats[i]);
        }
        return;
    }

    for (size_t i = 0; i < input_quats.size(); ++i) {
        output_quats[i] = normalizeQuaternion(calibration_quat_inv_ * input_quats[i]);
    }
}

void MountingAngleCalibrator::updateCalibrationQuat() {
    switch (config_.angle_type) {
        case MountingAngleConfig::AngleType::NORMAL:
            config_.calibration_quat = Eigen::Quaterniond(1.0, 0.0, 0.0, 0.0);
            break;

        case MountingAngleConfig::AngleType::SIDE_LEFT:
            config_.calibration_quat = Eigen::AngleAxisd(M_PI / 2.0, Eigen::Vector3d::UnitY());
            break;

        case MountingAngleConfig::AngleType::SIDE_RIGHT:
            config_.calibration_quat = Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitY());
            break;

        // 绕X轴旋转的常用角度
        case MountingAngleConfig::AngleType::ROTATE_X_45:
            config_.calibration_quat = Eigen::AngleAxisd(M_PI / 4.0, Eigen::Vector3d::UnitX());
            break;

        case MountingAngleConfig::AngleType::ROTATE_X_60:
            config_.calibration_quat = Eigen::AngleAxisd(M_PI / 3.0, Eigen::Vector3d::UnitX());
            break;

        case MountingAngleConfig::AngleType::ROTATE_X_90:
            config_.calibration_quat = Eigen::AngleAxisd(M_PI / 2.0, Eigen::Vector3d::UnitX());
            break;

        case MountingAngleConfig::AngleType::ROTATE_X_NEG_45:
            config_.calibration_quat = Eigen::AngleAxisd(-M_PI / 4.0, Eigen::Vector3d::UnitX());
            break;

        case MountingAngleConfig::AngleType::ROTATE_X_NEG_60:
            config_.calibration_quat = Eigen::AngleAxisd(-M_PI / 3.0, Eigen::Vector3d::UnitX());
            break;

        case MountingAngleConfig::AngleType::ROTATE_X_NEG_90:
            config_.calibration_quat = Eigen::AngleAxisd(-M_PI / 2.0, Eigen::Vector3d::UnitX());
            break;
    }

    // 归一化并确保w≥0
    config_.calibration_quat.normalize();
    if (config_.calibration_quat.w() < 0.0) {
        config_.calibration_quat.coeffs() *= -1.0;
    }

    // 计算共轭
    calibration_quat_inv_ = config_.calibration_quat.conjugate();
}

Eigen::Quaterniond MountingAngleCalibrator::normalizeQuaternion(const Eigen::Quaterniond& q) {
    Eigen::Quaterniond q_normalized = q.normalized();
    if (q_normalized.w() < 0.0) {
        q_normalized.coeffs() *= -1.0;
    }
    return q_normalized;
}

bool MountingAngleCalibrator::saveConfig(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // 简单的JSON格式保存
    // 注意：保存时按XYZW顺序（x, y, z, w）
    file << "{\n";
    file << "  \"angle_type\": " << static_cast<int>(config_.angle_type) << ",\n";
    file << "  \"enabled\": " << (config_.enabled ? "true" : "false") << ",\n";
    file << "  \"calibration_quat\": ["
         << config_.calibration_quat.x() << ", "
         << config_.calibration_quat.y() << ", "
         << config_.calibration_quat.z() << ", "
         << config_.calibration_quat.w() << "]\n";
    file << "}\n";

    return true;
}

bool MountingAngleCalibrator::loadConfig(const std::string& filepath) {
    // 简化实现，实际应使用JSON解析库
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // TODO: 实现JSON解析
    // 这里仅作示例，实际应使用rapidjson或nlohmann/json
    // 建议使用config_.fromJson()方法解析JSON字符串

    return true;
}

// MountingAngleConfig的toJson和fromJson方法实现
// 注意：这些方法需要在MountingAngleConfig结构体定义后实现
std::string MountingAngleConfig::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"angle_type\": " << static_cast<int>(angle_type) << ",\n";
    oss << "  \"enabled\": " << (enabled ? "true" : "false") << ",\n";
    oss << "  \"calibration_quat\": ["
        << calibration_quat.x() << ", "
        << calibration_quat.y() << ", "
        << calibration_quat.z() << ", "
        << calibration_quat.w() << "]\n";
    oss << "}";
    return oss.str();
}

bool MountingAngleConfig::fromJson(const std::string& json_str) {
    // TODO: 实现JSON解析
    // 这里仅作示例，实际应使用rapidjson或nlohmann/json解析JSON字符串
    // 解析后设置angle_type, enabled, calibration_quat等字段
    return false; // 暂未实现
}

bool MountingAngleCalibrator::validateConfig() const {
    // 验证校准四元数是否归一化
    double norm = config_.calibration_quat.norm();
    if (std::abs(norm - 1.0) > 1e-6) {
        return false;
    }

    // 验证w≥0
    if (config_.calibration_quat.w() < -1e-6) {
        return false;
    }

    return true;
}
