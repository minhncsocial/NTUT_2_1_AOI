#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;

enum _State_ {
	STATE_OK = 0,
	STATE_EMPTY_IMAGE = 1
};

enum _ImageType_ {
	/* 8bit, color or not */
	IMAGE_UNCHANGED = -1,
	/* 8bit, gray */
	IMAGE_GRAYSCALE = 0,
	/* ?, color */
	IMAGE_COLOR = 1,
	/* any depth, ? */
	IMAGE_ANYDEPTH = 2,
	/* ?, any color */
	IMAGE_ANYCOLOR = 4,
	/* ?, no rotate */
	IMAGE_IGNORE_ORIENTATION = 128
};

enum _Channel {
	CM_BLUE = 0,
	CM_GREEN = 1,
	CM_RED = 2,
	CM_ALL = 3
};

namespace IProcess
{
	bool loadImage(char * file_name, _ImageType_ image_type, Mat * output_image);
	bool showImage(Mat input_image, char * window_name);
	bool findGroupMap(int16_t num_group, Mat input_image, Mat * output_group_map);
	bool findDistribution(Mat input_image, Mat * blue_hist, Mat * green_hist, Mat * red_hist, int16_t * number_distinct_values, int16_t * max_values);
	bool normalizeDistributions(Mat input_blue_hist, Mat input_green_hist, Mat input_red_hist, Mat histogram_image, Mat * output_blue_hist, Mat * output_green_hist, Mat * output_red_hist);
	bool plotHistogram(Mat * histogram_image, Mat blue_histogram, Mat green_histogram, Mat red_histogram, int histogram_size, int hist_height, int bin_width);
}

bool IProcess::loadImage(char * file_name, _ImageType_ image_type, Mat * output_image) {
	*output_image = imread(file_name, image_type);
	if (!(*output_image).data) {
		cout << "Could not open or find the image!!!" << endl;
		return false;
	}

	cout << "Load image successfully." << endl;
	cout << "Size: " << (*output_image).rows << "x" << (*output_image).cols << "x" << (*output_image).channels() << endl;
	/*imshow("aaa", *output_image);
	waitKey(0);*/
	return true;
}

bool IProcess::showImage(Mat input_image, char * window_name) {
	if (!input_image.data) {
		cout << "Image is not ready yet" << endl;
		return false;
	}

	cout << "Image showed ..." << endl;
	imshow(window_name, input_image);
	waitKey(0);
	cout << "Image closed ..." << endl;
	return true;
}

bool IProcess::findGroupMap(int16_t num_group, Mat input_image, Mat * output_group_map) {
	if (num_group < 1) {
		cout << "Number of group must be a positive integer number." << endl;
		return false;
	}

	*output_group_map = Mat::zeros(input_image.rows, input_image.cols, input_image.type());

	int16_t range_length = 256 / num_group;

	for (int row = 0; row < input_image.rows; row++) {
		for (int col = 0; col < input_image.cols; col++) {
			for (int channel = 0; channel < 3; channel++) {
				(*output_group_map).at<Vec3b>(row, col)[channel] = input_image.at<Vec3b>(row, col)[channel] / range_length;
			}
		}
	}

	return true;
}

bool IProcess::findDistribution(Mat input_image, Mat * blue_hist, Mat * green_hist, Mat * red_hist, int16_t * number_distinct_values, int16_t * max_value) {
	cout << "\nFind number of distinct values ..." << endl;
	*number_distinct_values = 0;
	*max_value = -1;
	Mat value_flag = Mat(255, 1, CV_8UC1, Scalar(0, 0));
	for (int row = 0; row < input_image.rows; row++) {
		for (int col = 0; col < input_image.cols; col++) {
			for (int channel = 0; channel < input_image.channels(); channel++) {
				int16_t value = input_image.at<Vec3b>(row, col)[channel];
				if (!value_flag.at<uchar>(value, 0)) {
					value_flag.at<uchar>(value, 0) = 1;
					*number_distinct_values += 1;
					if (value > *max_value) *max_value = value;
				}
			}
		}
	}
	cout << "Number of distinct values is: " << *number_distinct_values << endl;
	cout << "Max values is: " << *max_value << endl;

	cout << "Find distribution ..." << endl;
	/// Establish the number of bins
	int histSize = *max_value;

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(input_image, bgr_planes);

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, histSize };
	const float* histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), *blue_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), *green_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), *red_hist, 1, &histSize, &histRange, uniform, accumulate);

	cout << "Find distribution successfully." << endl;

	return true;
}

