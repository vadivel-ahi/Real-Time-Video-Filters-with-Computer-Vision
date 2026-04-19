//Name: Ahilesh Vadivel
//Date: 23rd Janurary 2026
//Description: This program contains various image filter functions to be applied on video frames.

#include "filter.h"
#include "faceDetect.h"

int grayscale(cv::Mat& src, cv::Mat& dst) {
	/*
	function to convert a color image to grayscale
	arguments:
		src - input color image (BGR format)
		dst - output grayscale image (BGR format with equal channels)
	returns:
		0 on success
	*/
	//creating destinatin image with same size and type as source
	dst.create(src.size(), src.type());

	for (int i = 0; i < src.rows; i++) {
		cv::Vec3b* srcptr = src.ptr<cv::Vec3b>(i);
		cv::Vec3b* dstptr = dst.ptr<cv::Vec3b>(i);

		for (int j = 0; j < src.cols; j++) {
			uchar blue = srcptr[j][0];
			uchar green = srcptr[j][1];
			uchar red = srcptr[j][2];

			//calculating simple average
			uchar grey = 0.7 * blue + 0.1 * green + 0.2 * red;

			//setting all three channels to the same grey value
			dstptr[j] = cv::Vec3b(grey, grey, grey);
		}
	}
	return 0;
}

int sepiaTone(cv::Mat& src, cv::Mat& dst) {
	/*
	function to convert a color image to sepia tone
	arguments:
		src - input color image (BGR format)
		dst - output sepia tone image (BGR format)
	returns:
		0 on success
	*/
	//creating output image
	dst.create(src.size(), src.type());

	for (int i = 0; i < src.rows; i++) {
		cv::Vec3b* srcptr = src.ptr<cv::Vec3b>(i);
		cv::Vec3b* dstptr = dst.ptr<cv::Vec3b>(i);

		for (int j = 0; j < src.cols; j++) {
			uchar orig_blue = srcptr[j][0];
			uchar orig_green = srcptr[j][1];
			uchar orig_red = srcptr[j][2];

			//calculating distance from center
			float center_x = src.cols / 2.0;
			float center_y = src.rows / 2.0;
			float dx = j - center_x;
			float dy = i - center_y;
			float distance = sqrt(dx * dx + dy * dy);
			float max_distance = sqrt(center_x * center_x + center_y * center_y);

			//creating vignette factor
			float vignette = 1.0 - (distance / max_distance);
			vignette = vignette * vignette; //squaring for stronger effect

			//calculating new values using original
			float new_blue = 0.272 * orig_red + 0.534 * orig_green + 0.131 * orig_blue;
			float new_green = 0.349 * orig_red + 0.686 * orig_green + 0.168 * orig_blue;
			float new_red = 0.393 * orig_red + 0.769 * orig_green + 0.189 * orig_blue;

			//applying vignette effect
			new_blue *= vignette;
			new_green *= vignette;
			new_red *= vignette;

			//clamping values to 0 - 255 range
			if (new_blue > 255) new_blue = 255;
			if (new_green > 255) new_green = 255;
			if (new_red > 255) new_red = 255;

			dstptr[j] = cv::Vec3b((uchar)new_blue, (uchar)new_green, (uchar)new_red);
		}
	}
	return 0;
}

