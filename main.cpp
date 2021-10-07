#include <iostream>
#include "opencv2/opencv.hpp"

int main() {
	cv::VideoCapture cam(0);
	if (!cam.isOpened()) {
		throw std::runtime_error("Error");
	}

	cv::namedWindow("Window");
	while (true) {
		cv::Mat frame;
		cam >> frame;
		cv::resize(frame, frame, cv::Size(400, 400));
		cv::imshow("bgr_frame", frame);
		cv::Mat gray_frame;
		cv::cvtColor(frame, gray_frame, CV_BGR2GRAY);
		v::imshow("gray_frame", gray_frame);
		if (cv::waitKey(30) >= 0) break;
	}
}