bool IProcess::normalizeDistributions(Mat input_blue_hist, Mat input_green_hist, Mat input_red_hist, Mat histogram_image, Mat * output_blue_hist, Mat * output_green_hist, Mat * output_red_hist) {
	cout << "Normalize distribution ..." << endl;
	/// Normalize the result to [ 0, histImage.rows ]
	normalize(input_blue_hist, *output_blue_hist, 0, histogram_image.rows, NORM_MINMAX, -1, Mat());
	normalize(input_green_hist, *output_green_hist, 0, histogram_image.rows, NORM_MINMAX, -1, Mat());
	normalize(input_red_hist, *output_red_hist, 0, histogram_image.rows, NORM_MINMAX, -1, Mat());

	cout << "Normalize distribution successfully." << endl;

	return true;
}

bool IProcess::plotHistogram(Mat * histogram_image, Mat blue_histogram, Mat green_histogram, Mat red_histogram, int histogram_size, int hist_height, int bin_width) {
	cout << "\nPlot histogram ..." << endl;

	/// Draw for each channel
	for (int i = 0; i < histogram_size; i++)
	{
		line(*histogram_image,
			Point(bin_width*(i), hist_height),
			Point(bin_width*(i), hist_height - cvRound(blue_histogram.at<float>(i))),
			Scalar(255, 0, 0), bin_width / 10, 8, 0);
		line(*histogram_image,
			Point(bin_width*(i) +bin_width / 10, hist_height),
			Point(bin_width*(i) +bin_width / 10, hist_height - cvRound(green_histogram.at<float>(i))),
			Scalar(0, 255, 0), bin_width / 10, 8, 0);
		line(*histogram_image,
			Point(bin_width*(i) + 2 * bin_width / 10, hist_height),
			Point(bin_width*(i) + 2 * bin_width / 10, hist_height - cvRound(red_histogram.at<float>(i))),
			Scalar(0, 0, 255), bin_width / 10, 8, 0);

		if (!i) continue;

		line(*histogram_image, Point(bin_width*(i - 1), hist_height - cvRound(blue_histogram.at<float>(i - 1))),
			Point(bin_width*(i), hist_height - cvRound(blue_histogram.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(*histogram_image, Point(bin_width*(i - 1), hist_height - cvRound(green_histogram.at<float>(i - 1))),
			Point(bin_width*(i), hist_height - cvRound(green_histogram.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(*histogram_image, Point(bin_width*(i - 1), hist_height - cvRound(red_histogram.at<float>(i - 1))),
			Point(bin_width*(i), hist_height - cvRound(red_histogram.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}

	cout << "Plot histogram successfully" << endl;
	return true;
}

int main(int args, char ** argv) {
	printf("Hello \n");

	Mat image, groupMap;
	/*image = imread("fox.jpg", CV_LOAD_IMAGE_COLOR);*/
	IProcess::loadImage("fox.jpg", IMAGE_COLOR, &image);
	//IProcess::showImage(image, "Original Image");
	IProcess::findGroupMap(128, image, &groupMap);
	//IProcess::showImage(groupMap*60, "Grouped Image");

	// Find distribution
	int histSize = 2;
	Mat b_hist, g_hist, r_hist;
	int16_t numberDistinctValue, maxValue;
	IProcess::findDistribution(groupMap, &b_hist, &g_hist, &r_hist, &numberDistinctValue, &maxValue);

	// Draw the histograms for B, G and R
	int hist_w = 512; 
	int hist_h = 400;
	int bin_w = cvRound((double)hist_w / maxValue);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	Mat norm_b_hist, norm_g_hist, norm_r_hist;
	IProcess::normalizeDistributions(b_hist, g_hist, r_hist, histImage, &norm_b_hist, &norm_g_hist, &norm_r_hist);

	// Plot histogram
	IProcess::plotHistogram(&histImage, norm_b_hist, norm_g_hist, norm_r_hist, maxValue, hist_h, bin_w);

	// Show histogram image
	IProcess::showImage(histImage, "Histogram");

	return 0;
}
// Some useful Opencv commands
//cout << image.size() << image.type() << endl;
//Mat temp = Mat::zeros(2, 3, CV_8UC3);
//Mat temp1 = Mat(2, 3, CV_8UC3, Scalar(0, 0, 0));
//temp1 = temp1 * 10;
//cout << temp1 << endl;