int blur5x5_1(cv::Mat& src, cv::Mat& dst) {
	/*
	function to blur an image using a 5x5 Gaussian filter
	arguments:
		src - input color image (BGR format)
		dst - output blurred image (BGR format)
	returns:
		0 on success
	*/
	//copying src to dst
	dst = src.clone();
	//defining filter
	int kernel[5][5] = {
		{1,2,4,2,1},
		{2,4,8,4,2},
		{4,8,16,8,4},
		{2,4,8,4,2},
		{1,2,4,2,1}
	};
	int kernelSum = 100;

	//processing only the inner pixels (leaving 2-pixel border)
	for (int i = 2; i < src.rows - 2; i++) {
		for (int j = 2; j < src.cols - 2; j++) {
			int sumBlue = 0;
			int sumGreen = 0;
			int sumRed = 0;

			//applying the 5x5 kernel
			for (int ki = -2; ki <= 2; ki++) {
				for (int kj = -2; kj <= 2; kj++) {
					//getting pixel at offset (ki,kj) from the current position
					cv::Vec3b pixel = src.at<cv::Vec3b>(i + ki, j + kj);

					//getting kernel weight
					int weight = kernel[ki + 2][kj + 2];

					//accumulating weights for each channel
					sumBlue += pixel[0] * weight;
					sumGreen += pixel[1] * weight;
					sumRed += pixel[2] * weight;
				}
			}

			//normalize by dividing by kernel sum and storing it to destination image
			dst.at<cv::Vec3b>(i, j) = cv::Vec3b(
				sumBlue / kernelSum,
				sumGreen / kernelSum,
				sumRed / kernelSum
			);
		}
	}

	return 0;
}

int blur5x5_2(cv::Mat& src, cv::Mat& dst) {
	/*
	function to blur an image using a 5x5 Gaussian filter (optimized using separable kernels)
	arguments:
		src - input color image (BGR format)
		dst - output blurred image (BGR format)
	returns:
		0 on success
	*/
	//creating temporary image for intermediate results
	cv::Mat temp;
	temp.create(src.size(), src.type());
	dst.create(src.size(), src.type());

	//1D kernel
	int kernel[5] = { 1,2,4,2,1 };
	int kernelSum = 10;

	//performing horizontal blur
	for (int i = 0; i < src.rows; i++) {
		cv::Vec3b *srcRow = src.ptr<cv::Vec3b>(i); //pointers
		cv::Vec3b *tempRow = temp.ptr<cv::Vec3b>(i);

		for (int j = 2; j < src.cols - 2; j++) {
			int sumBlue = 0;
			int sumGreen = 0;
			int sumRed = 0;

			//applying horizontal kernel
			for (int k = -2; k <= 2; k++) {
				cv::Vec3b pixel = srcRow[j + k];
				int weight = kernel[k + 2];

				sumBlue += pixel[0] * weight;
				sumGreen += pixel[1] * weight;
				sumRed += pixel[2] * weight;
			}

			tempRow[j] = cv::Vec3b(
				sumBlue / kernelSum,
				sumGreen / kernelSum,
				sumRed / kernelSum
			);
		}

		//copy border pixels
		tempRow[0] = srcRow[0];
		tempRow[1] = srcRow[1];
		tempRow[src.cols - 2] = srcRow[src.cols - 2];
		tempRow[src.cols - 1] = srcRow[src.cols - 1];
	}

	//performing vertical blur
	for (int i = 2; i < temp.rows - 2; i++) {
		cv::Vec3b* dstRow = dst.ptr<cv::Vec3b>(i);

		for (int j = 0; j < temp.cols; j++) {
			int sumBlue = 0;
			int sumGreen = 0;
			int sumRed = 0;

			//applying Vertical kernel
			for (int k = -2; k <= 2; k++) {
				cv::Vec3b pixel = temp.ptr<cv::Vec3b>(i + k)[j];
				int weight = kernel[k + 2];

				sumBlue += pixel[0] * weight;
				sumGreen += pixel[1] * weight;
				sumRed += pixel[2] * weight;
			}

			dstRow[j] = cv::Vec3b(
				sumBlue / kernelSum,
				sumGreen / kernelSum,
				sumRed / kernelSum
			);
		}
	}

	temp.row(0).copyTo(dst.row(0));
	temp.row(1).copyTo(dst.row(1));
	temp.row(src.rows - 2).copyTo(dst.row(src.rows - 2));
	temp.row(src.rows - 1).copyTo(dst.row(src.rows - 1));

	return 0;
}

