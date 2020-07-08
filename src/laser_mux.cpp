#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <std_srvs/Empty.h>

class LaserMux{

public:

    LaserMux(){

        nh_.param("laser_1_topic", laser1topic_, std::string("/scanPicoFull"));
        nh_.param("laser_2_topic", laser2topic_, std::string("/scanGridMap"));
        
        laser_pub_ = nh_.advertise<sensor_msgs::LaserScan>("/localScan", 1);   
        laser1sub_ = nh_.subscribe<sensor_msgs::LaserScan>(laser1topic_, 1, &LaserMux::laser1Callback, this);
        laser2sub_ = nh_.subscribe<sensor_msgs::LaserScan>(laser2topic_, 1, &LaserMux::laser2Callback, this);

        switch_input_srv_ = nh_.advertiseService("/custom_costmap_node/switch_input", &LaserMux::switchInput, this);

    }

private:

    bool switchInput(std_srvs::EmptyRequest &req, std_srvs::EmptyResponse &rep){
        if(laser_selected_ == 0){
            laser_selected_ = 1;
        }else
        {
            laser_selected_ = 0;
        }
        return true;
    }

    void laser1Callback(const sensor_msgs::LaserScanConstPtr &scan){
        
        if(laser_selected_ != 0)
            return;
        
        laser_pub_.publish(*scan);
    }

    void laser2Callback(const sensor_msgs::LaserScanConstPtr &scan){
        
        if(laser_selected_ != 1)
            return;

        laser_pub_.publish(*scan);

    }

    ros::NodeHandle nh_{"~"};
    ros::Publisher laser_pub_;
    ros::Subscriber laser1sub_, laser2sub_;
    ros::ServiceServer switch_input_srv_;
    sensor_msgs::LaserScan out_scan_;

    std::string laser1topic_, laser2topic_;
    int laser_selected_ = 0;
};
int main(int argc, char **argv){

    ros::init(argc, argv, "laser_mux");

    LaserMux mux;

    ros::spin();

    return 0;
}