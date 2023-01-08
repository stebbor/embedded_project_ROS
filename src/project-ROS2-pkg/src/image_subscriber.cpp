#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include </home/mp4d/ros2_ws/src/project-ROS2-pkg/src/BRAM-uio-driver/src/bram_uio.h>
#include </home/mp4d/ros2_ws/src/project-ROS2-pkg/src/test_images.h>
#include <std_msgs/msg/string.hpp>

class ImageSubscriber : public rclcpp::Node
{
	public:
		ImageSubscriber() : Node("image_subscriber"),writer(0, 400), reader(2, 400) {
			RCLCPP_INFO(this->get_logger(), "Initializing ImageSubscriber node");

			RCLCPP_INFO(this->get_logger(), "Starting camera subscription");

			camera_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
					"/image_raw",
					10,
					std::bind(&ImageSubscriber::onImageMsg, this, std::placeholders::_1)
			);
		servo_pos_pub_ = this->create_publisher<std_msgs::msg::String>("servo_pos", 10);

		}

	private:
		rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_subscription_;
		rclcpp::Publisher<std_msgs::msg::String>::SharedPtr servo_pos_pub_;		
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
			cv::Mat floatImage;
			resized.convertTo(floatImage, CV_32F, 1/255.0); 
			std::vector<float>vec(floatImage.begin<float>(), floatImage.end<float>());
			for(unsigned long int i=0;i<400;i++){
			//RCLCPP_INFO(this->get_logger(), "vec: '%f'", vec[i]);
			bram0_addr[i] = *(uint32_t *)(&vec[i]);
			}
			
		/*for(int n=0 ; n<20 ; n++){
                uint32_t imgToLoad[400];
                for(int i = 0 ; i<400 ; i++){
                        imgToLoad[i] = *(uint32_t *)(&images_array[n][i]);
			//RCLCPP_INFO(this->get_logger(), "image: '%d'", imgToLoad[i]); 
                }

                for(int i = 0 ; i<400 ; i++){
                        bram0_addr[i] = imgToLoad[i];
                }*/
		

			//RCLCPP_INFO(this->get_logger(), "IMG vector: '%f'",(float)vec.size());
			//for(unsigned long int j=0;j<vec.size();j++){
			//RCLCPP_INFO(this->get_logger(), "BRAM: '%d'", bram0_addr[j]);}
			//RCLCPP_INFO(this->get_logger(),"Vector end")
			//RCLCPP_INFO(this->get_logger(), "Successfully loaded image");
			usleep(10000);
			uint32_t predict = bram0_addr[512];
			RCLCPP_INFO(this->get_logger(), "Predict: '%d'", bram0_addr[512]);
			//}
			auto message = std_msgs::msg::String();
			message.data = std::to_string(predict);
			servo_pos_pub_->publish(message);
			usleep(100000);
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