int sobelX3x3(cv::Mat& src, cv::Mat& dst) {
	/*
	function to compute Sobel X gradient using 3x3 kernel
	arguments:
		src - input color image (BGR format)
		dst - output gradient image (16-bit signed, 3 channels)
	results:
		0 on success
	*/
	//creating output as 16-bit signed, 3 channels
	dst.create(src.size(), CV_16SC3);

	//temporary image for intermediate results
	cv::Mat temp;
	temp.create(src.size(), CV_16SC3);

	//step1: applying [1,2,1] vertical 
	for (int i = 1; i < src.rows - 1; i++) {
		short* tempRow = temp.ptr<short>(i);

		for (int j = 0; j < src.cols; j++) {
			//getting pixels above and below
			cv::Vec3b top = src.ptr<cv::Vec3b>(i - 1)[j];
			cv::Vec3b mid = src.ptr<cv::Vec3b>(i)[j];
			cv::Vec3b bot = src.ptr<cv::Vec3b>(i + 1)[j];

			//apply [1,2,1] kernel to each channel
			for (int c = 0; c < 3; c++) {
				tempRow[j * 3 + c] = top[c] + 2 * mid[c] + bot[c];
			}
		}
	}

	//handling top and bottom border rows
	for (int j = 0; j < src.cols; j++) {
		cv::Vec3b pixel_top = src.ptr<cv::Vec3b>(0)[j];
		cv::Vec3b pixel_bot = src.ptr<cv::Vec3b>(src.rows - 1)[j];

		short* temp_top = temp.ptr<short>(0);
		short* temp_bot = temp.ptr<short>(src.rows - 1);

		for (int c = 0; c < 3; c++) {
			temp_top[j * 3 + c] = pixel_top[c] * 4;
			temp_bot[j * 3 + c] = pixel_bot[c] * 4;
		}
	}

	//step2: applying [-1,0,1] horizontally
	for (int i = 0; i < temp.rows; i++) {
		short* dstRow = dst.ptr<short>(i);
		short* tempRow = temp.ptr<short>(i);

		for (int j = 1; j < temp.cols - 1; j++) {
			//getting left and right pixels from temp
			for (int c = 0; c < 3; c++) {
				short left = tempRow[(j - 1) * 3 + c];
				short right = tempRow[(j + 1) * 3 + c];

				//apply [-1,0,1] kernel
				dstRow[j * 3 + c] = -left + right; // -1*left + 0*mid + 1*right
			}
		}
	}

	//handling left and right border columns
	for (int i = 0; i < dst.rows; i++) {
		short* dstRow = dst.ptr<short>(i);
		for (int c = 0; c < 3; c++) {
			dstRow[0 * 3 + c] = 0; //Left border
			dstRow[(dst.cols - 1) * 3 + c] = 0; //Right border
		}
	}

	return 0;
}

int sobelY3x3(cv::Mat& src, cv::Mat& dst) {
	/*
	function to compute Sobel Y gradient using 3x3 kernel
	arguments:
		src - input color image (BGR format)
		dst - output gradient image (16-bit signed, 3 channels)
	results:
		0 on success
	*/
	//creating output as 16-bit signed, 3 channels
	dst.create(src.size(), CV_16SC3);

	//temporary image for intermediate results
	cv::Mat temp;
	temp.create(src.size(), CV_16SC3);

	//step1: applying [1,2,1] horizontal
	for (int i = 0; i < src.rows; i++) {
		cv::Vec3b* srcRow = src.ptr<cv::Vec3b>(i);
		short* tempRow = temp.ptr<short>(i);

		for (int j = 1; j < src.cols - 1; j++) {
			//getting pixels left and right
			cv::Vec3b left = srcRow[j - 1];
			cv::Vec3b mid = srcRow[j];
			cv::Vec3b right = srcRow[j + 1];

			//apply [1,2,1] kernel to each channel
			for (int c = 0; c < 3; c++) {
				tempRow[j * 3 + c] = left[c] + 2 * mid[c] + right[c];
			}
		}
	}

	//handling left and right border columns
	for (int i = 0; i < src.rows; i++) {
		cv::Vec3b pixel_left = src.ptr<cv::Vec3b>(i)[0];
		cv::Vec3b pixel_right = src.ptr<cv::Vec3b>(i)[src.cols - 1];

		short* tempRow = temp.ptr<short>(i);

		for (int c = 0; c < 3; c++) {
			tempRow[0 * 3 + c] = pixel_left[c] * 4;
			tempRow[(src.cols - 1) * 3 + c] = pixel_right[c] * 4;
		}
	}

	//step2: applying [-1,0,1] vertically
	for (int i = 1; i < temp.rows - 1; i++) {
		short* dstRow = dst.ptr<short>(i);

		for (int j = 0; j < temp.cols; j++) {
			//getting top and bottom pixels from temp
			short* topRow = temp.ptr<short>(i - 1);
			short* botRow = temp.ptr<short>(i + 1);

			for (int c = 0; c < 3; c++) {
				short top = topRow[j * 3 + c];
				short bot = botRow[j * 3 + c];

				//apply [-1,0,1] kernel
				dstRow[j * 3 + c] = top - bot; // 1*top + 0*mid + (-1)*bot
			}
		}
	}

	//handling left and right border columns
	for (int j = 0; j < dst.cols; j++) {
		short* top_row = dst.ptr<short>(0);
		short* bot_row = dst.ptr<short>(dst.rows - 1);

		for (int c = 0; c < 3; c++) {
			top_row[j * 3 + c] = 0;  // Top border
			bot_row[j * 3 + c] = 0;  // Bottom border
		}
	}

	return 0;
}

