#include <stdio.h>
#include <string.h>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/Joy.h"


void reset(geometry_msgs::Vector3& v){
	v.x = v.y = v.z = 0.0;
}

class Controller{
	private:
		ros::Publisher vel_pub;
		geometry_msgs::Twist vel_msg;
		ros::NodeHandle nh;
		ros::Subscriber joy_sub;
		double last_published;

	public:
		Controller(){
			vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel",1000);
			joy_sub = nh.subscribe<sensor_msgs::Joy>("/joy", 1000, &Controller::joyread, this);
		}

		~Controller(){
		}
		
		void joyread(const sensor_msgs::JoyConstPtr& msg){
			// actually reads joy
			last_published = ros::Time::now().toSec();
			double turn = msg->axes[0];
			double thrust = msg->axes[3];
			vel_msg.linear.x = thrust;
			vel_msg.angular.z = turn;
		}

		void joyread(){
			// reset
			double now = ros::Time::now().toSec();
			//std::cout << now - last_published << std::endl;
			if((now - last_published) > 1.0){
				reset(vel_msg.linear);
				reset(vel_msg.angular);
			}
			return;
		}

		void run(){
			ros::Rate rate(10.0);
			while(ros::ok()){
				joyread();
				vel_pub.publish(vel_msg);
				ros::spinOnce();
				rate.sleep();
			}
		}

};

int main(int argc, char* argv[]) {
	ros::init(argc,argv,"controller");
	Controller controller;
	controller.run();
	return 0;
}
