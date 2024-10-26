#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    
    
    float a = 0.0003;  
    float b = -0.3401;  
    float c = 97.9891;  

    VideoCapture cap(0);  
    if (!cap.isOpened()){
        return -1;
    }

    Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()){
            break;
        }

       
        Mat hsv_frame, mask;
        cvtColor(frame, hsv_frame, COLOR_BGR2HSV);

        Scalar lower_green(40, 40, 40);  
        Scalar upper_green(70, 255, 255);  
        inRange(hsv_frame, lower_green, upper_green, mask);

        GaussianBlur(mask, mask, Size(9, 9), 2);

        
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(mask, mask, MORPH_CLOSE, kernel);

        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        if (contours.size() > 0) {
            
            int largest_contour_index = 0;
            double max_area = contourArea(contours[0]);
            for (size_t i = 1; i < contours.size(); i++) {
                double area = contourArea(contours[i]);
                if (area > max_area) {
                    max_area = area;
                    largest_contour_index = i;
                }
            }

            
            Point2f center;
            float radius;
            minEnclosingCircle(contours[largest_contour_index], center, radius);

            
            circle(frame, center, (int)radius, Scalar(0, 255, 0), 2);  

            
            float object_pixel_width = 2 * radius;

            if (object_pixel_width > 0) {
                
                float calculated_distance = a * pow(object_pixel_width, 2) + b * object_pixel_width + c;

                
                cout << "Calculated Real-Life Distance (cm): " << calculated_distance << endl;
                // cout << "Pixel Width: " << object_pixel_width << endl;

               
                string distance_text = "Distance: " + to_string(calculated_distance) + " cm";
                // string pixel_width_text = "Pixel Width: " + to_string(object_pixel_width) + " px";

                
                putText(frame, distance_text, Point(center.x - 50, center.y - 20),
                        FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
                // putText(frame, pixel_width_text, Point(center.x - 50, center.y + 20),
                //         FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
            }
        }

        imshow("Camera", frame);
        // imshow("HSV Frame", hsv_frame);
        // imshow("Detected Green Object", mask);

        if (waitKey(30) == 'q') {
            break;
        }
    }

    
    cap.release();
    
   
    destroyAllWindows();

    return 0;
}