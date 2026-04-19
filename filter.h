//Name: Ahilesh Vadivel
//Date: 23rd Janurary 2026
//Description: This program captures video from the webcam and applies various filters based on user input.

#pragma once
#ifndef FILTER_H
#define FILTER_H

#include <opencv2/opencv.hpp>

//function to convert image to grayscale
int grayscale(cv::Mat& src, cv::Mat& dst);
//function to convert to sepia tone
int sepiaTone(cv::Mat& src, cv::Mat& dst);
//function to blur using a 5x5 gaussian filter
int blur5x5_1(cv::Mat& src, cv::Mat & dst);
//function to blur using a 5x5 gaussian filter optimized
int blur5x5_2(cv::Mat& src, cv::Mat& dst);
//function for sobel X and sobel Y
int sobelX3x3(cv::Mat &src, cv::Mat &dst);
int sobelY3x3(cv::Mat &src, cv::Mat &dst);
//function to generate a gradient magnitude image
int magnitude(cv::Mat& sx, cv::Mat& sy, cv::Mat& dst);
//function to blur and quantize a color image
int blurQuantize(cv::Mat& src, cv::Mat& dst, int levels);
//function to create negative of an image
int negative(cv::Mat& src, cv::Mat& dst);
//function to create emboss effect
int emboss(cv::Mat& src, cv::Mat& dst);
//function to create colorful face effect
int colorfulFace(cv::Mat& src, cv::Mat& dst);

#endif