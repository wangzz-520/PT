#ifndef STABILIZATION_ALGORITHM_MODULE_H
#define STABILIZATION_ALGORITHM_MODULE_H


#include <cstdint>
#include <string>
#include <sstream>

#include "global.h"

class OneEuroFilter;

struct StabilizationConfig {
    double static_cutoff = 0.5;
    double static_beta = 0.03;
    double moving_cutoff = 1.0;
    double moving_beta = 0.1;
    double dynamic_cutoff = 2.0;
    double dynamic_beta = 0.2;
    double transition_time_ms = 200.0;

    std::string toJson() const;
    bool fromJson(const std::string& json_str);
};

/*!
    @ClassName   : StabilizationAlgorithmModule
    @Description : 增稳算法
    @Author      : xxx
    @Data        : xxx
*/

class StabilizationAlgorithmModule {
public:
    StabilizationAlgorithmModule();
    ~StabilizationAlgorithmModule();

    void setConfig(const StabilizationConfig& config);
    const StabilizationConfig& getConfig() const { return config_; }

    void setMotionState(MotionState state);
    MotionState getMotionState() const { return current_motion_state_; }

    StabilizedFrame stabilize(const PairedFrame& frame);

    void reset();

private:
    StabilizationConfig config_;
    MotionState current_motion_state_;
    MotionState target_motion_state_;

    OneEuroFilter* filter_x_;
    OneEuroFilter* filter_y_;
    OneEuroFilter* filter_z_;

    double current_cutoff_;
    double current_beta_;
    double target_cutoff_;
    double target_beta_;
    int64_t transition_start_time_;
    bool is_transitioning_;

    void updateFilterParameters(int64_t timestamp);
    void applyTransition(int64_t timestamp);
    void getTargetParameters(MotionState state, double& cutoff, double& beta);
};

#endif // STABILIZATION_ALGORITHM_MODULE_H
