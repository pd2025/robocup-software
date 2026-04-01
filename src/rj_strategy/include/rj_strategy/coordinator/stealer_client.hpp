#pragma once

#include <functional>
#include <utility>

#include <rclcpp/rclcpp.hpp>
#include <spdlog/spdlog.h>

#include <rj_msgs/msg/stealer.hpp>
#include <rj_msgs/srv/stealer.hpp>

#include "rj_strategy/coordinator/stealer.hpp"

namespace strategy {

/**
 * @brief Client for interacting with the Stealer coordinator.
 *
 * Manages membership in the stealer group and tracks the currently selected stealer.
 */
class StealerClient {
public:
    struct Result {
        bool am_i_member{false};  // Whether this robot is currently a member of the stealer group.
        std::optional<int> stealer_id{0};  // ID of Stealer id
    };

    using StatusCallback = std::function<void(Result)>;

    explicit StealerClient(rclcpp::Node::SharedPtr node, uint8_t robot_id);
    ~StealerClient() = default;
    StealerClient(const StealerClient&) = delete;
    StealerClient& operator=(const StealerClient&) = delete;
    StealerClient(StealerClient&&) = delete;
    StealerClient& operator=(StealerClient&&) = delete;

    /**
     * @brief Join the stealer group.
     * @param callback Called with current membership status after attempt to join.
     */
    void join_group(StatusCallback callback = nullptr);

    /**
     * @brief Leave the stealer group.
     * @param callback Called with current membership status after attempt to leave.
     */
    void leave_group(StatusCallback callback = nullptr);

    /**
     * @brief Check if this robot is a member of the stealer group.
     */
    [[nodiscard]] bool am_i_member() const;

    /**
     * @brief Get the currently selected stealer.
     * @return robot ID of selected stealer, or kInvalidRobotId if none selected.
     */
    [[nodiscard]] uint8_t selected_stealer() const;

    /**
     * @brief Check if this robot is currently selected as the stealer.
     */
    [[nodiscard]] bool is_selected() const;

private:
    rclcpp::Node::SharedPtr node_;
    const uint8_t robot_id_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members) -- class
                              // isn't move/copy-able anyway
    rclcpp::Client<rj_msgs::srv::Stealer>::SharedPtr client_;
    rclcpp::Subscription<rj_msgs::msg::Stealer>::SharedPtr subscription_;

    bool am_i_member_{false};
    uint8_t selected_stealer_;
};

}  // namespace strategy
