#include "rj_strategy/coordinator/stealer.hpp"

namespace strategy {

Stealer::Stealer()
    : Coordinator("stealer_srv", "stealer_data", "stealer_node") {
    // Subscribe to world state
    world_state_sub_ = this->create_subscription<rj_msgs::msg::WorldState>(
        vision_filter::topics::kWorldStateTopic, rclcpp::QoS(1),
        [this](rj_msgs::msg::WorldState::SharedPtr world_state) {  // NOLINT
            last_world_state_ = rj_convert::convert_from_ros(*world_state);
            publish_selected_stealer();
        });
}

void Stealer::service_callback(RequestPtr request, ResponsePtr response) {
    // keeps track of if a robot's membership in the stealer group has changed
    bool membership_changed = wants_to_steal_by_id_[request->robot_id] != request->wants_to_steal;

    wants_to_steal_by_id_[request->robot_id] = request->wants_to_steal;

    if (membership_changed) {
        publish_selected_stealer();
    }

    response->success = true;
}

void Stealer::publish_selected_stealer() {
    // Find closest robot to ball among group members
    double min_distance = std::numeric_limits<double>::infinity();
    uint8_t selected_stealer = kInvalidRobotId;

    // saves ball position as ball_pos
    const auto& ball_pos = last_world_state_.ball.position;

    // if ball is on the other team's side of the field
    if (field_dimensions_.their_defense_area().contains_point(ball_pos)) {
        // for every robot
        for (uint8_t i = 0; i < kNumShells; ++i) {
            // if the robot wants to steal
            if (wants_to_steal_by_id_[i]) {
                const auto& robot = last_world_state_.get_robot(true, i);
                // save distance as dist between ball_pos and robot_pos
                double distance = ball_pos.dist_to(robot.pose.position());
                // if the distance is less than min dist, update min dist and make that robot the selected stealer
                if (distance < min_distance) {
                    min_distance = distance;
                    selected_stealer = i;
                }
            }
        }
    }
    

    // Only publish if the selected stealer has changed
    if (selected_stealer != last_published_stealer_) {
        // publish the id of the selected stealer (?)
        publisher_->publish(rj_msgs::msg::Stealer().set__robot_id(selected_stealer));
        // update last_published_stealer_
        last_published_stealer_ = selected_stealer;
    }
}

}  // namespace strategy

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<strategy::Stealer>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}