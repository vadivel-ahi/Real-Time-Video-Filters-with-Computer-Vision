//Name: Ahilesh Vadivel
//Date: 23rd Janurary 2026
//Description: This program captures video from the webcam and applies various filters based on user input.

#include <opencv2/opencv.hpp>
#include <iostream>
#include "filter.h"
#include "faceDetect.h"

int main(int argc, char* argv[]) {
    cv::VideoCapture* capdev;

    // open the video device
    capdev = new cv::VideoCapture(0);
    if (!capdev->isOpened()) {
        printf("Unable to open video device\n");
        return(-1);
    }
    
    // get some properties of the image
    cv::Size refS((int)capdev->get(cv::CAP_PROP_FRAME_WIDTH),
        (int)capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);

    cv::namedWindow("Video", 1); // identifies a window
    cv::Mat frame;

    bool grayscale_mode = false; //state variable to keep track of the mode
    bool custom_grayscale_mode = false; //state variable for custom grayscale mode
    bool sepia_mode = false; //state variable for sepia mode
    bool blur_mode = false; //state variable for blur mode
    bool sobel_x_mode = false; //state variable for sobel mode
    bool sobel_y_mode = false;
    bool magnitude_mode = false; //state variable for gradient magnitude mode
    bool blur_quantize_mode = false; //state variable for blur quantize mode
    bool face_detect_mode = false; //state variable for face detection
    bool depth_mode = false; //state variable for depth mode
	bool negative_mode = false; //state variable for negative mode
	bool emboss_mode = false; //state variable for emboss mode
	bool colorful_face_mode = false; //state variable for colorful face mode

    // This code snippet added to include the ability to load an image from the system and 
    // perform the functions. (Needed for HW 4!)
    // Add this near the top with other state variables
    bool image_mode = false;
    cv::Mat loaded_image;
    std::string image_path = "";

    // Add this before the main loop starts (after window creation)
    if (argc > 1) {
        // If command line argument provided, load that image
        image_path = argv[1];
        loaded_image = cv::imread(image_path);
        if (!loaded_image.empty()) {
            image_mode = true;
            std::cout << "Loaded image: " << image_path << std::endl;
            std::cout << "Press 'v' to switch back to video mode" << std::endl;
        }
        else {
            std::cout << "Failed to load image: " << image_path << std::endl;
        }
    }

    for (;;) {
        /**capdev >> frame; // get a new frame from the camera, treat as a stream
        if (frame.empty()) {
            printf("frame is empty\n");
            break;
        }*/

        if (image_mode) {
            frame = loaded_image.clone();  // Use loaded image
        }
        else {
            *capdev >> frame;  // Get from camera
            if (frame.empty()) {
                printf("frame is empty\n");
                break;
            }
        }

        // converting to grayscale or to color according to the state variable
        cv::Mat display_frame;
        if (grayscale_mode) {
            cv::cvtColor(frame, display_frame, cv::COLOR_BGR2GRAY);
        }
        else if (custom_grayscale_mode) {
            grayscale(frame, display_frame); //using custom grayscale function
        }
        else if (sepia_mode) {
            sepiaTone(frame, display_frame); //sepia function
        }
        else if (blur_mode) {
            blur5x5_2(frame, display_frame);
        }
        else if (sobel_x_mode) {
            cv::Mat sobelOutput;
            sobelX3x3(frame, sobelOutput);
            // Convert signed short to unsigned char for display
            cv::convertScaleAbs(sobelOutput, display_frame);
        }
        else if (sobel_y_mode) {
            cv::Mat sobelOutput;
            sobelY3x3(frame, sobelOutput);
            //converting signed short to unsigned char for display
            cv::convertScaleAbs(sobelOutput, display_frame);
        }
        else if (magnitude_mode) {
            //calculate both sobel X and Y
            cv::Mat sobelX, sobelY;
            sobelX3x3(frame, sobelX);
            sobelY3x3(frame, sobelY);

            //calculate magnitude
            magnitude(sobelX, sobelY, display_frame);

            if (display_frame.empty()) {
                std::cout << "Error: magnitude error" << std::endl;
                display_frame = frame;
            }
        }
        else if (blur_quantize_mode) {
            blurQuantize(frame, display_frame, 10);  // Use 10 levels as default

            if (display_frame.empty()) {
                std::cout << "Error: blurQuantize failed!" << std::endl;
                display_frame = frame;
            }
        }
        else if (face_detect_mode) {
            //converting frame to grayscale for face detection
            cv::Mat grey;
            cv::cvtColor(frame, grey, cv::COLOR_BGR2GRAY);

            //vector to store detected faces
            std::vector<cv::Rect> faces;

            //detect faces in the grayscale image
            detectFaces(grey, faces);

            //copy frame to display_frame
            display_frame = frame.clone();

            //draw boxes around detected faces
            drawBoxes(display_frame, faces);

            //print number of faces detected
            if (faces.size() > 0) {
                std::cout << "Detected " << faces.size() << " face(s)" << std::endl;
            }
        }
        else if (negative_mode) {
            negative(frame, display_frame);
        }
        else if (emboss_mode) {
            emboss(frame, display_frame);
        }
        else if (colorful_face_mode) {
            colorfulFace(frame, display_frame);
        }
        else {
            display_frame = frame; //using the original frame
        }

        cv::imshow("Video", display_frame); //displaying the processed frame

        // see if there is a waiting keystroke
        char key = cv::waitKey(10);
        if (key == 'q') {
            break;
        }
        else if (key == 's') {
            cv::imwrite("captured_frame.jpg", display_frame);
            std::cout << "Frame saved as captured_frame.jpg" << std::endl;
        }
        // checking for command to change to greyscale or color
        else if (key == 'g') {
            grayscale_mode = !grayscale_mode; //toggle between true/false
            std::cout << "Grayscale mode:" << (grayscale_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'h') {
            custom_grayscale_mode = !custom_grayscale_mode;
            std::cout << "Custom Grayscale mode:" << (custom_grayscale_mode ? "ON" : "OFF");
        }
        else if (key == 'p') {
            sepia_mode = !sepia_mode;
            std::cout << "Sepia mode" << (sepia_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'b') {
            blur_mode = !blur_mode;
            std::cout << "Blur mode" << (blur_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'x') {
            sobel_x_mode = !sobel_x_mode;
            sobel_y_mode = false;
            std::cout << "Sobel X mode" << (sobel_x_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'y') {
            sobel_y_mode = !sobel_y_mode;
            sobel_x_mode = false;
            std::cout << "Sobel Y mode" << (sobel_y_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'm') {
            magnitude_mode = !magnitude_mode;
            std::cout << "Magnitude mode:" << (magnitude_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'l') {
            blur_quantize_mode = !blur_quantize_mode;
            std::cout << "Blur-Quantize mode: " << (blur_quantize_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'f') {
            face_detect_mode = !face_detect_mode;
            std::cout << "Face detection mode: " << (face_detect_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'n') {
            negative_mode = !negative_mode;
            std::cout << "Negative mode: " << (negative_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'e') {
            emboss_mode = !emboss_mode;
            std::cout << "Emboss mode: " << (emboss_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'k') {  // 'k' for colorful (c is taken)
            colorful_face_mode = !colorful_face_mode;
            std::cout << "Colorful Face mode: " << (colorful_face_mode ? "ON" : "OFF") << std::endl;
        }
        else if (key == 'i') {
            std::cout << "Enter image path: ";
            std::string path;
            
            std::getline(std::cin, path);  // Just use getline without ignore first

            // Remove surrounding quotes if present
            if (path.length() >= 2 && path.front() == '"' && path.back() == '"') {
                path = path.substr(1, path.length() - 2);
            }

            cv::Mat temp_image = cv::imread(path);
            if (!temp_image.empty()) {
                loaded_image = temp_image;
                image_mode = true;
                std::cout << "Image loaded successfully!" << std::endl;
            }
            else {
                std::cout << "Failed to load image!" << std::endl;
            }
        }
        else if (key == 'v') {
            image_mode = false;
            std::cout << "Switched to video mode" << std::endl;
        }
    }

    delete capdev;
    return(0);
}