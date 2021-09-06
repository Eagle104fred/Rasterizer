#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H

#include "global.h"
#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>

class Texture {
private:
	cv::Mat imageData;
public:
	Texture(const std::string&name)
	{
		imageData = cv::imread(name);
		cv::cvtColor(imageData, imageData, cv::COLOR_RGB2BGR);
		width = imageData.cols;
		height = imageData.rows;
	}

	int width, height;

	Eigen::Vector3f GetColor(float u, float v)
	{
		auto uImg = u * width;
		auto vImg = v * height;
		auto color = imageData.at<cv::Vec3b>(vImg, uImg);//KS: 提取image中对应坐标的RGB 
		return Eigen::Vector3f(color[0], color[1], color[2]);
	}
};

#endif