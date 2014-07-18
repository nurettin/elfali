#include <algorithm>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, char* argv[])
{
  // 1. Load img
  cv::Mat img= cv::imread("/home/nurettin/Downloads/database/IMG_001 (1).JPG", CV_LOAD_IMAGE_UNCHANGED);
  cv::Size new_size= img.size();
  new_size.width/= 3;
  new_size.height/= 3;
  cv::resize(img, img, new_size); 
  cv::namedWindow("Step 1", CV_WINDOW_AUTOSIZE);
  cv::imshow("Step 1", img);
  
  // 2. Grayscale img2
  cv::Mat img2;
  cv::cvtColor(img, img2, CV_BGR2GRAY);
  // 3. Blur 3x3 img2
  cv::blur(img2, img2, cv::Size(3, 3));
  // 4. Threshold 107 img2
  cv::threshold(img2, img2, 107, 255, cv::THRESH_BINARY);

  // 5. Find Contours img2
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(img2, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
  // 6. Get Longest Contour img2
  auto max_contour= std::max_element(contours.begin(), contours.end(), [](auto const &v1, auto const &v2){
    return v1.size()< v2.size();
  });
  if(max_contour== contours.end())
    throw std::runtime_error("No contours found!");
  auto max_contour_index= std::distance(contours.begin(), max_contour);

  // 7. Draw Contour filled img3
  cv::Mat img3(img2.size(), CV_8U, cv::Scalar(0));
  cv::drawContours(img3, contours, max_contour_index, cv::Scalar(255), CV_FILLED);

  // 8. Use img3 as mask to copy img to img3
  img.copyTo(img3, img3);

  cv::namedWindow("Step 8", CV_WINDOW_AUTOSIZE);
  cv::imshow("Step 8", img3);

  cv::waitKey(0);
}