int magnitude(cv::Mat& sx, cv::Mat& sy, cv::Mat& dst) {
	/*
	function to compute gradient magnitude from Sobel X and Y images
	arguments:
		sx - input Sobel X image (16-bit signed, 3 channels)
		sy - input Sobel Y image (16-bit signed, 3 channels)
		dst - output magnitude image (8-bit unsigned, 3 channels)
	returns:
		0 on success
	*/
	//verify inputs are the same size
	if (sx.size() != sy.size()) {
		std::cerr << "Error: sx and sy must be the same size" << std::endl;
		return -1;
	}

	//verifying if inputs are CV_16SC3
	if (sx.type() != CV_16SC3 || sy.type() != CV_16SC3) {
		std::cerr << "Error: sx and sy must be CV_16SC3" << std::endl;
		return -1;
	}

	//creating output as unsigned char, 3 channels (ready for display)
	dst.create(sx.size(), CV_8UC3);

	for (int i = 0; i < sx.rows; i++) {
		//pointers for rows
		cv::Vec3s* sxRow = sx.ptr<cv::Vec3s>(i);
		cv::Vec3s* syRow = sy.ptr<cv::Vec3s>(i);
		cv::Vec3b* dstRow = dst.ptr<cv::Vec3b>(i);

		for (int j = 0; j < sx.cols; j++) {
			//getting sobel X and sobel Y values
			cv::Vec3s sx_pixel = sxRow[j];
			cv::Vec3s sy_pixel = syRow[j];

			//calculating magnitude for each channel
			for (int c = 0; c < 3; c++) {
				//getting gradient of X and Y for the channel
				short gx = sx_pixel[c];
				short gy = sy_pixel[c];

				//calculating Euclidean magnitude
				double mag = sqrt(gx * gx + gy * gy);

				//clamp to 0 - 255 range
				if (mag > 255) {
					mag = 255;
				}

				dstRow[j][c] = (uchar)mag;
			}
		}
	}

	return 0;
}

int blurQuantize(cv::Mat& src, cv::Mat& dst, int levels) {
	/*
	function to blur and quantize a color image
	arguments:
		src - input color image (BGR format)
		dst - output blurred and quantized image (BGR format)
		levels - number of quantization levels per channel
	returns:
		0 on success
	*/
	//bluring the image first
	cv::Mat blurred;
	blur5x5_2(src, blurred);

	//creating output image
	dst.create(blurred.size(), blurred.type());

	//calculating bucket size
	float b = 255.0f / levels;

	//quantizing each pixel
	for (int i = 0; i < blurred.rows; i++) {
		cv::Vec3b* blurredRow = blurred.ptr<cv::Vec3b>(i);
		cv::Vec3b* dstRow = dst.ptr<cv::Vec3b>(i);

		for (int j = 0; j < blurred.cols; j++) {
			cv::Vec3b pixel = blurredRow[j];

			//quantize each channel
			for (int c = 0; c < 3; c++) {
				uchar x = pixel[c];
				int xt = (int)(x / b);
				uchar xf = (uchar)(xt * b);

				dstRow[j][c] = xf;
			}
		}
	}

	return 0;
}

