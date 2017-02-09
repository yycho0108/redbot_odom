#include <RedBot.h>
#include <ros.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Twist.h>

#define W_D (.0635) // wheel diameter
//#define countsPerRev 192

float l_p=0, r_p=0;
RedBotMotors motors;
RedBotEncoder encoder = RedBotEncoder(A2, 10);  // initializes encoder on pins A2 and 10
//int countsPerRev = 192;   // 4 pairs of N-S x 48:1 gearbox = 192 ticks per wheel rev

/* ROS_RELATED */
void twist_cb(const geometry_msgs::Twist& twist_msg) {
  float v = twist_msg.linear.x;
  float w = twist_msg.angular.z;

  float p = v * 2;
  float m = w * W_D;
  l_p = (p - m) / 2.0;
  r_p = (p + m) / 2.0;

  l_p = constrain(l_p * 255, -255, 255);
  r_p = constrain(r_p * 255, -255, 255);
}

ros::NodeHandle nh;
ros::Subscriber<geometry_msgs::Twist> twist_sub("cmd_vel", &twist_cb);
std_msgs::Int16 lwheel_msg;
std_msgs::Int16 rwheel_msg;
ros::Publisher lwheel_pub("lwheel", &lwheel_msg);
ros::Publisher rwheel_pub("rwheel", &rwheel_msg);

void setup()
{
  nh.initNode();
  nh.subscribe(twist_sub);
  nh.advertise(lwheel_pub);
  nh.advertise(rwheel_pub);
  
  encoder.clearEnc(BOTH);
}

void loop(void)
{
  
  nh.spinOnce();
  
  // drive
  motors.leftMotor(l_p);
  motors.rightMotor(r_p);
  
  //sense
  lwheel_msg.data = -encoder.getTicks(LEFT);
  rwheel_msg.data = encoder.getTicks(RIGHT);

  lwheel_pub.publish(&lwheel_msg);
  rwheel_pub.publish(&rwheel_msg);
  
  delay(100);
}
