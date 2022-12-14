#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include </home/mp4d/ros2_ws/src/project-ROS2-pkg/src/BRAM-uio-driver/src/bram_uio.h>
class ImageSubscriber : public rclcpp::Node
{
	public:
		ImageSubscriber() : Node("image_subscriber"),writer(1, 400), reader(2, 400) {
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
		BRAM writer;
		BRAM reader;
		void onImageMsg(const sensor_msgs::msg::Image::SharedPtr msg) {
			uint32_t *bram0_addr = writer.GetPointer();
  			//uint32_t *bram1_addr = reader.GetPointer();
			RCLCPP_INFO(this->get_logger(), "Received image!");
			cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, msg->encoding);
			cv::Mat img = cv_ptr->image;
			cv::Mat greyimg;
			cv::cvtColor(img, greyimg, CV_BGR2GRAY);
			cv::Mat resized;
			cv::resize(greyimg,resized, cv::Size(20,20), cv::INTER_LINEAR);
			std::vector<uint8_t>vec(resized.begin<uint8_t>(), resized.end<uint8_t>());
			for(unsigned long int i=0;i<vec.size();i++){
			bram0_addr[i] = (uint32_t)vec[i];
			}
			//RCLCPP_INFO(this->get_logger(), "IMG vector: '%f'",(float)vec.size());
			for(unsigned long int j=0;j<vec.size();j++){
			RCLCPP_INFO(this->get_logger(), "BRAM: '%d'", bram0_addr[j]);}
			//RCLCPP_INFO(this->get_logger(),"Vector end")
			RCLCPP_INFO(this->get_logger(), "Successfully loaded image");
			}

};

int main(int argc, char *argv[])
{
	setvbuf(stdout,NULL,_IONBF,BUFSIZ);
	//BRAM writer;
	//BRAM reader;

	rclcpp::init(argc,argv);
	rclcpp::spin(std::make_shared<ImageSubscriber>());

	rclcpp::shutdown();
	return 0;
}
