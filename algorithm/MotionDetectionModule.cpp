#include "MotionDetectionModule.h".h"
#include "OneEuroFilter.h"
#include <cmath>
#include <algorithm>
#include <sstream>


MotionDetectionModule::MotionDetectionModule() {
    config_ = MotionDetectionConfig();
    current_state_ = MotionState::STATIC;
    current_motion_intensity_ = 0.0;
    state_hold_counter_ = 0;
    last_state_change_time_ = 0;
    pending_state_start_time_ = 0;
    pending_state_ = MotionState::STATIC;

    filter_acc_ = new OneEuroFilter(config_.filter_cutoff, config_.filter_beta, config_.filter_d_cutoff);
    filter_ang_vel_ = new OneEuroFilter(config_.filter_cutoff, config_.filter_beta, config_.filter_d_cutoff);
}

MotionDetectionModule::~MotionDetectionModule() {
    delete filter_acc_;
    delete filter_ang_vel_;
}

void MotionDetectionModule::setConfig(const MotionDetectionConfig& config) {
    config_ = config;

    if (filter_acc_) {
        delete filter_acc_;
        filter_acc_ = new OneEuroFilter(config_.filter_cutoff, config_.filter_beta, config_.filter_d_cutoff);
    }
    if (filter_ang_vel_) {
        delete filter_ang_vel_;
        filter_ang_vel_ = new OneEuroFilter(config_.filter_cutoff, config_.filter_beta, config_.filter_d_cutoff);
    }
}

void MotionDetectionModule::reset() {
    current_state_ = MotionState::STATIC;
    current_motion_intensity_ = 0.0;
    state_hold_counter_ = 0;
    last_state_change_time_ = 0;
    pending_state_start_time_ = 0;
    pending_state_ = MotionState::STATIC;

    if (filter_acc_) {
        delete filter_acc_;
        filter_acc_ = new OneEuroFilter(config_.filter_cutoff, config_.filter_beta, config_.filter_d_cutoff);
    }
    if (filter_ang_vel_) {
        delete filter_ang_vel_;
        filter_ang_vel_ = new OneEuroFilter(config_.filter_cutoff, config_.filter_beta, config_.filter_d_cutoff);
    }
}

MotionState MotionDetectionModule::detectMotion(const PairedFrame& frame) {
    Eigen::Vector3d acceleration/* = frame.acceleration*/;
    Eigen::Vector3d angular_velocity = frame.angular_velocity;
    int64_t timestamp = frame.timestamp_ns;

    double motion_intensity = calculateMotionIntensity(
        acceleration, angular_velocity, timestamp
    );

    current_motion_intensity_ = motion_intensity;

    MotionState new_state = determineState(motion_intensity);

    if (shouldTransitionState(new_state, timestamp)) {
        updateStateHistory(new_state, timestamp);
        current_state_ = new_state;
    }

    return current_state_;
}

double MotionDetectionModule::calculateMotionIntensity(
    const Eigen::Vector3d& acceleration,
    const Eigen::Vector3d& angular_velocity,
    int64_t timestamp
) {
    double acc_magnitude = acceleration.norm();
    double ang_vel_magnitude = angular_velocity.norm();

    double timestamp_s = timestamp / 1e9;
    double acc_filtered = filter_acc_->filter(acc_magnitude, timestamp_s);
    double ang_vel_filtered = filter_ang_vel_->filter(ang_vel_magnitude, timestamp_s);

    double norm_angular_velocity = std::min(ang_vel_filtered / 1.0, 1.0);
    double motion_intensity = norm_angular_velocity;

    return motion_intensity;
}

MotionState MotionDetectionModule::determineState(double motion_intensity) {
    if (motion_intensity < config_.static_threshold_intensity) {
        return MotionState::STATIC;
    } else if (motion_intensity >= config_.dynamic_threshold_intensity) {
        return MotionState::DYNAMIC;
    } else {
        return MotionState::MOVING;
    }
}

bool MotionDetectionModule::shouldTransitionState(MotionState new_state, int64_t timestamp) {
    if (new_state == current_state_) {
        state_hold_counter_ = 0;
        pending_state_ = current_state_;
        return false;
    }

    if (new_state == pending_state_) {
        state_hold_counter_++;

        int64_t time_since_pending = timestamp - pending_state_start_time_;
        int64_t min_stable_time_ns = config_.min_stable_time_ms * 1000000LL;

        bool time_condition_met = (time_since_pending >= min_stable_time_ns);
        bool frame_condition_met = (state_hold_counter_ >= config_.min_stable_frames);

        if (time_condition_met || frame_condition_met) {
            int64_t time_since_last_change = timestamp - last_state_change_time_;
            int64_t hold_time_ns = config_.state_hold_time_ms * 1000000LL;

            if (time_since_last_change >= hold_time_ns || last_state_change_time_ == 0) {
                return true;
            }
        }
    } else {
        pending_state_ = new_state;
        pending_state_start_time_ = timestamp;
        state_hold_counter_ = 1;
    }

    return false;
}

void MotionDetectionModule::updateStateHistory(MotionState new_state, int64_t timestamp) {
    if (new_state != current_state_) {
        last_state_change_time_ = timestamp;
        state_hold_counter_ = 0;
        pending_state_ = new_state;
    }
}


std::string MotionDetectionConfig::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"static_threshold_acc\": " << static_threshold_acc << ",\n";
    oss << "  \"static_threshold_ang\": " << static_threshold_ang << ",\n";
    oss << "  \"dynamic_threshold_acc\": " << dynamic_threshold_acc << ",\n";
    oss << "  \"dynamic_threshold_ang\": " << dynamic_threshold_ang << ",\n";
    oss << "  \"static_threshold_intensity\": " << static_threshold_intensity << ",\n";
    oss << "  \"dynamic_threshold_intensity\": " << dynamic_threshold_intensity << ",\n";
    oss << "  \"filter_cutoff\": " << filter_cutoff << ",\n";
    oss << "  \"filter_beta\": " << filter_beta << ",\n";
    oss << "  \"filter_d_cutoff\": " << filter_d_cutoff << ",\n";
    oss << "  \"min_stable_frames\": " << min_stable_frames << ",\n";
    oss << "  \"min_stable_time_ms\": " << min_stable_time_ms << ",\n";
    oss << "  \"state_hold_time_ms\": " << state_hold_time_ms << ",\n";
    oss << "  \"transition_time_ms\": " << transition_time_ms << "\n";
    oss << "}";
    return oss.str();
}

bool MotionDetectionConfig::fromJson(const std::string& json_str) {
    // TODO: 实现JSON解析
    // 这里仅作示例，实际应使用rapidjson或nlohmann/json解析JSON字符串
    // 解析后设置各个阈值和参数
    return false; // 暂未实现
}
