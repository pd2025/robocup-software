#include "rj_strategy/coordinator/stealer_client.hpp"

namespace strategy {

/**
 * @brief Client for interacting with the Stealer coordinator.
 *
 * Manages membership in the stealer group and tracks the currently selected stealer.
 */

StealerClient::StealerClient(rclcpp::Node::SharedPtr node, uint8_t robot_id)
    : node_{std::move(node)}, robot_id_{robot_id}, selected_stealer_{Stealer::kInvalidRobotId} {
    client_ = node_->create_client<rj_msgs::srv::Stealer>("stealer_srv");
}

void StealerClient::join_group(StatusCallback callback) {
    if (am_i_member_) {
        return;
    }

    if (!client_->wait_for_service(std::chrono::seconds(1))) {
        SPDLOG_ERROR("Stealer service not available.");
        if (callback) {
            callback(Result{false});
        }
        return;
    }

    auto request = std::make_shared<rj_msgs::srv::Stealer::Request>();
    request->robot_id = robot_id_;
    request->wants_to_steal = true;

    client_->async_send_request(
        request, [this, callback = std::move(callback)](
                     rclcpp::Client<rj_msgs::srv::Stealer>::SharedFuture
                         future) {  // 6 NOLINT(performance-unnecessary-value-param) --
                                    //  ROS2 async callbacks require value capture.
            if (!future.valid() || !future.get()->success) {
                if (callback) {
                    callback(Result{false});
                }
                return;
            }

            am_i_member_ = true;

            // Create subscription to track selected stealer.
            subscription_ = node_->create_subscription<rj_msgs::msg::Stealer>(
                "stealer_data", rclcpp::QoS(1).transient_local(),
                [this,
                 callback = std::move(callback)](const rj_msgs::msg::Stealer::SharedPtr msg) {
                    selected_stealer_ = msg->robot_id;
                    if (callback) {
                        callback(Result{true, selected_stealer_});
                    }
                });
        });
}

void StealerClient::leave_group(StatusCallback callback) {
    if (!am_i_member_) {
        if (callback) {
            callback(Result{false});
        }
        return;
    }

    auto request = std::make_shared<rj_msgs::srv::Stealer::Request>();
    request->robot_id = robot_id_;
    request->wants_to_steal = false;

    client_->async_send_request(
        request, [this, callback = std::move(callback)](
                     rclcpp::Client<rj_msgs::srv::Stealer>::SharedFuture
                         future) {  // 6 NOLINT(performance-unnecessary-value-param) --
                                    //  ROS2 async callbacks require value capture.
            if (!future.valid() || !future.get()->success) {
                if (callback) {
                    callback(Result{false});
                }
                return;
            }

            am_i_member_ = false;

            // Resetting the shared ptr releases our pointer to the
            // subscription. ROS only keeps a weak_ptr, so this will
            // deallocate the subscription. The callback will no longer be
            // called.
            subscription_.reset();
            selected_stealer_ = Stealer::kInvalidRobotId;

            if (callback) {
                callback(Result{false});
            }
        });
}

bool StealerClient::am_i_member() const { return am_i_member_; }

uint8_t StealerClient::selected_stealer() const { return selected_stealer_; }

bool StealerClient::is_selected() const { return selected_stealer_ == robot_id_; }

}  // namespace strategy
