#pragma once

#include <rclcpp/rclcpp.hpp> // gives access to create_subscription and create_publisher
#include <std_msgs/msg/string.hpp> // allows message of type std_msgs::msg::String to be published to /team_fruit
#include <rj_msgs/msg/team_color.hpp> // allows you to use TeamColor messages
#include <rj_constants/topic_names.hpp> // allows you to use topic name constants defined in this file in the cpp file

namespace rj_radio {

class SoccerMomNode : public rclcpp::Node {
public:
    SoccerMomNode(); // declares a public constructor called SoccerMomNode

private:
    void team_color_callback(const rj_msgs::msg::TeamColor::SharedPtr msg); // declares a private function called team_color_callback that is invoked when a TeamColor message is recieved

    rclcpp::Subscription<rj_msgs::msg::TeamColor>::SharedPtr team_color_sub_; // creates a private varible called team_color_sub of type rclcpp::Subscription<rj_msgs::msg::TeamColor>::SharedPtr
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr fruit_pub_; // creates a private varible called fruit_pub_ of type rclcpp::Publisher<std_msgs::msg::String>::SharedPtr
};

}  // namespace rj_radio
