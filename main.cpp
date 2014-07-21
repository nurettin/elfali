#include <algorithm>
#include <iostream>
#include <functional>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

struct TrackbarCallbackData
{
	typedef std::function<void(int, void*)> fun_t;
	fun_t fun;
	void* data;
	TrackbarCallbackData(fun_t fun, void* data)
		: fun(fun)
		, data(data)
	{}
};

void trackbar_callback(int value, void* data)
{
	TrackbarCallbackData* d = reinterpret_cast<TrackbarCallbackData *>(data);
	d->fun(value, d-> data);
}

int main(int argc, char* argv[])
{
	using namespace cv;
	
	if (argc != 2)
		return EXIT_FAILURE;
	
	// 1. Load img
	Mat img = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);
	Size new_size = img.size();
	new_size.width /= 3;
	new_size.height /= 3;
	resize(img, img, new_size);
	namedWindow("Step 1", CV_WINDOW_AUTOSIZE);
	imshow("Step 1", img);

	// 2. Grayscale img2
	Mat img2;
	cvtColor(img, img2, CV_BGR2GRAY);

	// 3. Blur 3x3 img2
	blur(img2, img2, Size(3, 3));

  namedWindow("Step 11", CV_WINDOW_AUTOSIZE);
  int thresh= 80;
  TrackbarCallbackData tcd([&](int v, void* d){
    Mat out;
	  // 4. Threshold img2
    threshold(img2, out, v, 255, THRESH_BINARY);
    // 5. Find Contours img2
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(out, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    // 6. Get Longest (hand) Contour
    auto max_contour = std::max_element(contours.begin(), contours.end(), [](std::vector<Point> const &v1, std::vector<Point> const &v2){
      return v1.size()< v2.size();
    });
    if (max_contour == contours.end())
      throw std::runtime_error("No contours found!");
    auto max_contour_index = std::distance(contours.begin(), max_contour);

    // 7. Draw Contour filled img3
    Mat img3(out.size(), CV_8UC1, Scalar(0));
    drawContours(img3, contours, max_contour_index, Scalar(255), CV_FILLED);

    // 8. Use img3 as mask to copy img to img3
    img.copyTo(img3, img3);
    imshow("Step 11", img3);
    
  }, &img2);
  createTrackbar("Step 11 Thresh", "Step 11", &thresh, 255, &trackbar_callback, &tcd);
  tcd.fun(thresh, &img2);
  waitKey(0);
	destroyAllWindows();
  return EXIT_SUCCESS;
/*
	// 9. Convert to grayscale img3
  // GaussianBlur(img3, img3, Size(5,5), 0, 0, BORDER_DEFAULT );
	cvtColor(img3, img3, CV_BGR2GRAY);
  Laplacian(img3, img3, CV_8U, 5, 1, 0, BORDER_DEFAULT);
	fastNlMeansDenoising(img3, img3, 70);
  threshold(img3, img3, 70, 255, THRESH_BINARY);

  namedWindow("Step 11", CV_WINDOW_AUTOSIZE);
	imshow("Step 11", img3);
	
  // Remove thin lines
  
  waitKey(0);
	destroyAllWindows();*/
}
// http://stackoverflow.com/questions/17195325/palm-veins-enhancement-with-opencv
