#include "StabilizationAlgorithmModule.h"
#include "OneEuroFilter.h"
#include <cmath>
#include <algorithm>
#include <sstream>

StabilizationAlgorithmModule::StabilizationAlgorithmModule() {
    config_ = StabilizationConfig();
    current_motion_state_ = MotionState::STATIC;
    target_motion_state_ = MotionState::STATIC;
    current_cutoff_ = config_.static_cutoff;
    current_beta_ = config_.static_beta;
    target_cutoff_ = config_.static_cutoff;
    target_beta_ = config_.static_beta;
    transition_start_time_ = 0;
    is_transitioning_ = false;

    filter_x_ = new OneEuroFilter(current_cutoff_, current_beta_, 1.0);
    filter_y_ = new OneEuroFilter(current_cutoff_, current_beta_, 1.0);
    filter_z_ = new OneEuroFilter(current_cutoff_, current_beta_, 1.0);
}

StabilizationAlgorithmModule::~StabilizationAlgorithmModule() {
    delete filter_x_;
    delete filter_y_;
    delete filter_z_;
}

void StabilizationAlgorithmModule::setConfig(const StabilizationConfig& config) {
    config_ = config;
    updateFilterParameters(0);
}

void StabilizationAlgorithmModule::setMotionState(MotionState state) {
    if (state != target_motion_state_) {
        target_motion_state_ = state;
        getTargetParameters(state, target_cutoff_, target_beta_);

        if (state != current_motion_state_) {
            is_transitioning_ = true;
            transition_start_time_ = 0;
        }
    }
}

StabilizedFrame StabilizationAlgorithmModule::stabilize(const PairedFrame& frame) {
    int64_t timestamp = frame.timestamp_ns;

    if (is_transitioning_ && transition_start_time_ == 0) {
        transition_start_time_ = timestamp;
    }

    applyTransition(timestamp);
    updateFilterParameters(timestamp);

    double timestamp_s = timestamp / 1e9;

    double filtered_x = filter_x_->filter(frame.position.x(), timestamp_s);
    double filtered_y = filter_y_->filter(frame.position.y(), timestamp_s);
    double filtered_z = filter_z_->filter(frame.position.z(), timestamp_s);

    StabilizedFrame result;
    result.position = Eigen::Vector3d(filtered_x, filtered_y, filtered_z);
    result.rotation = frame.rotation;
    result.acceleration = frame.acceleration;
    result.angular_velocity = frame.angular_velocity;
    result.timestamp_ns = frame.timestamp_ns;
    result.confidence = frame.confidence;
    //result.mode = frame.mode;
    result.motion_state = current_motion_state_;

    return result;
}

void StabilizationAlgorithmModule::reset() {
    current_motion_state_ = MotionState::STATIC;
    target_motion_state_ = MotionState::STATIC;
    current_cutoff_ = config_.static_cutoff;
    current_beta_ = config_.static_beta;
    target_cutoff_ = config_.static_cutoff;
    target_beta_ = config_.static_beta;
    transition_start_time_ = 0;
    is_transitioning_ = false;

    delete filter_x_;
    delete filter_y_;
    delete filter_z_;

    filter_x_ = new OneEuroFilter(current_cutoff_, current_beta_, 1.0);
    filter_y_ = new OneEuroFilter(current_cutoff_, current_beta_, 1.0);
    filter_z_ = new OneEuroFilter(current_cutoff_, current_beta_, 1.0);
}

void StabilizationAlgorithmModule::updateFilterParameters(int64_t timestamp) {
    if (filter_x_ && filter_y_ && filter_z_) {
        filter_x_->setMinCutoff(current_cutoff_);
        filter_x_->setBeta(current_beta_);
        filter_y_->setMinCutoff(current_cutoff_);
        filter_y_->setBeta(current_beta_);
        filter_z_->setMinCutoff(current_cutoff_);
        filter_z_->setBeta(current_beta_);
    }
}

void StabilizationAlgorithmModule::applyTransition(int64_t timestamp) {
    if (!is_transitioning_) {
        return;
    }

    int64_t transition_time_ns = static_cast<int64_t>(config_.transition_time_ms * 1e6);
    int64_t elapsed = timestamp - transition_start_time_;

    if (elapsed >= transition_time_ns) {
        current_cutoff_ = target_cutoff_;
        current_beta_ = target_beta_;
        current_motion_state_ = target_motion_state_;
        is_transitioning_ = false;
    } else {
        double t = static_cast<double>(elapsed) / static_cast<double>(transition_time_ns);

        double old_cutoff = (current_motion_state_ == MotionState::STATIC) ? config_.static_cutoff :
                           (current_motion_state_ == MotionState::MOVING) ? config_.moving_cutoff :
                           config_.dynamic_cutoff;
        double old_beta = (current_motion_state_ == MotionState::STATIC) ? config_.static_beta :
                         (current_motion_state_ == MotionState::MOVING) ? config_.moving_beta :
                         config_.dynamic_beta;

        current_cutoff_ = old_cutoff + (target_cutoff_ - old_cutoff) * t;
        current_beta_ = old_beta + (target_beta_ - old_beta) * t;
    }
}

void StabilizationAlgorithmModule::getTargetParameters(MotionState state, double& cutoff, double& beta) {
    switch (state) {
        case MotionState::STATIC:
            cutoff = config_.static_cutoff;
            beta = config_.static_beta;
            break;
        case MotionState::MOVING:
            cutoff = config_.moving_cutoff;
            beta = config_.moving_beta;
            break;
        case MotionState::DYNAMIC:
            cutoff = config_.dynamic_cutoff;
            beta = config_.dynamic_beta;
            break;
    }
}

std::string StabilizationConfig::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"static_cutoff\": " << static_cutoff << ",\n";
    oss << "  \"static_beta\": " << static_beta << ",\n";
    oss << "  \"moving_cutoff\": " << moving_cutoff << ",\n";
    oss << "  \"moving_beta\": " << moving_beta << ",\n";
    oss << "  \"dynamic_cutoff\": " << dynamic_cutoff << ",\n";
    oss << "  \"dynamic_beta\": " << dynamic_beta << ",\n";
    oss << "  \"transition_time_ms\": " << transition_time_ms << "\n";
    oss << "}";
    return oss.str();
}

bool StabilizationConfig::fromJson(const std::string& json_str) {
    return false;
}
