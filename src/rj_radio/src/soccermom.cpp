#include "rj_radio/soccermom.hpp"

namespace rj_radio {

SoccerMomNode::SoccerMomNode()
    : Node("soccermom") //node title is soccermom (ros2 run rj_radio soccermom)
{
    // fruit pub
    // assigns the private variable fruit_pub_ (which was created in soccermom.hpp) to a 
    // publisher object that publishes a String message to the /team_fruit topic 
    fruit_pub_ = this->create_publisher<std_msgs::msg::String>("/team_fruit", 10); 

    // fruit sub
    // assigns the private variable team_color_sub_ (which was created in soccermom.hpp) to a 
    // subscription object that subscribes to the /referee/team_color topic
    // when a new TeamColor message arrives, the team_color_callback function should be called
    team_color_sub_ = this->create_subscription<rj_msgs::msg::TeamColor>(
        referee::topics::kTeamColorTopic,
        10,
        std::bind(&SoccerMomNode::team_color_callback, this, std::placeholders::_1)
        // invokes team_color_callback on SoccerMomNode whenever a new TeamColor message is published to the topic.
    );

    // prints to terminal when soccermom node is started
    RCLCPP_INFO(this->get_logger(), "SoccerMom node started.");
}

void SoccerMomNode::team_color_callback(const rj_msgs::msg::TeamColor::SharedPtr msg)
{
    // Creates a message of type std_msgs::msg::String called fruit
    std_msgs::msg::String fruit;

    // if the message recieved is blue, set fruit.data to "blueberries"
    // otherwise, set fruit.data to "bananas"
    if (msg->is_blue) {
        fruit.data = "blueberries";
    } else {
        fruit.data = "bananas";
    }

    // prints fruit.data as a String
    RCLCPP_INFO(this->get_logger(), fruit.data.c_str());
    // publishes the String message fruit to the /fruit_pub_ topic
    fruit_pub_->publish(fruit);
}

}  // namespace rj_radio

// main method
// takes in an int param (number of arguments) and a char param (argument values)
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv); // initializes ROS2 client library
    rclcpp::spin(std::make_shared<rj_radio::SoccerMomNode>()); // creates a shared pointer to a new SoccerMomNode object
    // rclccp::spin keeps the program running and listening for messages
    // when a message arrives, the program calls the team_color_callback method
    // it knows to do this because of the line std::bind(&SoccerMomNode::team_color_callback, this, std::placeholders::_1)
    // stops spinning when the program is stopped in the command line
    rclcpp::shutdown(); // destroys nodes and stopcs spinning
    return 0; // main methods in cpp must return an integer, so 0 is returned
    // 0 is the exit status of the program
    // 0 -> no errors
    // anything else -> errors
}
