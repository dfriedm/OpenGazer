#include "GraphicalPointer.h"

int WindowPointer::windowNumber;

WindowPointer::PointerSpec::PointerSpec(int width, int height, int red, int green, int blue) 
: width(width), height(height), red(red), green(green), blue(blue)
{ }

WindowPointer::WindowPointer(const PointerSpec &spec) {
	stringstream stream;
	stream << "Window" << WindowPointer::windowNumber++;
	name = stream.str();
	IplImage *image = cvCreateImage(cvSize(spec.width, spec.height), IPL_DEPTH_8U, 3);
	cvCircle(image, cvPoint(spec.width/2, spec.height/2), spec.width/2, cvScalar(spec.red, spec.green, spec.blue), CV_FILLED);
	cvNamedWindow(name.c_str(), CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
	cvShowImage(name.c_str(), image);
}

void WindowPointer::setPosition(int x, int y) {
	cvMoveWindow(name.c_str(), x, y);
}
