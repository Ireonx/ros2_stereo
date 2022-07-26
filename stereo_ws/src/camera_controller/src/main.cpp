#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "std_msgs/msg/header.hpp"
#include <tlive/tlive_client.h>
#include <rclcpp/logging.hpp>
#include <rclcpp/logger.hpp>
#include <opencv/cv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <camera_calibration_parsers/parse_yml.h>

using namespace std::chrono_literals;

bool is_stop_ctrl_shell = false;

bool ctrl_shell(void *user_data);

class TrackingCam3d_Client : public tlive::Client
{
public:
    TrackingCam3d_Client() {
        support("left");
        support("right");
    };
};

TrackingCam3d_Client cam;

class Publisher : public rclcpp::Node
{
public:
    Publisher()
        : Node("publisherCam"), count_(0)
    {
        
        std::string tmp;
        std::string leftCalib;
        std::string rightCalib;
        //rclcpp::Parameter leftCalib;
        //rclcpp::Parameter rightCalib;
        //this->declare_parameter("/home/nikita/stereo_ws/config/left.yaml");
        //this->declare_parameter("/home/nikita/stereo_ws/config/right.yaml");
        leftCalib = "/packages/config/left.yaml";
        rightCalib = "/packages/config/right.yaml";
        //this->get_parameter("/home/nikita/stereo_ws/config/left.yaml", leftCalib);
        //this->get_parameter("/home/nikita/stereo_ws/config/right.yaml", rightCalib);
        //Publisher::get_parameter("/home/nikita/stereo_ws/config/left.yaml", leftCalib);
        //Publisher::get_parameter("/home/nikita/stereo_ws/config/right.yaml", rightCalib);
        camera_calibration_parsers::readCalibrationYml(leftCalib, tmp, infoLeftCam);
        camera_calibration_parsers::readCalibrationYml(rightCalib, tmp, infoRightCam);

        int expected_fps = 60;
        cam.open("tcp://10.10.11.2");
        publisher_left_ = this->create_publisher<sensor_msgs::msg::Image>("topic_left", 10);
        publisher_right_ = this->create_publisher<sensor_msgs::msg::Image>("topic_right", 10);
        publisher_left_info = this->create_publisher<sensor_msgs::msg::CameraInfo>("topic_left_info", 10);
        publisher_right_info = this->create_publisher<sensor_msgs::msg::CameraInfo>("topic_right_info", 10);
        timer_ = this->create_wall_timer(1ms, std::bind(&Publisher::timer_callback, this));
    }

private:
    void timer_callback()
    {
        Process();
    }
    void Process()
    {
        std::map<std::string, CvMat*> images = cam.receive();
        std_msgs::msg::Header h;

        h.stamp = rclcpp::Node::now();
        h.frame_id = "map";
        for (std::map<std::string, CvMat*>::iterator it = images.begin(); it != images.end(); ++it)
        {
            std::string name = it->first;
            cv::Mat img = cv::cvarrToMat(it->second);

            if (img.type() == CV_8UC2)
            {
                cv::cvtColor(img, img, cv::COLOR_YUV2BGR_YUYV);
            }

            if (name == "left")
            {
                infoLeftCam.header = h;
                sensor_msgs::msg::Image::SharedPtr msg = cv_bridge::CvImage(h, "bgr8", img).toImageMsg();
                publisher_left_->publish(*msg.get());
                publisher_left_info->publish(infoLeftCam);
            }
            if (name == "right")
            {
                infoRightCam.header = h;
                sensor_msgs::msg::Image::SharedPtr msg = cv_bridge::CvImage(h, "bgr8", img).toImageMsg();
                publisher_right_->publish(*msg.get());
                publisher_right_info->publish(infoRightCam);
            }
        }
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_left_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_right_;
    rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr publisher_left_info;
    rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr publisher_right_info;
    size_t count_;
    sensor_msgs::msg::CameraInfo infoLeftCam;
    sensor_msgs::msg::CameraInfo infoRightCam;
};

int main(int argc, char* argv[])
{   
    rclcpp::init(argc, argv);

    tbb::tbb_thread* ctrl_shell_thread = new tbb::tbb_thread(&ctrl_shell, &cam);

    rclcpp::spin(std::make_shared<Publisher>());

    is_stop_ctrl_shell = true;
    if (ctrl_shell_thread->joinable())
    {
        ctrl_shell_thread->join();
    }

    rclcpp::shutdown();

    return 0;
}

bool ctrl_shell(void *user_data)
{
	char buf[2000];
	while(!is_stop_ctrl_shell)
	{
		// Апишка теряет некоторые кадры. Из запрошенных 3х получается по 1 кадру с каждой камеры. ВСЕ СТРОЧКИ НУЖНЫ!
		char cmd[] = "left=3 right=3\0";
        cam.ctrl_send(cmd, strlen(cmd) + 1);
        cam.ctrl_recv(buf, 2000);
	}
	return true;
}