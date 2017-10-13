#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

enum _Channel {
	CM_BLUE = 0,
	CM_GREEN = 1,
	CM_RED = 2,
	CM_ALL = 3
};

class CMImage {
public:
	CMImage(int16_t num_rows, int16_t num_cols);
	~CMImage();
	bool SetSize(int16_t num_rows, int16_t num_cols);
	void GetSize(int16_t * rows, int16_t * cols);
	bool AssignImageValue(int16_t value);
	bool GetImageFromMat(Mat image);
	bool AssignImageToMat(Mat image);
	bool IncreaseIntensity(int16_t value, _Channel selected_channel);
	bool DecreaseIntensity(int16_t value, _Channel selected_channel);
	void DisplayImageValue(void);
	void DisplayImage(char * display_name);

private:
	int16_t *** mImage;
	int16_t mChannels, mRows, mCols;

};

CMImage::CMImage(int16_t num_rows, int16_t num_cols) {
	mChannels = 3;
	mRows = num_rows;
	mCols = num_cols;

	mImage = new int16_t**[mChannels];
	for (int channel = 0; channel < mChannels; channel++) {
		mImage[channel] = new int16_t*[mRows];
		for (int row = 0; row < num_rows; row++) {
			mImage[channel][row] = new int16_t[num_cols];
		}
	}

	AssignImageValue(0);

	cout << "Image is created" << endl;
	cout << "Image Size : " << mRows << "x" << mCols << endl;
}

CMImage::~CMImage() {
	for (int channel = 0; channel < mChannels; channel++) {
		for (int row = 0; row < mRows; row++) {
			delete[] mImage[channel][row];
		}
		delete[] mImage[channel];
	}
	delete[] mImage;

	mRows = NULL;
	mCols = NULL;
	mChannels = NULL;

	cout << "Image is destroyed" << endl;
}

bool CMImage::SetSize(int16_t num_rows, int16_t num_cols) {
	mRows = num_rows;
	mCols = num_cols;

	return true;
}

void CMImage::GetSize(int16_t * rows, int16_t * cols) {
	*rows = mRows;
	*cols = mCols;
}

bool CMImage::AssignImageValue(int16_t value) {
	if (value < 0) {
		cout << "Assigned value must be bigger than 0" << endl;
		return false;
	}
	else if (value > 255) {
		cout << "Assigned value must be smaller than 255" << endl;
		return false;
	}

	for (int channel = 0; channel < mChannels; channel++) {
		for (int row = 0; row < mRows; row++) {
			for (int col = 0; col < mCols; col++) mImage[channel][row][col] = value;
		}
	}

	return true;
}

bool CMImage::GetImageFromMat(Mat image) {
	if ((image.rows != mRows) || (image.cols != mCols)) {
		cout << "Size is not compatible to Image" << endl;
		return false;
	}

	for (int row = 0; row < image.rows; row++) {
		for (int col = 0; col < image.cols; col++) {
			mImage[0][row][col] = image.at<Vec3b>(row, col)[0];
			mImage[1][row][col] = image.at<Vec3b>(row, col)[1];
			mImage[2][row][col] = image.at<Vec3b>(row, col)[2];
		}
	}

	return true;
}

bool CMImage::AssignImageToMat(Mat image) {
	if ((image.rows != mRows) || (image.cols != mCols)) {
		cout << "Size is not compatible to Image" << endl;
		return false;
	}

	for (int row = 0; row < image.rows; row++) {
		for (int col = 0; col < image.cols; col++) {
			image.at<Vec3b>(row, col)[0] = mImage[0][row][col];
			image.at<Vec3b>(row, col)[1] = mImage[1][row][col];
			image.at<Vec3b>(row, col)[2] = mImage[2][row][col];
		}
	}

	return true;
}

bool CMImage::IncreaseIntensity(int16_t value, _Channel selected_channel) {
	if (value < 0) {
		cout << "Assigned value must be bigger than 0" << endl;
		return false;
	}
	else if (value > 255) {
		cout << "Assigned value must be smaller than 255" << endl;
		return false;
	}

	for (int channel = 0; channel < mChannels; channel++) {
		if ((selected_channel == CM_ALL) || (channel == selected_channel)) {
			for (int row = 0; row < mRows; row++) {
				for (int col = 0; col < mCols; col++) {
					mImage[channel][row][col] += value;
					mImage[channel][row][col] = mImage[channel][row][col] > 255 ? 255 : mImage[channel][row][col];
				}
			}
		}
	}

	return true;
}

bool CMImage::DecreaseIntensity(int16_t value, _Channel selected_channel) {
	if (value < 0) {
		cout << "Assigned value must be bigger than 0" << endl;
		return false;
	}
	else if (value > 255) {
		cout << "Assigned value must be smaller than 255" << endl;
		return false;
	}

	for (int channel = 0; channel < mChannels; channel++) {
		if ((selected_channel == CM_ALL) || (channel == selected_channel)) {
			for (int row = 0; row < mRows; row++) {
				for (int col = 0; col < mCols; col++) {
					mImage[channel][row][col] -= value;
					mImage[channel][row][col] = mImage[channel][row][col] < 0 ? 0 : mImage[channel][row][col];
				}
			}
		}
	}

	return true;
}

void CMImage::DisplayImageValue(void) {
	for (int channel = 0; channel < mChannels; channel++) {
		cout << "Channel " << channel << " : " << endl;
		for (int row = 0; row < mRows; row++) {
			for (int col = 0; col < mCols; col++) cout << mImage[channel][row][col] << " ";
			cout << endl;
		}
	}
}

void CMImage::DisplayImage(char * display_name) {
	Mat cvImage(mRows, mCols, CV_8UC3, Scalar(0, 0, 0));

	AssignImageToMat(cvImage);
	namedWindow(display_name, WINDOW_AUTOSIZE);
	imshow(display_name, cvImage);
	waitKey(0);
}

int main(int argc, char ** argv) {
	cout << "Hallo! This is the homework 1 program" << endl;

	Mat cvImage;
	cvImage = imread("fox.jpg", CV_LOAD_IMAGE_COLOR);

	//1st requirement: Load Image to 3D matrix
	CMImage cmImage(cvImage.rows, cvImage.cols); // init 3D matrix
	cmImage.GetImageFromMat(cvImage); // load value from image to 3D matrix
	//cmImage.DisplayImageValue(); // display 3D matrix to console

	//4th requirement: Display image before processing
	cmImage.DisplayImage("Before processing");

	//2nd requirement: Add value to channel Red
	cmImage.IncreaseIntensity(28, CM_RED);

	//3nd requirement: Minus value to channel Green
	cmImage.DecreaseIntensity(30, CM_GREEN);

	//4th requirement: Display image after processing
	cmImage.DisplayImage("After processing");

	//system("pause");
	return 0;
}