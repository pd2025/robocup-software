#pragma once

#include <algorithm>
#include <array>
#include <limits>

#include <rclcpp/rclcpp.hpp>

#include <rj_common/world_state.hpp>
#include <rj_common/field_dimensions.hpp>
#include <rj_constants/constants.hpp>
#include <rj_constants/topic_names.hpp>
#include <rj_convert/ros_convert.hpp>
#include <rj_msgs/msg/stealer.hpp>
#include <rj_msgs/msg/world_state.hpp>
#include <rj_msgs/srv/stealer.hpp>

#include "rj_strategy/coordinator.hpp"

namespace strategy {

class Stealer
    : public Coordinator<Stealer, rj_msgs::srv::Stealer, rj_msgs::msg::Stealer> {
public:
    static constexpr uint8_t kInvalidRobotId = kNumShells;

    Stealer();
    ~Stealer() override = default;
    Stealer(const Stealer&) = delete;
    Stealer& operator=(const Stealer&) = delete;
    Stealer(Stealer&&) = delete;
    Stealer& operator=(Stealer&&) = delete;

    void service_callback(RequestPtr request, ResponsePtr response);

private:
    void publish_selected_stealer();

    std::array<bool, kNumShells> wants_to_steal_by_id_{};  // Zero-initialized
    WorldState last_world_state_;
    FieldDimensions field_dimensions_;
    rclcpp::Subscription<rj_msgs::msg::WorldState>::SharedPtr world_state_sub_;
    uint8_t last_published_stealer_ = kInvalidRobotId;  // Track last published stealer
};

}  // namespace strategy