int negative(cv::Mat& src, cv::Mat& dst) {
	/*
	function to create negative of an image
	arguments:
		src - input color image (BGR format)
		dst - output negative image (BGR format)
	returns:
		0 on success
	*/
	dst.create(src.size(), src.type());

	for (int i = 0; i < src.rows; i++) {
		cv::Vec3b* srcRow = src.ptr<cv::Vec3b>(i);
		cv::Vec3b* dstRow = dst.ptr<cv::Vec3b>(i);

		for (int j = 0; j < src.cols; j++) {
			//invert each channel: new = 255 - old
			dstRow[j][0] = 255 - srcRow[j][0];  // Blue
			dstRow[j][1] = 255 - srcRow[j][1];  // Green
			dstRow[j][2] = 255 - srcRow[j][2];  // Red
		}
	}

	return 0;
}

int emboss(cv::Mat& src, cv::Mat& dst) {
	/*
	function to create emboss effect
	arguments:
		src - input color image (BGR format)
		dst - output embossed image (BGR format)
	returns:
		0 on success
	*/
	//get Sobel X and Y gradients
	cv::Mat sobelX, sobelY;
	sobelX3x3(src, sobelX);  
	sobelY3x3(src, sobelY); 

	dst.create(src.size(), CV_8UC3);

	const float light_x = 0.7071; 
	const float light_y = 0.7071;  

	for (int i = 0; i < src.rows; i++) {
		cv::Vec3s* sxRow = sobelX.ptr<cv::Vec3s>(i);
		cv::Vec3s* syRow = sobelY.ptr<cv::Vec3s>(i);
		cv::Vec3b* dstRow = dst.ptr<cv::Vec3b>(i);

		for (int j = 0; j < src.cols; j++) {
			//process each channel
			for (int c = 0; c < 3; c++) {
				//dot product: gradient · light_direction
				float dot = sxRow[j][c] * light_x + syRow[j][c] * light_y;

				//map to 0-255 range (add 128 to center around gray)
				int value = (int)(dot / 4.0) + 128;  //divide by 4 to scale down

				//clamp to valid range
				if (value < 0) value = 0;
				if (value > 255) value = 255;

				dstRow[j][c] = (uchar)value;
			}
		}
	}

	return 0;
}

int colorfulFace(cv::Mat& src, cv::Mat& dst) {
	/*
	function to create colorful face effect
	arguments:
		src - input color image (BGR format)
		dst - output image with colorful faces (BGR format)
	returns:
		0 on success
	*/
	//convert to grayscale first
	cv::Mat gray_version;
	cv::cvtColor(src, gray_version, cv::COLOR_BGR2GRAY);

	//convert back to 3-channel for consistency
	cv::cvtColor(gray_version, dst, cv::COLOR_GRAY2BGR);

	//detect faces
	cv::Mat grey;
	cv::cvtColor(src, grey, cv::COLOR_BGR2GRAY);
	std::vector<cv::Rect> faces;
	detectFaces(grey, faces);

	//for each detected face, restore color
	for (const auto& face : faces) {
		for (int i = face.y; i < face.y + face.height && i < src.rows; i++) {
			cv::Vec3b* srcRow = src.ptr<cv::Vec3b>(i);
			cv::Vec3b* dstRow = dst.ptr<cv::Vec3b>(i);

			for (int j = face.x; j < face.x + face.width && j < src.cols; j++) {
				//restore original color in face region
				dstRow[j] = srcRow[j];
			}
		}

		//draw a subtle border around the face
		cv::rectangle(dst, face, cv::Scalar(255, 255, 0), 2);
	}

	return 0;
}