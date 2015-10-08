#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

#define LED 0
#define LED2 2
void testCallback(const geometry_msgs::Twist move)
{
	softPwmWrite(LED, move.linear.x);
	softPwmWrite(LED2, move.angular.z);
	std::cout << "Linear X: " << move.linear.x << " Angula Z: " << move.angular.z << std::endl;
}
int main(int argc, char **argv)
{
	wiringPiSetup ();
	softPwmCreate(LED, 0, 100);
	softPwmCreate(LED2, 0, 100);
	ros::init(argc, argv, "mover");
	ros::NodeHandle nodeHandler;

	ros::Subscriber sub = nodeHandler.subscribe("cmd_vel", 1, testCallback);

	ros::spin();
	
	return 0;
}
