#include <ros/ros.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <image_segment/target.h>

#define TRANSFORM_MATRIX_FILE "/home/nimpng/pos_transform_matrix.xml"

cv::Mat R, t;
cv::Mat pos_in_robot_frame;
float factor = 1.0;

bool mat_init(void)
{
    cv::FileStorage fs(TRANSFORM_MATRIX_FILE, cv::FileStorage::READ);
    try
    {
        fs["matR"] >> R;
        fs["matT"] >> t;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

void callback(const image_segment::targetConstPtr &tar_pos)
{
    cv::Mat pos = cv::Mat::zeros(3, 1, CV_64FC1);
    pos.at<double>(0, 0) = tar_pos->x / factor;
    pos.at<double>(1, 0) = tar_pos->y / factor;
    pos.at<double>(2, 0) = tar_pos->z / factor;
    pos_in_robot_frame = R * pos + t;
    std::cout << "pos = " << pos << std::endl;
    std::cout << "pos_in_robot" << pos_in_robot_frame << std::endl;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "coor_in_robot_node");
    ros::NodeHandle nh;
    if (!mat_init())
        return -1;
    ros::Subscriber sub = nh.subscribe<image_segment::target>("/target_position", 1, callback);
    ros::spin();
    return 0;
}