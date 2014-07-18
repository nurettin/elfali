#include <algorithm>
#include <iostream>
#include <functional>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

	// 4. Threshold img2
	threshold(img2, img2, 125, 255, THRESH_BINARY);

	// 5. Find Contours img2
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(img2, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// 6. Get Longest (hand) Contour
	auto max_contour = std::max_element(contours.begin(), contours.end(), [](std::vector<Point> const &v1, std::vector<Point> const &v2){
		return v1.size()< v2.size();
	});
	if (max_contour == contours.end())
		throw std::runtime_error("No contours found!");
	auto max_contour_index = std::distance(contours.begin(), max_contour);

	// 7. Draw Contour filled img3
	Mat img3(img2.size(), CV_8UC1, Scalar(0));
	drawContours(img3, contours, max_contour_index, Scalar(255), CV_FILLED);

	// 8. Use img3 as mask to copy img to img3
	img.copyTo(img3, img3);

	// 9. Convert to grayscale img3
	cvtColor(img3, img3, CV_BGR2GRAY);

	// 10. Equalize histogram to make lines darker
	//equalizeHist(img3, img3);

	namedWindow("Step Test", CV_WINDOW_AUTOSIZE);
	imshow("Step Test", img3);

	threshold(img3, img3, 165, 255, cv::THRESH_BINARY);
	//Canny(img3, img3, 50, 200);

	contours.clear();
	hierarchy.clear();
	findContours(img3, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	std::sort(contours.begin(), contours.end(), [](std::vector<Point> const &v1, std::vector<Point> const &v2){
		return v1.size() > v2.size();
	});
	contours.resize(3);
	/// Draw contours
	Mat drawing = Mat::zeros(img3.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
		drawContours(img, contours, i, Scalar(0, 255, 0), 2, 8, hierarchy, 0, Point());
	
	namedWindow("Step Test 2", CV_WINDOW_AUTOSIZE);
	imshow("Step Test 2", img);

	waitKey(0);
	destroyAllWindows();
}
