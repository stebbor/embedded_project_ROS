#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
class ImageSubscriber : public rclcpp::Node
{
	public:
		ImageSubscriber() : Node("image_subscriber") {
			RCLCPP_INFO(this->get_logger(), "Initializing ImageSubscriber node");

			RCLCPP_INFO(this->get_logger(), "Starting camera subscription");

			camera_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
					"/image_raw",
					10,
					std::bind(&ImageSubscriber::onImageMsg, this, std::placeholders::_1)
			);

		}

	private:
		rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_subscription_;

		void onImageMsg(const sensor_msgs::msg::Image::SharedPtr msg) {
			RCLCPP_INFO(this->get_logger(), "Received image!");
			cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, msg->encoding);
			cv::Mat img = cv_ptr->image;
			cv::Mat greyimg;
			cv::cvtColor(img, greyimg, CV_BGR2GRAY);
			cv::Mat resized;
			cv::resize(greyimg,resized, cv::Size(20,20), cv::INTER_LINEAR);
			std::vector<uint8_t>vec(resized.begin<uint8_t>(), resized.end<uint8_t>());
			//RCLCPP_INFO(this->get_logger(), "IMG vector: '%f'",(float)vec.size());
			//for(unsigned long int i=0;i<vec.size();i++){
			//RCLCPP_INFO(this->get_logger(), "Vec: '%d'", vec[i]);}
			//RCLCPP_INFO(this->get_logger(),"Vector end")
			RCLCPP_INFO(this->get_logger(), "Successfully loaded image");
			}

};

int main(int argc, char *argv[])
{
	setvbuf(stdout,NULL,_IONBF,BUFSIZ);

	rclcpp::init(argc,argv);
	rclcpp::spin(std::make_shared<ImageSubscriber>());

	rclcpp::shutdown();
	return 0;
}
